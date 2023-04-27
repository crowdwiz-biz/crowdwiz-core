/*
 * Copyright (c) 2015 Cryptonomex, Inc., and contributors.
 *
 * The MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <graphene/chain/database.hpp>
#include <graphene/chain/db_with.hpp>

#include <graphene/chain/asset_object.hpp>
#include <graphene/chain/global_property_object.hpp>
#include <graphene/chain/hardfork.hpp>
#include <graphene/chain/market_object.hpp>
#include <graphene/chain/gamezone_object.hpp>
#include <graphene/chain/exchange_object.hpp>
#include <graphene/chain/proposal_object.hpp>
#include <graphene/chain/financial_object.hpp>
#include <graphene/chain/greatrace_object.hpp>
#include <graphene/chain/transaction_object.hpp>
#include <graphene/chain/withdraw_permission_object.hpp>
#include <graphene/chain/witness_object.hpp>

#include <graphene/chain/protocol/fee_schedule.hpp>

#include <fc/uint128.hpp>
#include <iostream>


namespace graphene { namespace chain {

share_type count_prize(share_type a)
{
   uint16_t p = 9000;
   if (a == 0)
      return 0;
   fc::uint128 r(a.value);
   r *= p*2;
   r /= GRAPHENE_100_PERCENT;
   return r.to_uint64();
}

share_type count_referral_prize(share_type a)
{
   uint16_t p = 1000;
   if (a == 0)
      return 0;
   fc::uint128 r(a.value);
   r *= p*2;
   r /= GRAPHENE_100_PERCENT;
   return r.to_uint64();
}

void database::update_global_dynamic_data( const signed_block& b, const uint32_t missed_blocks )
{
   const dynamic_global_property_object& _dgp = get_dynamic_global_properties();

   // dynamic global properties updating
   modify( _dgp, [&b,this,missed_blocks]( dynamic_global_property_object& dgp ){
      const uint32_t block_num = b.block_num();
      if( BOOST_UNLIKELY( block_num == 1 ) )
         dgp.recently_missed_count = 0;
      else if( _checkpoints.size() && _checkpoints.rbegin()->first >= block_num )
         dgp.recently_missed_count = 0;
      else if( missed_blocks )
         dgp.recently_missed_count += GRAPHENE_RECENTLY_MISSED_COUNT_INCREMENT*missed_blocks;
      else if( dgp.recently_missed_count > GRAPHENE_RECENTLY_MISSED_COUNT_INCREMENT )
         dgp.recently_missed_count -= GRAPHENE_RECENTLY_MISSED_COUNT_DECREMENT;
      else if( dgp.recently_missed_count > 0 )
         dgp.recently_missed_count--;

      dgp.head_block_number = block_num;
      dgp.head_block_id = b.id();
      dgp.time = b.timestamp;
      dgp.current_witness = b.witness;
      dgp.recent_slots_filled = (
           (dgp.recent_slots_filled << 1)
           + 1) << missed_blocks;
      dgp.current_aslot += missed_blocks+1;
   });

   if( !(get_node_properties().skip_flags & skip_undo_history_check) )
   {
      GRAPHENE_ASSERT( _dgp.head_block_number - _dgp.last_irreversible_block_num  < GRAPHENE_MAX_UNDO_HISTORY, undo_database_exception,
                 "The database does not have enough undo history to support a blockchain with so many missed blocks. "
                 "Please add a checkpoint if you would like to continue applying blocks beyond this point.",
                 ("last_irreversible_block_num",_dgp.last_irreversible_block_num)("head", _dgp.head_block_number)
                 ("recently_missed",_dgp.recently_missed_count)("max_undo",GRAPHENE_MAX_UNDO_HISTORY) );
   }

   _undo_db.set_max_size( _dgp.head_block_number - _dgp.last_irreversible_block_num + 1 );
   _fork_db.set_max_size( _dgp.head_block_number - _dgp.last_irreversible_block_num + 1 );
}

void database::update_signing_witness(const witness_object& signing_witness, const signed_block& new_block)
{
   const global_property_object& gpo = get_global_properties();
   const dynamic_global_property_object& dpo = get_dynamic_global_properties();
   uint64_t new_block_aslot = dpo.current_aslot + get_slot_at_time( new_block.timestamp );

   share_type witness_pay = std::min( gpo.parameters.witness_pay_per_block, dpo.witness_budget );

   modify( dpo, [&]( dynamic_global_property_object& _dpo )
   {
      _dpo.witness_budget -= witness_pay;
   } );

   deposit_witness_pay( signing_witness, witness_pay );

   modify( signing_witness, [&]( witness_object& _wit )
   {
      _wit.last_aslot = new_block_aslot;
      _wit.last_confirmed_block_num = new_block.block_num();
   } );
}

void database::update_last_irreversible_block()
{
   const global_property_object& gpo = get_global_properties();
   const dynamic_global_property_object& dpo = get_dynamic_global_properties();

   // TODO for better performance, move this to db_maint, because only need to do it once per maintenance interval
   vector< const witness_object* > wit_objs;
   wit_objs.reserve( gpo.active_witnesses.size() );
   for( const witness_id_type& wid : gpo.active_witnesses )
      wit_objs.push_back( &(wid(*this)) );

   static_assert( GRAPHENE_IRREVERSIBLE_THRESHOLD > 0, "irreversible threshold must be nonzero" );

   // 1 1 1 2 2 2 2 2 2 2 -> 2     .3*10 = 3
   // 1 1 1 1 1 1 1 2 2 2 -> 1
   // 3 3 3 3 3 3 3 3 3 3 -> 3
   // 3 3 3 4 4 4 4 4 4 4 -> 4

   size_t offset = ((GRAPHENE_100_PERCENT - GRAPHENE_IRREVERSIBLE_THRESHOLD) * wit_objs.size() / GRAPHENE_100_PERCENT);

   std::nth_element( wit_objs.begin(), wit_objs.begin() + offset, wit_objs.end(),
      []( const witness_object* a, const witness_object* b )
      {
         return a->last_confirmed_block_num < b->last_confirmed_block_num;
      } );

   uint32_t new_last_irreversible_block_num = wit_objs[offset]->last_confirmed_block_num;

   if( new_last_irreversible_block_num > dpo.last_irreversible_block_num )
   {
      modify( dpo, [&]( dynamic_global_property_object& _dpo )
      {
         _dpo.last_irreversible_block_num = new_last_irreversible_block_num;
      } );
   }
}

void database::clear_expired_transactions()
{ try {
   //Look for expired transactions in the deduplication list, and remove them.
   //Transactions must have expired by at least two forking windows in order to be removed.
   auto& transaction_idx = static_cast<transaction_index&>(get_mutable_index(implementation_ids, impl_transaction_object_type));
   const auto& dedupe_index = transaction_idx.indices().get<by_expiration>();
   while( (!dedupe_index.empty()) && (head_block_time() > dedupe_index.begin()->trx.expiration) )
      transaction_idx.remove(*dedupe_index.begin());
} FC_CAPTURE_AND_RETHROW() }

void database::clear_expired_proposals()
{
   const auto& proposal_expiration_index = get_index_type<proposal_index>().indices().get<by_expiration>();
   while( !proposal_expiration_index.empty() && proposal_expiration_index.begin()->expiration_time <= head_block_time() )
   {
      const proposal_object& proposal = *proposal_expiration_index.begin();
      processed_transaction result;
      try {
         if( proposal.is_authorized_to_execute(*this) )
         {
            result = push_proposal(proposal);
            //TODO: Do something with result so plugins can process it.
            continue;
         }
      } catch( const fc::exception& e ) {
         elog("Failed to apply proposed transaction on its expiration. Deleting it.\n${proposal}\n${error}",
              ("proposal", proposal)("error", e.to_detail_string()));
      }
      remove(proposal);
   }
}

/**
 *  let HB = the highest bid for the collateral  (aka who will pay the most DEBT for the least collateral)
 *  let SP = current median feed's Settlement Price 
 *  let LC = the least collateralized call order's swan price (debt/collateral)
 *
 *  If there is no valid price feed or no bids then there is no black swan.
 *
 *  A black swan occurs if MAX(HB,SP) <= LC
 */
bool database::check_for_blackswan( const asset_object& mia, bool enable_black_swan,
                                    const asset_bitasset_data_object* bitasset_ptr )
{
    if( !mia.is_market_issued() ) return false;

    const asset_bitasset_data_object& bitasset = ( bitasset_ptr ? *bitasset_ptr : mia.bitasset_data(*this) );
    if( bitasset.has_settlement() ) return true; // already force settled
    auto settle_price = bitasset.current_feed.settlement_price;
    if( settle_price.is_null() ) return false; // no feed

    const call_order_index& call_index = get_index_type<call_order_index>();
    const auto& call_price_index = call_index.indices().get<by_price>();

    auto call_min = price::min( bitasset.options.short_backing_asset, mia.id );
    auto call_max = price::max( bitasset.options.short_backing_asset, mia.id );
    auto call_itr = call_price_index.lower_bound( call_min );
    auto call_end = call_price_index.upper_bound( call_max );

    if( call_itr == call_end ) return false;  // no call orders

    price highest = settle_price;

    const auto& dyn_prop = get_dynamic_global_properties();
    auto maint_time = dyn_prop.next_maintenance_time;
    if( maint_time > HARDFORK_CORE_338_TIME )
       // due to #338, we won't check for black swan on incoming limit order, so need to check with MSSP here
       highest = bitasset.current_feed.max_short_squeeze_price();

    const limit_order_index& limit_index = get_index_type<limit_order_index>();
    const auto& limit_price_index = limit_index.indices().get<by_price>();

    // looking for limit orders selling the most USD for the least CORE
    auto highest_possible_bid = price::max( mia.id, bitasset.options.short_backing_asset );
    // stop when limit orders are selling too little USD for too much CORE
    auto lowest_possible_bid  = price::min( mia.id, bitasset.options.short_backing_asset );

    FC_ASSERT( highest_possible_bid.base.asset_id == lowest_possible_bid.base.asset_id );
    // NOTE limit_price_index is sorted from greatest to least
    auto limit_itr = limit_price_index.lower_bound( highest_possible_bid );
    auto limit_end = limit_price_index.upper_bound( lowest_possible_bid );

    if( limit_itr != limit_end ) {
       FC_ASSERT( highest.base.asset_id == limit_itr->sell_price.base.asset_id );
       highest = std::max( limit_itr->sell_price, highest );
    }

    auto least_collateral = call_itr->collateralization();
    if( ~least_collateral >= highest  ) 
    {
       wdump( (*call_itr) );
       elog( "Black Swan detected on asset ${symbol} (${id}) at block ${b}: \n"
             "   Least collateralized call: ${lc}  ${~lc}\n"
           //  "   Highest Bid:               ${hb}  ${~hb}\n"
             "   Settle Price:              ${~sp}  ${sp}\n"
             "   Max:                       ${~h}  ${h}\n",
            ("id",mia.id)("symbol",mia.symbol)("b",head_block_num())
            ("lc",least_collateral.to_real())("~lc",(~least_collateral).to_real())
          //  ("hb",limit_itr->sell_price.to_real())("~hb",(~limit_itr->sell_price).to_real())
            ("sp",settle_price.to_real())("~sp",(~settle_price).to_real())
            ("h",highest.to_real())("~h",(~highest).to_real()) );
       edump((enable_black_swan));
       FC_ASSERT( enable_black_swan, "Black swan was detected during a margin update which is not allowed to trigger a blackswan" );
       if( maint_time > HARDFORK_CORE_338_TIME && ~least_collateral <= settle_price )
          // global settle at feed price if possible
          globally_settle_asset(mia, settle_price );
       else
          globally_settle_asset(mia, ~least_collateral );
       return true;
    } 
    return false;
}

void database::auto_reset_status()
{

   auto head_time = head_block_time();
   auto& acc_idx = get_index_type<account_index>().indices().get<by_status_expiration>();
   auto itr = acc_idx.upper_bound(fc::time_point_sec(1));
   auto end = acc_idx.lower_bound( head_time );

   while( itr != end )
   {
      elog( "account status expired  ${a}", ("a", itr->name) );
      elog( "account status expired  ${a}", ("a", itr->referral_status_expiration_date) );
      const account_object& acc_obj = *itr;
      const auto& global_properties = get_global_properties();
      const auto& params = global_properties.parameters;

      modify(acc_obj, [&](account_object& a)
      {
         a.referral_status_type = 0;
         a.referral_status_paid_fee = 0;
         a.referral_status_expiration_date = fc::time_point_sec();
         a.referral_levels = params.status_levels_00;
         a.status_denominator = params.status_denominator_00;
      });
      itr++;
   }

}

void database::auto_cancel_p2p_orders()
{
	auto head_time = head_block_time();
   if ( head_time >= HARDFORK_CWD3_TIME ) {
      const auto& po_idx_approve = get_index_type<p2p_order_index>().indices().get<by_status>();
      auto itr0 = po_idx_approve.equal_range(1);
      vector<p2p_order_object> orders_to_remove_1;

      std::for_each(itr0.first, itr0.second, [&](const p2p_order_object& current_p2p_order) {
         if (current_p2p_order.time_for_approve < head_time && current_p2p_order.status == 1 ) {
            // elog( "auto_cancel_p2p_orders, only cancel (status 1), head_time  ${e}", ("e", head_time) );
            // elog( "auto_cancel_p2p_orders, only cancel (status 1), current_order  ${e}", ("e", current_p2p_order.time_for_approve) );
            orders_to_remove_1.emplace_back(current_p2p_order);
         }
      });
      if (orders_to_remove_1.size()>0) {
         // elog( "auto_cancel_p2p_orders, only cancel (status 1), orders_to_remove_1 before remove  ${e}", ("e", orders_to_remove_1) );
         for( auto order : orders_to_remove_1 ) {
            
            // elog( "auto_cancel_p2p_orders, only cancel (status 1), full_order  ${e}", ("e", order) );
            const auto& p2p_gateway_stats = get_account_stats_by_owner(order.p2p_gateway);

            modify(p2p_gateway_stats, [&](account_statistics_object& p2p_gateway_statistics)
            {
               p2p_gateway_statistics.p2p_canceled_deals++;
               if (p2p_gateway_statistics.p2p_current_month_rating > 0) {
                  p2p_gateway_statistics.p2p_current_month_rating--;
               }
            });
         
            autocancel_p2p_order_operation cancel_order_op;
            cancel_order_op.p2p_order = order.id;
            cancel_order_op.p2p_gateway = order.p2p_gateway;
            cancel_order_op.p2p_client = order.p2p_client;

            push_applied_operation( cancel_order_op );
            remove(get_object(order.id));
            // elog( "auto_cancel_p2p_orders, only cancel (status 1), orders_to_remove_1 after remove  ${e}", ("e", orders_to_remove_1) );
         }
      }

      
      const auto& po_idx_reply = get_index_type<p2p_order_index>().indices().get<by_status>();
      auto itr1 = po_idx_reply.equal_range(2);
      vector<p2p_order_object> orders_to_remove_2;
      std::for_each(itr1.first, itr1.second, [&](const p2p_order_object& current_p2p_order) {
         if (current_p2p_order.time_for_reply < head_time  && current_p2p_order.status == 2) {
            // elog( "auto_cancel_p2p_orders, cancel and refund (status 2), head_time  ${e}", ("e", head_time) );
            // elog( "auto_cancel_p2p_orders, cancel and refund (status 2), current_order  ${e}", ("e", current_p2p_order.time_for_reply) );
            orders_to_remove_2.emplace_back(current_p2p_order);
         }
      });

      if (orders_to_remove_2.size()>0) {
         // elog( "auto_cancel_p2p_orders, cancel and refund (status 2), orders_to_remove_2 before remove  ${e}", ("e", orders_to_remove_2) );
         for( auto order : orders_to_remove_2 ) {

            const auto& p2p_client_stats = get_account_stats_by_owner(order.p2p_client);
            const auto& p2p_gateway_stats = get_account_stats_by_owner(order.p2p_gateway);
            
            autorefund_p2p_order_operation cancel_order_op;
            cancel_order_op.p2p_gateway = order.p2p_gateway;
            cancel_order_op.p2p_client = order.p2p_client;
            cancel_order_op.p2p_order = order.id;
            cancel_order_op.amount = order.amount;

            if (order.order_type) {
               uint16_t first_ban_time = (12*3600);
               modify(p2p_client_stats, [&](account_statistics_object& p2p_client_statistics)
               {
                  p2p_client_statistics.p2p_canceled_deals++;
                  p2p_client_statistics.p2p_banned = head_time + first_ban_time;
                  if (p2p_client_statistics.p2p_current_month_rating > 0) {
                     p2p_client_statistics.p2p_current_month_rating--;
                  }
               });
               adjust_balance( order.p2p_gateway, order.amount );
               cancel_order_op.refund_to = order.p2p_gateway;
            } else {
               modify(p2p_gateway_stats, [&](account_statistics_object& p2p_gateway_statistics)
               {
                  p2p_gateway_statistics.p2p_canceled_deals++;
                  if (p2p_gateway_statistics.p2p_current_month_rating > 0) {
                     p2p_gateway_statistics.p2p_current_month_rating--;
                  }                  
               });
               adjust_balance( order.p2p_client, order.amount );
               cancel_order_op.refund_to = order.p2p_client;
            };

            push_applied_operation( cancel_order_op );
            remove(get_object(order.id));
            // elog( "auto_cancel_p2p_orders, cancel and refund (status 2), orders_to_remove_2 after remove  ${e}", ("e", orders_to_remove_2) );

         }
      }
   }
   else {
      const auto& po_idx_approve = get_index_type<p2p_order_index>().indices().get<by_status>();
      auto itr0 = po_idx_approve.equal_range(1);

      std::for_each(itr0.first, itr0.second, [&](const p2p_order_object& current_p2p_order) {
         if (current_p2p_order.time_for_approve < head_time) {
            // elog( "auto_cancel_p2p_orders, only cancel (status 1), head_time  ${e}", ("e", head_time) );
            // elog( "auto_cancel_p2p_orders, only cancel (status 1), current_order  ${e}", ("e", current_p2p_order) );

            const auto& p2p_gateway_stats = get_account_stats_by_owner(current_p2p_order.p2p_gateway);

            modify(p2p_gateway_stats, [&](account_statistics_object& p2p_gateway_statistics)
            {
               p2p_gateway_statistics.p2p_canceled_deals++;
               if (p2p_gateway_statistics.p2p_current_month_rating > 0) {
                  p2p_gateway_statistics.p2p_current_month_rating--;
               }
            });
         
            autocancel_p2p_order_operation cancel_order_op;
            cancel_order_op.p2p_order = current_p2p_order.id;
            cancel_order_op.p2p_gateway = current_p2p_order.p2p_gateway;
            cancel_order_op.p2p_client = current_p2p_order.p2p_client;

            push_applied_operation( cancel_order_op );
            remove(current_p2p_order);
         }
      });
      
      const auto& po_idx_reply = get_index_type<p2p_order_index>().indices().get<by_status>();
      auto itr1 = po_idx_reply.equal_range(2);

      std::for_each(itr1.first, itr1.second, [&](const p2p_order_object& current_p2p_order) {
         if (current_p2p_order.time_for_reply < head_time) {
            // elog( "auto_cancel_p2p_orders, cancel and refund (status 2), head_time  ${e}", ("e", head_time) );
            // elog( "auto_cancel_p2p_orders, cancel and refund (status 2), current_order  ${e}", ("e", current_p2p_order) );
            const auto& p2p_client_stats = get_account_stats_by_owner(current_p2p_order.p2p_client);
            const auto& p2p_gateway_stats = get_account_stats_by_owner(current_p2p_order.p2p_gateway);
            
            autorefund_p2p_order_operation cancel_order_op;
            cancel_order_op.p2p_gateway = current_p2p_order.p2p_gateway;
            cancel_order_op.p2p_client = current_p2p_order.p2p_client;
            cancel_order_op.p2p_order = current_p2p_order.id;
            cancel_order_op.amount = current_p2p_order.amount;

            if (current_p2p_order.order_type) {
               uint16_t first_ban_time = (12*3600);
               modify(p2p_client_stats, [&](account_statistics_object& p2p_client_statistics)
               {
                  p2p_client_statistics.p2p_canceled_deals++;
                  p2p_client_statistics.p2p_banned = head_time + first_ban_time;
                  if (p2p_client_statistics.p2p_current_month_rating > 0) {
                     p2p_client_statistics.p2p_current_month_rating--;
                  }
               });
               adjust_balance( current_p2p_order.p2p_gateway, current_p2p_order.amount );
               cancel_order_op.refund_to = current_p2p_order.p2p_gateway;
            } else {
               modify(p2p_gateway_stats, [&](account_statistics_object& p2p_gateway_statistics)
               {
                  p2p_gateway_statistics.p2p_canceled_deals++;
                  if (p2p_gateway_statistics.p2p_current_month_rating > 0) {
                     p2p_gateway_statistics.p2p_current_month_rating--;
                  }
               });
               adjust_balance( current_p2p_order.p2p_client, current_p2p_order.amount );
               cancel_order_op.refund_to = current_p2p_order.p2p_client;
            };

            push_applied_operation( cancel_order_op );
            remove(current_p2p_order);
         }
      });      
   }

}


void database::proceed_bets()
{ try {
   auto head_time = head_block_time();

   auto& flipcoin_idx = get_index_type<flipcoin_index>().indices().get<by_expiration>();
   while( !flipcoin_idx.empty() && flipcoin_idx.begin()->expiration <= head_time && flipcoin_idx.begin()->status < 2 )
   {
      auto block_id = head_block_id();
      const flipcoin_object& flipcoin = *flipcoin_idx.begin();
      uint8_t check_nonce = 8+flipcoin.nonce;
      if (check_nonce>39) {
         check_nonce = 39;
      }
      std::string id_substr = std::string(block_id).substr(check_nonce,1);
      if(flipcoin.status == 0) {
         flipcoin_log( "proceed_bets: CANCEL FLIPCOIN: ${b}", ("b", flipcoin.id) );
         adjust_balance( flipcoin.bettor, flipcoin.bet );
         flipcoin_cancel_operation vop_cancel;
         vop_cancel.flipcoin = flipcoin.id;
         vop_cancel.bettor = flipcoin.bettor;
         vop_cancel.bet = flipcoin.bet;
         push_applied_operation( vop_cancel );

         remove(flipcoin);
      }
      if (flipcoin.status == 1) {
         flipcoin_log( "proceed_bets: FLIP FLIPCOIN: ${b}", ("b", flipcoin.id) );
         flipcoin_log( "proceed_bets: block ID: ${b}", ("b", block_id) );
         flipcoin_log( "proceed_bets: block NUM: ${b}", ("b", head_block_num()) );
         flipcoin_log( "proceed_bets: block TIME: ${b}", ("b", head_time) );
         flipcoin_log( "proceed_bets: ID SUBSTRING: ${b}", ("b", id_substr) );
         bool heads;
         switch(id_substr[0])
               {
                     case '0': heads = true; break;
                     case '1': heads = false; break;
                     case '2': heads = true; break;
                     case '3': heads = false; break;
                     case '4': heads = true; break;
                     case '5': heads = false; break;
                     case '6': heads = true; break;
                     case '7': heads = false; break;
                     case '8': heads = true; break;
                     case '9': heads = false; break;
                     case 'a': heads = true; break;
                     case 'b': heads = false; break;
                     case 'c': heads = true; break;
                     case 'd': heads = false; break;
                     case 'e': heads = true; break;
                     case 'f': heads = false; break;
               }
         flipcoin_log( "proceed_bets: HEADS: ${b}", ("b", heads) );
         flipcoin_log( "proceed_bets: flipcoin: ${b}", ("b", flipcoin ) );
         asset prize;
         prize.asset_id = flipcoin.bet.asset_id;
         prize.amount = count_prize(flipcoin.bet.amount);
         asset referral_prize;
         referral_prize.asset_id = flipcoin.bet.asset_id;
         referral_prize.amount = count_referral_prize(flipcoin.bet.amount);

         flipcoin_win_operation vop_win;
         flipcoin_loose_operation vop_loose;

         vop_win.flipcoin = flipcoin.id;
         vop_win.payout = prize;
         vop_win.referral_payout = referral_prize;

         vop_loose.flipcoin = flipcoin.id;
         vop_loose.bet = flipcoin.bet;
         
         const account_object& caller = get(*flipcoin.caller);

         if(heads == true) {
            flipcoin_log( "proceed_bets: Winner: ${b}", ("b", flipcoin.bettor ) );
            vop_win.winner = flipcoin.bettor;     
            vop_loose.looser =  caller.id;
            adjust_balance( flipcoin.bettor, prize );
         }
         else {
            flipcoin_log( "proceed_bets: Winner: ${b}", ("b", flipcoin.caller ) );
            vop_win.winner = caller.id;
            vop_loose.looser = flipcoin.bettor;
            adjust_balance( caller.id, prize );
         }
         push_applied_operation( vop_win );
         push_applied_operation( vop_loose );

         const account_object& winner_account = get(vop_win.winner);
         const account_object& looser_account = get(vop_loose.looser);

         std::set<account_id_type> accounts_set = {};
         winner_account.statistics(*this).update_nv(referral_prize.amount, uint8_t(1) , uint8_t(0) , winner_account, *this, accounts_set);
         const account_statistics_object& customer_statistics = winner_account.statistics(*this);

         if ( head_block_time() >= HARDFORK_CWD2_TIME ) {
            winner_account.statistics(*this).update_pv(flipcoin.bet.amount, winner_account, *this);
            looser_account.statistics(*this).update_pv(flipcoin.bet.amount, looser_account, *this);
         }
         else {
            winner_account.statistics(*this).update_pv(referral_prize.amount, winner_account, *this);
         }

         modify(customer_statistics, [&](account_statistics_object& s)
         {
            s.pay_fee( referral_prize.amount, false );
         });

         // if (head_time < HARDFORK_CWD6_TIME) {
            modify(winner_account, [&](account_object& a) {
               a.statistics(*this).process_fees(a, *this);
            });
         // }

         remove(flipcoin);
      }
   }
} FC_CAPTURE_AND_RETHROW() }


void database::proceed_pledge()
{ try {
   auto head_time = head_block_time();
   
   auto& po_idx = get_index_type<pledge_offer_index>().indices().get<by_expiration>();
   auto itr = po_idx.upper_bound(fc::time_point_sec(1));
   auto end = po_idx.lower_bound( head_time );
   if (itr != end) {
      elog( "PLEDGE ITR ${a} status ${s} expiration ${e}", ("a", itr->id)("s", itr->status)("e", itr->expiration) );
      elog( "PLEDGE END cycle ${a} status ${s} expiration ${e}", ("a", end->id)("s", end->status)("e", end->expiration) );
   }
   while( itr != end )
   {
      const pledge_offer_object& po_obj = *itr;
      elog( "PLEDGE in cycle ${a} status ${s} expiration ${e}", ("a", po_obj.id)("s", po_obj.status)("e", po_obj.expiration) );
      if (po_obj.status == 1 && po_obj.expiration <= head_time) {
         pledge_offer_auto_repay_operation po_repay;

         po_repay.debitor = po_obj.debitor;
         po_repay.creditor = po_obj.creditor;
         po_repay.pledge_amount = po_obj.pledge_amount;
         po_repay.credit_amount = po_obj.credit_amount;
         po_repay.repay_amount = po_obj.repay_amount;
         po_repay.pledge_offer = po_obj.id;

         push_applied_operation( po_repay );    

         adjust_balance( po_obj.creditor, po_obj.pledge_amount );
         elog( "PLEDGE status MARKED FOR REMOVE  ${a} status ${s} expiration ${e}", ("a", po_obj.id)("s", po_obj.status)("e", po_obj.expiration) );
         modify(po_obj, [&](pledge_offer_object& p)
         {
            p.status = 8;
            p.expiration = fc::time_point_sec();
         });
      }
      itr++; 
   }
    auto& po_idx_remove = get_index_type<pledge_offer_index>().indices().get<by_status>();
    while( !po_idx_remove.empty() && po_idx_remove.rbegin()->status == 8 ) {
      elog( "PLEDGE status REMOVED  ${a} status ${s} expiration ${e}", ("a", po_idx_remove.rbegin()->id)("s", po_idx_remove.rbegin()->status)("e", po_idx_remove.rbegin()->expiration) );
      remove(*po_idx_remove.rbegin());
    }
} FC_CAPTURE_AND_RETHROW() }

void database::proceed_approved_transfer()
{ try {
   auto head_time = head_block_time();
   
   auto& ato_idx = get_index_type<approved_transfer_index>().indices().get<by_expiration>();
   auto itr = ato_idx.upper_bound(fc::time_point_sec(1));
   auto end = ato_idx.lower_bound( head_time );
   if (itr != end) {
      elog( "ATO ITR ${a} status ${s} expiration ${e}", ("a", itr->id)("s", itr->status)("e", itr->expiration) );
      elog( "ATO END cycle ${a} status ${s} expiration ${e}", ("a", end->id)("s", end->status)("e", end->expiration) );
   }
   while( itr != end )
   {
      const approved_transfer_object& ato_obj = *itr;
      elog( "ATO in cycle ${a} status ${s} expiration ${e}", ("a", ato_obj.id)("s", ato_obj.status)("e", ato_obj.expiration) );
      if (ato_obj.status == 0 && ato_obj.expiration <= head_time) {
         approved_transfer_cancel_operation ato_cancel;
         ato_cancel.from = ato_obj.from;
         ato_cancel.to = ato_obj.to;
         ato_cancel.arbitr = ato_obj.arbitr;
         ato_cancel.amount = ato_obj.amount;
         ato_cancel.ato = ato_obj.id;

         push_applied_operation( ato_cancel );    

         adjust_balance( ato_obj.from, ato_obj.amount );

         elog( "ATO status MARKED FOR REMOVE  ${a} status ${s} expiration ${e}", ("a", ato_obj.id)("s", ato_obj.status)("e", ato_obj.expiration) );
         modify(ato_obj, [&](approved_transfer_object& p)
         {
            p.status = 2;
            p.expiration = fc::time_point_sec();
         });
      }
      itr++; 
   }
    auto& ato_idx_remove = get_index_type<approved_transfer_index>().indices().get<by_status>();
    while( !ato_idx_remove.empty() && ato_idx_remove.rbegin()->status == 2 ) {
      elog( "ATO status REMOVED  ${a} status ${s} expiration ${e}", ("a", ato_idx_remove.rbegin()->id)("s", ato_idx_remove.rbegin()->status)("e", ato_idx_remove.rbegin()->expiration) );
      remove(*ato_idx_remove.rbegin());
    }
} FC_CAPTURE_AND_RETHROW() }

void database::proceed_lottery_goods()
{ try {
   auto head_time = head_block_time()+fc::seconds(5);
   
   auto& lg_idx = get_index_type<lottery_goods_index>().indices().get<by_expiration>();
   auto itr = lg_idx.upper_bound(fc::time_point_sec(1));
   auto end = lg_idx.lower_bound( head_time );

   while( itr != end )
   {
     auto block_id = head_block_id();
      const lottery_goods_object& lg_obj = *itr;
      // elog( "lottery status expired  ${a} status ${s} expiration ${e} block_id ${b}", ("a", lg_obj.id)("s", lg_obj.status)("e", lg_obj.expiration)("b",block_id) );

      if (lg_obj.status == 1) {
         std::string block_id_str = std::string(block_id);
         lottery_log( "block: ${b}", ("b", block_id_str) );
         
         std::stringstream ss;
         std::string total_p_hex;
         ss << std::hex << lg_obj.total_participants;
         ss >> total_p_hex;
         
         lottery_log( "total_participants: ${total_participants}", ("total_participants", lg_obj.total_participants) );
         lottery_log( "total_p_hex: ${total_p_hex}", ("total_p_hex", total_p_hex) );
         
         const char * id_substr = block_id_str.substr(block_id_str.length()-total_p_hex.length(),total_p_hex.length()).c_str();
         lottery_log( "id_substr: ${id_substr}", ("id_substr", id_substr) );

         uint32_t block_id_int = strtoul(id_substr, NULL, 16);
         lottery_log( "block_id_int: ${block_id_int}", ("block_id_int", block_id_int) );
         
         std::vector<account_id_type> participants = lg_obj.participants;
         
         if((lg_obj.total_participants == participants.size()) && (lg_obj.total_participants > 0)){
            uint32_t winner_index = block_id_int % lg_obj.total_participants;
            
            lottery_log( "winner_index: ${winner_index}", ("winner_index", winner_index) );

            
            for(uint32_t i = 0; i < participants.size(); i++)
            {
               account_id_type participant_id = participants[i];
               if(i == winner_index){
                  lottery_log( "winner: ${winner}", ("winner", participant_id) );
                  
                  lottery_log( "modify_index: ${modify_index}", ("modify_index", lg_obj.id) );
                  // Hardforked fix precission in FC library error:
                  if (lg_obj.id == lottery_goods_id_type(48)) {
                     participant_id=account_id_type(164);
                  }
                  // Hardforked fix end

                  modify(lg_obj, [&](lottery_goods_object& l)
                  {
                     l.winner = participant_id;
                     l.status = 2;
                     l.expiration = fc::time_point_sec();
                  });  
               
                  lottery_goods_win_operation lg_win;
                  lg_win.lot_id = lg_obj.id;
                  lg_win.winner = participant_id;
                  push_applied_operation( lg_win );
               } else {
                  lottery_log( "looser: ${looser}", ("looser", participant_id) );
                  lottery_goods_loose_operation lg_loose;
                  lg_loose.lot_id = lg_obj.id;
                  lg_loose.looser = participant_id;
                  push_applied_operation( lg_loose );                  
               }
            }
         }
      // elog( "lottery status PLAYED  ${a} status ${s} expiration ${e} winner ${w}", ("a", lg_obj.id)("s", lg_obj.status)("e", lg_obj.expiration)("w",lg_obj.winner) );
      
      }
      if (lg_obj.status == 0) {
         std::vector<account_id_type> participants = lg_obj.participants;
         for(uint32_t i = 0; i < participants.size(); i++) {        
            account_id_type participant_id = participants[i];
            adjust_balance( participant_id, lg_obj.ticket_price );
            lottery_goods_refund_operation lg_refund;
            lg_refund.lot_id = lg_obj.id;
            lg_refund.participant = participant_id;
            lg_refund.ticket_price = lg_obj.ticket_price;
            push_applied_operation( lg_refund );          
         }
         elog( "lottery status MARKED FOR REMOVE ${a} status ${s} expiration ${e}", ("a", lg_obj.id)("s", lg_obj.status)("e", lg_obj.expiration) );
         modify(lg_obj, [&](lottery_goods_object& l)
         {
            l.status = 8;
            l.expiration = fc::time_point_sec();
         });
      }
      itr++;
   }

    auto& lg_idx_remove = get_index_type<lottery_goods_index>().indices().get<by_status>();
    while( !lg_idx_remove.empty() && lg_idx_remove.rbegin()->status == 8 ) {
      elog( "lottery status REMOVED  ${a} status ${s} expiration ${e}", ("a", lg_idx_remove.rbegin()->id)("s", lg_idx_remove.rbegin()->status)("e", lg_idx_remove.rbegin()->expiration) );
      remove(*lg_idx_remove.rbegin());
    }   
} FC_CAPTURE_AND_RETHROW() }


void database::proceed_matrix()
{ try {
   auto head_block = head_block_num();
   auto head_time = head_block_time();

   share_type    matrix_level_1_price                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(45));
   share_type    matrix_level_2_price                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(80));
   share_type    matrix_level_3_price                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(218));
   share_type    matrix_level_4_price                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(790));
   share_type    matrix_level_5_price                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(2870));
   share_type    matrix_level_6_price                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(7820));
   share_type    matrix_level_7_price                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(14200));
   share_type    matrix_level_8_price                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(25800));
   share_type    matrix_level_1_prize                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(90));
   share_type    matrix_level_2_prize                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(240));
   share_type    matrix_level_3_prize                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(872));
   share_type    matrix_level_4_prize                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(3160));
   share_type    matrix_level_5_prize                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(8610));
   share_type    matrix_level_6_prize                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(15640));
   share_type    matrix_level_7_prize                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(28400));
   share_type    matrix_level_8_prize                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(51600));
   uint8_t       matrix_level_1_cells                = uint8_t(2);
   uint8_t       matrix_level_2_cells                = uint8_t(3);
   uint8_t       matrix_level_3_cells                = uint8_t(4);
   uint8_t       matrix_level_4_cells                = uint8_t(4);
   uint8_t       matrix_level_5_cells                = uint8_t(3);
   uint8_t       matrix_level_6_cells                = uint8_t(2);
   uint8_t       matrix_level_7_cells                = uint8_t(2);
   uint8_t       matrix_level_8_cells                = uint8_t(2);
   uint32_t      matrix_first_start_block            = uint32_t(3491491);
   uint32_t      matrix_lasts_blocks                 = uint32_t(1000000);
   uint32_t      matrix_idle_blocks                  = uint32_t(120000);


   const auto& by_matrix_finish = get_index_type<matrix_index>().indices().get<by_finish_block_number>();
   while( !by_matrix_finish.empty() && by_matrix_finish.rbegin()->finish_block_number <= head_block && by_matrix_finish.rbegin()->status == 0 )
   {
      const matrix_object& current_matrix = *by_matrix_finish.rbegin();
      auto matrix_lasts = current_matrix.finish_block_number - current_matrix.start_block_number;
      auto average = current_matrix.amount / matrix_lasts * 70 / 100;
      auto average_120k = current_matrix.last_120k_amount / matrix_idle_blocks;

      // elog( "head_block: ${head_block}", ("head_block", head_block) );
      // elog( "average: ${average}", ("average", average) );
      // elog( "average_120k: ${average_120k}", ("average_120k", average_120k) );

      if (average_120k >= average && head_time < HARDFORK_CWD5_TIME) {
         modify(current_matrix, [&](matrix_object& m)
         {
            m.finish_block_number = current_matrix.finish_block_number+matrix_idle_blocks;
            m.last_120k_amount = 0;
         });
      }
      else {
         const asset_dynamic_data_object& core = get_core_dynamic_data();
         modify(core, [&]( asset_dynamic_data_object& _core )
         {
            _core.accumulated_fees = core.accumulated_fees+current_matrix.amount;
         });        

         modify(current_matrix, [&](matrix_object& m)
         {
            m.status = 1;
         }); 

         share_type    matrix_level_1_price_new                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(45));
         share_type    matrix_level_2_price_new                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(80));
         share_type    matrix_level_3_price_new                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(145));
         share_type    matrix_level_4_price_new                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(260));
         share_type    matrix_level_5_price_new                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(460));
         share_type    matrix_level_6_price_new                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(830));
         share_type    matrix_level_7_price_new                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(1500));
         share_type    matrix_level_8_price_new                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(2700));
         share_type    matrix_level_1_prize_new                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(90));
         share_type    matrix_level_2_prize_new                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(160));
         share_type    matrix_level_3_prize_new                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(290));
         share_type    matrix_level_4_prize_new                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(520));
         share_type    matrix_level_5_prize_new                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(920));
         share_type    matrix_level_6_prize_new                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(1660));
         share_type    matrix_level_7_prize_new                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(3000));
         share_type    matrix_level_8_prize_new                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(5400));
         uint8_t       matrix_level_1_cells_new                = uint8_t(2);
         uint8_t       matrix_level_2_cells_new                = uint8_t(2);
         uint8_t       matrix_level_3_cells_new                = uint8_t(2);
         uint8_t       matrix_level_4_cells_new                = uint8_t(2);
         uint8_t       matrix_level_5_cells_new                = uint8_t(2);
         uint8_t       matrix_level_6_cells_new                = uint8_t(2);
         uint8_t       matrix_level_7_cells_new                = uint8_t(2);
         uint8_t       matrix_level_8_cells_new                = uint8_t(2);

         if ( head_time  >= HARDFORK_CWD4_TIME ) { //HF4 - matrix lasts 240 000 blocks
            matrix_lasts_blocks = uint32_t(240000);
         }

         if ( head_time  >= HARDFORK_CWD5_TIME ) { //HF5 - matrix lasts 177 blocks
            matrix_lasts_blocks = uint32_t(177);
            matrix_idle_blocks = uint32_t(16671);
         }
         if (current_matrix.id == matrix_id_type(13)) {
            matrix_idle_blocks = uint32_t(15851);         
         }
         // ADDED MINI MATRIX
         if (current_matrix.finish_block_number >= uint32_t(8243772)) {
            matrix_level_1_price_new                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(9));
            matrix_level_2_price_new                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(16));
            matrix_level_3_price_new                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(29));
            matrix_level_4_price_new                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(52));
            matrix_level_5_price_new                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(94));
            matrix_level_6_price_new                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(170));
            matrix_level_7_price_new                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(309));
            matrix_level_8_price_new                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(555));
            matrix_level_1_prize_new                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(18));
            matrix_level_2_prize_new                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(32));
            matrix_level_3_prize_new                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(58));
            matrix_level_4_prize_new                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(104));
            matrix_level_5_prize_new                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(188));
            matrix_level_6_prize_new                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(340));
            matrix_level_7_prize_new                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(618));
            matrix_level_8_prize_new                = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(1110));        
         }

         const global_property_object& gpo = get_global_properties();
         const auto& gz_params = gpo.gamezone_parameters;
         if ( head_time  >= HARDFORK_CWD6_TIME ) { 
            matrix_lasts_blocks                 = gz_params.matrix_lasts_blocks;
            matrix_idle_blocks                  = gz_params.matrix_idle_blocks;

            matrix_level_1_cells_new            = gz_params.matrix_level_1_cells;
            matrix_level_2_cells_new            = gz_params.matrix_level_2_cells;
            matrix_level_3_cells_new            = gz_params.matrix_level_3_cells;
            matrix_level_4_cells_new            = gz_params.matrix_level_4_cells;
            matrix_level_5_cells_new            = gz_params.matrix_level_5_cells;
            matrix_level_6_cells_new            = gz_params.matrix_level_6_cells;
            matrix_level_7_cells_new            = gz_params.matrix_level_7_cells;
            matrix_level_8_cells_new            = gz_params.matrix_level_8_cells;

            matrix_level_1_price_new            = gz_params.matrix_level_1_price;
            matrix_level_2_price_new            = gz_params.matrix_level_2_price;
            matrix_level_3_price_new            = gz_params.matrix_level_3_price;
            matrix_level_4_price_new            = gz_params.matrix_level_4_price;
            matrix_level_5_price_new            = gz_params.matrix_level_5_price;
            matrix_level_6_price_new            = gz_params.matrix_level_6_price;
            matrix_level_7_price_new            = gz_params.matrix_level_7_price;
            matrix_level_8_price_new            = gz_params.matrix_level_8_price;

            matrix_level_1_prize_new            = gz_params.matrix_level_1_prize;
            matrix_level_2_prize_new            = gz_params.matrix_level_2_prize;
            matrix_level_3_prize_new            = gz_params.matrix_level_3_prize;
            matrix_level_4_prize_new            = gz_params.matrix_level_4_prize;
            matrix_level_5_prize_new            = gz_params.matrix_level_5_prize;
            matrix_level_6_prize_new            = gz_params.matrix_level_6_prize;
            matrix_level_7_prize_new            = gz_params.matrix_level_7_prize;
            matrix_level_8_prize_new            = gz_params.matrix_level_8_prize;
         }


         create<matrix_object>([&](matrix_object& obj){
            obj.start_block_number = head_block + matrix_idle_blocks;

            if (current_matrix.id == matrix_id_type(3)) {
               obj.finish_block_number = uint32_t(8092140);
            }
            else {
               obj.finish_block_number = head_block + matrix_idle_blocks + matrix_lasts_blocks;
            }
            
            obj.status = 0;
            obj.amount = 0;
            obj.total_amount = 0;
            obj.last_120k_amount = 0;
            obj.matrix_level_1_price = matrix_level_1_price_new;
            obj.matrix_level_2_price = matrix_level_2_price_new;
            obj.matrix_level_3_price = matrix_level_3_price_new;
            obj.matrix_level_4_price = matrix_level_4_price_new;
            obj.matrix_level_5_price = matrix_level_5_price_new;
            obj.matrix_level_6_price = matrix_level_6_price_new;
            obj.matrix_level_7_price = matrix_level_7_price_new;
            obj.matrix_level_8_price = matrix_level_8_price_new;
            obj.matrix_level_1_prize = matrix_level_1_prize_new;
            obj.matrix_level_2_prize = matrix_level_2_prize_new;
            obj.matrix_level_3_prize = matrix_level_3_prize_new;
            obj.matrix_level_4_prize = matrix_level_4_prize_new;
            obj.matrix_level_5_prize = matrix_level_5_prize_new;
            obj.matrix_level_6_prize = matrix_level_6_prize_new;
            obj.matrix_level_7_prize = matrix_level_7_prize_new;
            obj.matrix_level_8_prize = matrix_level_8_prize_new;
            obj.matrix_level_1_cells = matrix_level_1_cells_new;
            obj.matrix_level_2_cells = matrix_level_2_cells_new;
            obj.matrix_level_3_cells = matrix_level_3_cells_new;
            obj.matrix_level_4_cells = matrix_level_4_cells_new;
            obj.matrix_level_5_cells = matrix_level_5_cells_new;
            obj.matrix_level_6_cells = matrix_level_6_cells_new;
            obj.matrix_level_7_cells = matrix_level_7_cells_new;
            obj.matrix_level_8_cells = matrix_level_8_cells_new;
         });
      }   
   }
   
   if (by_matrix_finish.empty() && head_block >= matrix_first_start_block) {
      create<matrix_object>([&](matrix_object& obj){
         obj.start_block_number = head_block;
         obj.finish_block_number = head_block + matrix_lasts_blocks;
         obj.status = 0;
         obj.amount = 0;
         obj.total_amount = 0;
         obj.last_120k_amount = 0;
         obj.matrix_level_1_price = matrix_level_1_price;
         obj.matrix_level_2_price = matrix_level_2_price;
         obj.matrix_level_3_price = matrix_level_3_price;
         obj.matrix_level_4_price = matrix_level_4_price;
         obj.matrix_level_5_price = matrix_level_5_price;
         obj.matrix_level_6_price = matrix_level_6_price;
         obj.matrix_level_7_price = matrix_level_7_price;
         obj.matrix_level_8_price = matrix_level_8_price;
         obj.matrix_level_1_prize = matrix_level_1_prize;
         obj.matrix_level_2_prize = matrix_level_2_prize;
         obj.matrix_level_3_prize = matrix_level_3_prize;
         obj.matrix_level_4_prize = matrix_level_4_prize;
         obj.matrix_level_5_prize = matrix_level_5_prize;
         obj.matrix_level_6_prize = matrix_level_6_prize;
         obj.matrix_level_7_prize = matrix_level_7_prize;
         obj.matrix_level_8_prize = matrix_level_8_prize;
         obj.matrix_level_1_cells = matrix_level_1_cells;
         obj.matrix_level_2_cells = matrix_level_2_cells;
         obj.matrix_level_3_cells = matrix_level_3_cells;
         obj.matrix_level_4_cells = matrix_level_4_cells;
         obj.matrix_level_5_cells = matrix_level_5_cells;
         obj.matrix_level_6_cells = matrix_level_6_cells;
         obj.matrix_level_7_cells = matrix_level_7_cells;
         obj.matrix_level_8_cells = matrix_level_8_cells;
      });
   }


} FC_CAPTURE_AND_RETHROW() }

void database::clear_expired_orders()
{ try {
         //Cancel expired limit orders
         auto head_time = head_block_time();
         auto maint_time = get_dynamic_global_properties().next_maintenance_time;

         bool before_core_hardfork_184 = ( maint_time <= HARDFORK_CORE_184_TIME ); // something-for-nothing
         bool before_core_hardfork_342 = ( maint_time <= HARDFORK_CORE_342_TIME ); // better rounding
         bool before_core_hardfork_606 = ( maint_time <= HARDFORK_CORE_606_TIME ); // feed always trigger call

         auto& limit_index = get_index_type<limit_order_index>().indices().get<by_expiration>();
         while( !limit_index.empty() && limit_index.begin()->expiration <= head_time )
         {
            const limit_order_object& order = *limit_index.begin();
            auto base_asset = order.sell_price.base.asset_id;
            auto quote_asset = order.sell_price.quote.asset_id;
            cancel_limit_order( order );
            if( before_core_hardfork_606 )
            {
               // check call orders
               // Comments below are copied from limit_order_cancel_evaluator::do_apply(...)
               // Possible optimization: order can be called by cancelling a limit order
               //   if the canceled order was at the top of the book.
               // Do I need to check calls in both assets?
               check_call_orders( base_asset( *this ) );
               check_call_orders( quote_asset( *this ) );
            }
         }

   //Process expired force settlement orders
   auto& settlement_index = get_index_type<force_settlement_index>().indices().get<by_expiration>();
   if( !settlement_index.empty() )
   {
      asset_id_type current_asset = settlement_index.begin()->settlement_asset_id();
      asset max_settlement_volume;
      price settlement_fill_price;
      price settlement_price;
      bool current_asset_finished = false;
      bool extra_dump = false;

      auto next_asset = [&current_asset, &current_asset_finished, &settlement_index, &extra_dump] {
         auto bound = settlement_index.upper_bound(current_asset);
         if( bound == settlement_index.end() )
         {
            if( extra_dump )
            {
               ilog( "next_asset() returning false" );
            }
            return false;
         }
         if( extra_dump )
         {
            ilog( "next_asset returning true, bound is ${b}", ("b", *bound) );
         }
         current_asset = bound->settlement_asset_id();
         current_asset_finished = false;
         return true;
      };

      uint32_t count = 0;

      // At each iteration, we either consume the current order and remove it, or we move to the next asset
      for( auto itr = settlement_index.lower_bound(current_asset);
           itr != settlement_index.end();
           itr = settlement_index.lower_bound(current_asset) )
      {
         ++count;
         const force_settlement_object& order = *itr;
         auto order_id = order.id;
         current_asset = order.settlement_asset_id();
         const asset_object& mia_object = get(current_asset);
         const asset_bitasset_data_object& mia = mia_object.bitasset_data(*this);

         extra_dump = ((count >= 1000) && (count <= 1020));

         if( extra_dump )
         {
            wlog( "clear_expired_orders() dumping extra data for iteration ${c}", ("c", count) );
            ilog( "head_block_num is ${hb} current_asset is ${a}", ("hb", head_block_num())("a", current_asset) );
         }

         if( mia.has_settlement() )
         {
            ilog( "Canceling a force settlement because of black swan" );
            cancel_settle_order( order );
            continue;
         }

         // Has this order not reached its settlement date?
         if( order.settlement_date > head_time )
         {
            if( next_asset() )
            {
               if( extra_dump )
               {
                  ilog( "next_asset() returned true when order.settlement_date > head_block_time()" );
               }
               continue;
            }
            break;
         }
         // Can we still settle in this asset?
         if( mia.current_feed.settlement_price.is_null() )
         {
            ilog("Canceling a force settlement in ${asset} because settlement price is null",
                 ("asset", mia_object.symbol));
            cancel_settle_order(order);
            continue;
         }
         if( max_settlement_volume.asset_id != current_asset )
            max_settlement_volume = mia_object.amount(mia.max_force_settlement_volume(mia_object.dynamic_data(*this).current_supply));
         // When current_asset_finished is true, this would be the 2nd time processing the same order.
         // In this case, we move to the next asset.
         if( mia.force_settled_volume >= max_settlement_volume.amount || current_asset_finished )
         {
            /*
            ilog("Skipping force settlement in ${asset}; settled ${settled_volume} / ${max_volume}",
                 ("asset", mia_object.symbol)("settlement_price_null",mia.current_feed.settlement_price.is_null())
                 ("settled_volume", mia.force_settled_volume)("max_volume", max_settlement_volume));
                 */
            if( next_asset() )
            {
               if( extra_dump )
               {
                  ilog( "next_asset() returned true when mia.force_settled_volume >= max_settlement_volume.amount" );
               }
               continue;
            }
            break;
         }

         if( settlement_fill_price.base.asset_id != current_asset ) // only calculate once per asset
            settlement_fill_price = mia.current_feed.settlement_price
                                    / ratio_type( GRAPHENE_100_PERCENT - mia.options.force_settlement_offset_percent,
                                                  GRAPHENE_100_PERCENT );

         if( before_core_hardfork_342 )
         {
            auto& pays = order.balance;
            auto receives = (order.balance * mia.current_feed.settlement_price);
            receives.amount = ( fc::uint128_t(receives.amount.value) *
                                (GRAPHENE_100_PERCENT - mia.options.force_settlement_offset_percent) /
                                GRAPHENE_100_PERCENT ).to_uint64();
            assert(receives <= order.balance * mia.current_feed.settlement_price);
            settlement_price = pays / receives;
         }
         else if( settlement_price.base.asset_id != current_asset ) // only calculate once per asset
            settlement_price = settlement_fill_price;

         auto& call_index = get_index_type<call_order_index>().indices().get<by_collateral>();
         asset settled = mia_object.amount(mia.force_settled_volume);
         // Match against the least collateralized short until the settlement is finished or we reach max settlements
         while( settled < max_settlement_volume && find_object(order_id) )
         {
            auto itr = call_index.lower_bound(boost::make_tuple(price::min(mia_object.bitasset_data(*this).options.short_backing_asset,
                                                                           mia_object.get_id())));
            // There should always be a call order, since asset exists!
            assert(itr != call_index.end() && itr->debt_type() == mia_object.get_id());
            asset max_settlement = max_settlement_volume - settled;

            if( order.balance.amount == 0 )
            {
               wlog( "0 settlement detected" );
               cancel_settle_order( order );
               break;
            }
            try {
               asset new_settled = match(*itr, order, settlement_price, max_settlement, settlement_fill_price);
               if( !before_core_hardfork_184 && new_settled.amount == 0 ) // unable to fill this settle order
               {
                  if( find_object( order_id ) ) // the settle order hasn't been cancelled
                     current_asset_finished = true;
                  break;
               }
               settled += new_settled;
               // before hard fork core-342, `new_settled > 0` is always true, we'll have:
               // * call order is completely filled (thus itr will change in next loop), or
               // * settle order is completely filled (thus find_object(order_id) will be false so will break out), or
               // * reached max_settlement_volume limit (thus new_settled == max_settlement so will break out).
               //
               // after hard fork core-342, if new_settled > 0, we'll have:
               // * call order is completely filled (thus itr will change in next loop), or
               // * settle order is completely filled (thus find_object(order_id) will be false so will break out), or
               // * reached max_settlement_volume limit, but it's possible that new_settled < max_settlement,
               //   in this case, new_settled will be zero in next iteration of the loop, so no need to check here.
            } 
            catch ( const black_swan_exception& e ) { 
               wlog( "Cancelling a settle_order since it may trigger a black swan: ${o}, ${e}",
                     ("o", order)("e", e.to_detail_string()) );
               cancel_settle_order( order );
               break;
            }
         }
         if( mia.force_settled_volume != settled.amount )
         {
            modify(mia, [settled](asset_bitasset_data_object& b) {
               b.force_settled_volume = settled.amount;
            });
         }
      }
   }
} FC_CAPTURE_AND_RETHROW() }

void database::update_expired_feeds()
{
   const auto head_time = head_block_time();
   bool after_hardfork_615 = ( head_time >= HARDFORK_615_TIME );

   const auto& idx = get_index_type<asset_bitasset_data_index>().indices().get<by_feed_expiration>();
   auto itr = idx.begin();
   while( itr != idx.end() && itr->feed_is_expired( head_time ) )
   {
      const asset_bitasset_data_object& b = *itr;
      ++itr; // not always process begin() because old code skipped updating some assets before hf 615
      bool update_cer = false; // for better performance, to only update bitasset once, also check CER in this function
      const asset_object* asset_ptr = nullptr;
      // update feeds, check margin calls
      if( after_hardfork_615 || b.feed_is_expired_before_hardfork_615( head_time ) )
      {
         auto old_median_feed = b.current_feed;
         modify( b, [head_time,&update_cer]( asset_bitasset_data_object& abdo )
         {
            abdo.update_median_feeds( head_time );
            if( abdo.need_to_update_cer() )
            {
               update_cer = true;
               abdo.asset_cer_updated = false;
               abdo.feed_cer_updated = false;
            }
         });
         if( !b.current_feed.settlement_price.is_null() && !( b.current_feed == old_median_feed ) ) // `==` check is safe here
         {
            asset_ptr = &b.asset_id( *this );
            check_call_orders( *asset_ptr, true, false, &b );
         }
      }
      // update CER
      if( update_cer )
      {
         if( !asset_ptr )
            asset_ptr = &b.asset_id( *this );
         if( asset_ptr->options.core_exchange_rate != b.current_feed.core_exchange_rate )
         {
            modify( *asset_ptr, [&b]( asset_object& ao )
            {
               ao.options.core_exchange_rate = b.current_feed.core_exchange_rate;
            });
         }
      }
   } // for each asset whose feed is expired

   // process assets affected by crowdwiz-core issue 453 before hard fork 615
   if( !after_hardfork_615 )
   {
      for( asset_id_type a : _issue_453_affected_assets )
      {
         check_call_orders( a(*this) );
      }
   }
}

void database::update_core_exchange_rates()
{
   const auto& idx = get_index_type<asset_bitasset_data_index>().indices().get<by_cer_update>();
   if( idx.begin() != idx.end() )
   {
      for( auto itr = idx.rbegin(); itr->need_to_update_cer(); itr = idx.rbegin() )
      {
         const asset_bitasset_data_object& b = *itr;
         const asset_object& a = b.asset_id( *this );
         if( a.options.core_exchange_rate != b.current_feed.core_exchange_rate )
         {
            modify( a, [&b]( asset_object& ao )
            {
               ao.options.core_exchange_rate = b.current_feed.core_exchange_rate;
            });
         }
         modify( b, []( asset_bitasset_data_object& abdo )
         {
            abdo.asset_cer_updated = false;
            abdo.feed_cer_updated = false;
         });
      }
   }
}

void database::update_maintenance_flag( bool new_maintenance_flag )
{
   modify( get_dynamic_global_properties(), [&]( dynamic_global_property_object& dpo )
   {
      auto maintenance_flag = dynamic_global_property_object::maintenance_flag;
      dpo.dynamic_flags =
           (dpo.dynamic_flags & ~maintenance_flag)
         | (new_maintenance_flag ? maintenance_flag : 0);
   } );
   return;
}

void database::update_withdraw_permissions()
{
   auto& permit_index = get_index_type<withdraw_permission_index>().indices().get<by_expiration>();
   while( !permit_index.empty() && permit_index.begin()->expiration <= head_block_time() )
      remove(*permit_index.begin());
}

} }
