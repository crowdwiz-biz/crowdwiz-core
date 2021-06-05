#include <fc/container/flat.hpp>

#include <graphene/chain/protocol/authority.hpp>
#include <graphene/chain/protocol/operations.hpp>
#include <graphene/chain/protocol/transaction.hpp>
#include <graphene/chain/withdraw_permission_object.hpp>
#include <graphene/chain/database.hpp>
#include <graphene/chain/worker_object.hpp>
#include <graphene/chain/confidential_object.hpp>
#include <graphene/chain/market_object.hpp>
#include <graphene/chain/gamezone_object.hpp>
#include <graphene/chain/committee_member_object.hpp>
#include <graphene/chain/witness_object.hpp>
#include <graphene/chain/proposal_object.hpp>
#include <graphene/chain/operation_history_object.hpp>
#include <graphene/chain/vesting_balance_object.hpp>
#include <graphene/chain/transaction_object.hpp>
#include <graphene/chain/impacted.hpp>
#include <graphene/chain/exchange_object.hpp>
#include <graphene/chain/financial_object.hpp>
#include <graphene/chain/greatrace_object.hpp>

using namespace fc;
using namespace graphene::chain;

// TODO:  Review all of these, especially no-ops
struct get_impacted_account_visitor
{
   flat_set<account_id_type>& _impacted;
   get_impacted_account_visitor( flat_set<account_id_type>& impact ):_impacted(impact) {}
   typedef void result_type;

   void operator()( const transfer_operation& op )
   {
      _impacted.insert( op.to );
      _impacted.insert( op.fee_payer() ); // from
   }
   void operator()( const asset_claim_fees_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // issuer
   }
   void operator()( const asset_claim_pool_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // issuer
   }
   void operator()( const limit_order_create_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // seller
   }
   void operator()( const limit_order_cancel_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // fee_paying_account
   }
   void operator()( const call_order_update_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // funding_account
   }
   void operator()( const bid_collateral_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // bidder
   }
   void operator()( const fill_order_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // account_id
   }
   void operator()( const execute_bid_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // bidder
   }
   void operator()( const account_create_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // registrar
      _impacted.insert( op.referrer );
      add_authority_accounts( _impacted, op.owner );
      add_authority_accounts( _impacted, op.active );
   }
   void operator()( const account_update_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // account
      if( op.owner )
         add_authority_accounts( _impacted, *(op.owner) );
      if( op.active )
         add_authority_accounts( _impacted, *(op.active) );
   }
   void operator()( const account_whitelist_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // authorizing_account
      _impacted.insert( op.account_to_list );
   }
   void operator()( const account_upgrade_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // account_to_upgrade
   }
   void operator()( const account_transfer_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // account_id
   }
   void operator()( const asset_create_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // issuer
   }
   void operator()( const asset_update_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // issuer
      if( op.new_issuer )
         _impacted.insert( *(op.new_issuer) );
   }
   void operator()( const asset_update_issuer_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // issuer
      _impacted.insert( op.new_issuer );
   }
   void operator()( const asset_update_bitasset_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // issuer
   }
   void operator()( const asset_update_feed_producers_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // issuer
   }
   void operator()( const asset_issue_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // issuer
      _impacted.insert( op.issue_to_account );
   }
   void operator()( const asset_reserve_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // payer
   }
   void operator()( const asset_fund_fee_pool_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // from_account
   }
   void operator()( const asset_settle_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // account
   }
   void operator()( const asset_global_settle_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // issuer
   }
   void operator()( const asset_publish_feed_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // publisher
   }
   void operator()( const witness_create_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // witness_account
   }
   void operator()( const witness_update_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // witness_account
   }
   void operator()( const proposal_create_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // fee_paying_account
      vector<authority> other;
      for( const auto& proposed_op : op.proposed_ops )
         operation_get_required_authorities( proposed_op.op, _impacted, _impacted, other );
      for( auto& o : other )
         add_authority_accounts( _impacted, o );
   }
   void operator()( const proposal_update_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // fee_paying_account
   }
   void operator()( const proposal_delete_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // fee_paying_account
   }
   void operator()( const withdraw_permission_create_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // withdraw_from_account
      _impacted.insert( op.authorized_account );
   }
   void operator()( const withdraw_permission_update_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // withdraw_from_account
      _impacted.insert( op.authorized_account );
   }
   void operator()( const withdraw_permission_claim_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // withdraw_to_account
      _impacted.insert( op.withdraw_from_account );
   }
   void operator()( const withdraw_permission_delete_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // withdraw_from_account
      _impacted.insert( op.authorized_account );
   }
   void operator()( const committee_member_create_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // committee_member_account
   }
   void operator()( const committee_member_update_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // committee_member_account
   }
   void operator()( const committee_member_update_global_parameters_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // account_id_type()
   }
   void operator()( const vesting_balance_create_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // creator
      _impacted.insert( op.owner );
   }
   void operator()( const vesting_balance_withdraw_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // owner
   }
   void operator()( const worker_create_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // owner
   }
   void operator()( const custom_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // payer
   }
   void operator()( const assert_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // fee_paying_account
   }
   void operator()( const balance_claim_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // deposit_to_account
   }
   void operator()( const override_transfer_operation& op )
   {
      _impacted.insert( op.to );
      _impacted.insert( op.from );
      _impacted.insert( op.fee_payer() ); // issuer
   }
   void operator()( const transfer_to_blind_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // from
      for( const auto& out : op.outputs )
         add_authority_accounts( _impacted, out.owner );
   }
   void operator()( const blind_transfer_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // GRAPHENE_TEMP_ACCOUNT
      for( const auto& in : op.inputs )
         add_authority_accounts( _impacted, in.owner );
      for( const auto& out : op.outputs )
         add_authority_accounts( _impacted, out.owner );
   }
   void operator()( const transfer_from_blind_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // GRAPHENE_TEMP_ACCOUNT
      _impacted.insert( op.to );
      for( const auto& in : op.inputs )
         add_authority_accounts( _impacted, in.owner );
   }
   void operator()( const asset_settle_cancel_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // account
   }
   void operator()( const fba_distribute_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // account_id
   }
   void operator()( const account_status_upgrade_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // account_id
   }
   void operator()( const change_referrer_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // account_id
   }
   void operator()( const flipcoin_bet_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // account_id
   }
   void operator()( const flipcoin_call_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // account_id
   }
   void operator()( const flipcoin_win_operation& op )
   {
      _impacted.insert( op.winner); // account_id
   }
   void operator()( const flipcoin_loose_operation& op )
   {
      _impacted.insert( op.looser); // account_id
   }
   void operator()( const flipcoin_cancel_operation& op )
   {
      _impacted.insert( op.bettor); // account_id
   }
   void operator()( const lottery_goods_create_lot_operation& op )
   {
      _impacted.insert( op.fee_payer() );
      _impacted.insert( op.owner );
   }
   void operator()( const lottery_goods_buy_ticket_operation& op )
   {
      _impacted.insert( op.fee_payer() );
      _impacted.insert( op.participant );
   }
   void operator()( const lottery_goods_send_contacts_operation& op )
   {
      _impacted.insert( op.owner );
      _impacted.insert( op.fee_payer() );
   }
   void operator()( const lottery_goods_confirm_delivery_operation& op )
   {
      _impacted.insert( op.owner );
      _impacted.insert( op.fee_payer() );
   }
   void operator()( const lottery_goods_win_operation& op )
   {
      _impacted.insert( op.fee_payer() );
      _impacted.insert( op.winner );
   }
   void operator()( const lottery_goods_loose_operation& op )
   {
      _impacted.insert( op.fee_payer() );
      _impacted.insert( op.looser );
   } 

   void operator()( const lottery_goods_refund_operation& op )
   {
      _impacted.insert( op.fee_payer() );
      _impacted.insert( op.participant );
   } 

   void operator()( const send_message_operation& op )
   {
      _impacted.insert( op.to );
      _impacted.insert( op.fee_payer() ); // from
   }  
   void operator()( const matrix_open_room_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // from
   } 
   void operator()( const matrix_room_closed_operation& op )
   {
      _impacted.insert( op.player );
   } 
   void operator()( const matrix_cell_filled_operation& op )
   {
      _impacted.insert( op.player );
   }
   // P2P EXCHANGE
   void operator()( const create_p2p_adv_operation& op )
   {
      _impacted.insert( op.fee_payer() );
   }
   void operator()( const edit_p2p_adv_operation& op )
   {
      _impacted.insert( op.fee_payer() );
   }
   void operator()( const clear_p2p_adv_black_list_operation& op )
   {
      _impacted.insert( op.fee_payer() );
   }
   void operator()( const remove_from_p2p_adv_black_list_operation& op )
   {
      _impacted.insert( op.blacklisted );
      _impacted.insert( op.fee_payer() );
   }

   void operator()( const create_p2p_order_operation& op )
   {
      _impacted.insert( op.p2p_gateway );
      _impacted.insert( op.fee_payer() );
   }
   void operator()( const cancel_p2p_order_operation& op )
   {
      _impacted.insert( op.p2p_client );
      _impacted.insert( op.fee_payer() );
   }
   void operator()( const autocancel_p2p_order_operation& op )
   {
      _impacted.insert( op.p2p_client );
      _impacted.insert( op.fee_payer() );
   }
   void operator()( const autorefund_p2p_order_operation& op )
   {
      _impacted.insert( op.p2p_client );
      _impacted.insert( op.p2p_gateway );
   }
   void operator()( const call_p2p_order_operation& op )
   {
      _impacted.insert( op.p2p_client );
      _impacted.insert( op.fee_payer() );   
   }
   void operator()( const payment_p2p_order_operation& op )
   {
      _impacted.insert( op.recieving_account );
      _impacted.insert( op.fee_payer() );
   }
   void operator()( const release_p2p_order_operation& op )
   {
      _impacted.insert( op.paying_account );
      _impacted.insert( op.fee_payer() );
   }
   void operator()( const open_p2p_dispute_operation& op )
   {
      _impacted.insert( op.arbitr );
      _impacted.insert( op.defendant );
      _impacted.insert( op.fee_payer() );
   }
   void operator()( const reply_p2p_dispute_operation& op )
   {
      _impacted.insert( op.arbitr );
      _impacted.insert( op.fee_payer() );
   }
   void operator()( const resolve_p2p_dispute_operation& op )
   {
      _impacted.insert( op.winner );
      _impacted.insert( op.looser );
      _impacted.insert( op.fee_payer() );
   }
   // FINANCIAL
   void operator()( const credit_system_get_operation& op )
   {
      _impacted.insert( op.fee_payer() );
   }
   void operator()( const credit_total_repay_operation& op )
   {
      _impacted.insert( op.creditor );
      _impacted.insert( op.fee_payer() );
   }
   void operator()( const credit_repay_operation& op )
   {
      _impacted.insert( op.debitor );  
      _impacted.insert( op.fee_payer() );
   }
   void operator()( const credit_offer_create_operation& op )
   {
      _impacted.insert( op.fee_payer() );
   }
   void operator()( const credit_offer_cancel_operation& op )
   {
      _impacted.insert( op.fee_payer() );
   }
   void operator()( const credit_offer_fill_operation& op )
   {
      _impacted.insert( op.creditor );
      _impacted.insert( op.fee_payer() );
   }
   void operator()( const pledge_offer_give_create_operation& op )
   {
      _impacted.insert( op.fee_payer() );
   }
   void operator()( const pledge_offer_take_create_operation& op )
   {
      _impacted.insert( op.fee_payer() );
   }
   void operator()( const pledge_offer_cancel_operation& op )
   {
      _impacted.insert( op.fee_payer() );
   }
   void operator()( const pledge_offer_fill_operation& op )
   {
      _impacted.insert( op.debitor );
      _impacted.insert( op.creditor );
   }
   void operator()( const pledge_offer_repay_operation& op )
   {
      _impacted.insert( op.creditor );
      _impacted.insert( op.fee_payer() );
   }
   void operator()( const pledge_offer_auto_repay_operation& op )
   {
      _impacted.insert( op.creditor );
      _impacted.insert( op.fee_payer() );
   }

   void operator()( const committee_member_update_gamezone_parameters_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // account_id_type()

   }
   void operator()( const committee_member_update_staking_parameters_operation& op )
   {
      _impacted.insert( op.fee_payer() ); // account_id_type()

   }

   void operator()( const poc_vote_operation& op ) {
      _impacted.insert( op.fee_payer() );      
   }
   void operator()( const poc_stak_operation& op ) {
      _impacted.insert( op.fee_payer() );            
   }
   void operator()( const poc_staking_referal_operation& op ) {
       _impacted.insert( op.account );
       _impacted.insert( op.fee_payer() );      
   }
   void operator()( const exchange_silver_operation& op ) {
      _impacted.insert( op.fee_payer() );                  
   }

   void operator()( const buy_gcwd_operation& op )
   {
      _impacted.insert( op.fee_payer());
   }

   void operator()( const approved_transfer_create_operation& op )
   {
      _impacted.insert( op.fee_payer());
      _impacted.insert( op.to);
      _impacted.insert( op.arbitr);
   }

   void operator()( const approved_transfer_approve_operation& op )
   {
      _impacted.insert( op.fee_payer() );
      _impacted.insert( op.to );
      _impacted.insert( op.arbitr);
   }

   void operator()( const approved_transfer_cancel_operation& op )
   {
      _impacted.insert( op.fee_payer() );
      _impacted.insert( op.to );
      _impacted.insert( op.arbitr );
   }

   void operator()( const approved_transfer_open_dispute_operation& op )
   {
      _impacted.insert( op.from);
      _impacted.insert( op.fee_payer());
      _impacted.insert( op.arbitr);
   }

   void operator()( const approved_transfer_resolve_dispute_operation& op )
   {
      _impacted.insert( op.from);
      _impacted.insert( op.to);
      _impacted.insert( op.fee_payer());
   }

   void operator()( const mass_payment_operation& op )
   {
      _impacted.insert( op.fee_payer());
   }

   void operator()( const mass_payment_pay_operation& op )
   {
      _impacted.insert( op.to);
   }

   void operator()( const gr_team_create_operation& op )
   {
      _impacted.insert( op.fee_payer());
   }
   void operator()( const gr_team_delete_operation& op )
   {
      _impacted.insert( op.fee_payer());
   }
   void operator()( const gr_invite_send_operation& op )
   {
      _impacted.insert( op.fee_payer());
      _impacted.insert( op.player);

   }
   void operator()( const gr_invite_accept_operation& op )
   {
      _impacted.insert( op.fee_payer());
      _impacted.insert( op.captain);

   }
   void operator()( const gr_player_remove_operation& op )
   {
      _impacted.insert( op.fee_payer());
      _impacted.insert( op.player);

   }
   void operator()( const gr_team_leave_operation& op )
   {
      _impacted.insert( op.fee_payer());
      _impacted.insert( op.captain);
   }
   void operator()( const gr_vote_operation& op )
   {
      _impacted.insert( op.fee_payer());
   }
   void operator()( const gr_assign_rank_operation& op )
   {
      _impacted.insert( op.fee_payer());
   }
   void operator()( const gr_pay_rank_reward_operation& op )
   {
      _impacted.insert( op.fee_payer());
   }
   void operator()( const gr_pay_top_reward_operation& op )
   {
      _impacted.insert( op.fee_payer());
   }
   void operator()( const gr_apostolos_operation& op )
   {
      _impacted.insert( op.fee_payer());
   }
   void operator()( const gr_range_bet_operation& op )
   {
      _impacted.insert( op.fee_payer());
   }

   void operator()( const gr_team_bet_operation& op ) {
      _impacted.insert( op.fee_payer());
   }
   void operator()( const gr_range_bet_win_operation& op ) {
      _impacted.insert( op.fee_payer());
   }
   void operator()( const gr_range_bet_loose_operation& op ) {
      _impacted.insert( op.fee_payer());
   }
   void operator()( const gr_team_bet_win_operation& op ) {
      _impacted.insert( op.fee_payer());
   }
   void operator()( const gr_team_bet_loose_operation& op ) {
      _impacted.insert( op.fee_payer());
   }

   void operator()( const gr_range_bet_cancel_operation& op ) {
      _impacted.insert( op.fee_payer());
   }
   
   void operator()( const gr_team_bet_cancel_operation& op ) {
      _impacted.insert( op.fee_payer());
   }

};

void graphene::chain::operation_get_impacted_accounts( const operation& op, flat_set<account_id_type>& result )
{
  get_impacted_account_visitor vtor = get_impacted_account_visitor( result );
  op.visit( vtor );
}

void graphene::chain::transaction_get_impacted_accounts( const transaction& tx, flat_set<account_id_type>& result )
{
  for( const auto& op : tx.operations )
    operation_get_impacted_accounts( op, result );
}

void get_relevant_accounts( const object* obj, flat_set<account_id_type>& accounts )
{
   if( obj->id.space() == protocol_ids )
   {
      switch( (object_type)obj->id.type() )
      {
        case null_object_type:
        case base_object_type:
        case OBJECT_TYPE_COUNT:
           return;
        case account_object_type:{
           accounts.insert( obj->id );
           break;
        } case asset_object_type:{
           const auto& aobj = dynamic_cast<const asset_object*>(obj);
           FC_ASSERT( aobj != nullptr );
           accounts.insert( aobj->issuer );
           break;
        } case force_settlement_object_type:{
           const auto& aobj = dynamic_cast<const force_settlement_object*>(obj);
           FC_ASSERT( aobj != nullptr );
           accounts.insert( aobj->owner );
           break;
        } case committee_member_object_type:{
           const auto& aobj = dynamic_cast<const committee_member_object*>(obj);
           FC_ASSERT( aobj != nullptr );
           accounts.insert( aobj->committee_member_account );
           break;
        } case witness_object_type:{
           const auto& aobj = dynamic_cast<const witness_object*>(obj);
           FC_ASSERT( aobj != nullptr );
           accounts.insert( aobj->witness_account );
           break;
        } case limit_order_object_type:{
           const auto& aobj = dynamic_cast<const limit_order_object*>(obj);
           FC_ASSERT( aobj != nullptr );
           accounts.insert( aobj->seller );
           break;
        } case call_order_object_type:{
           const auto& aobj = dynamic_cast<const call_order_object*>(obj);
           FC_ASSERT( aobj != nullptr );
           accounts.insert( aobj->borrower );
           break;
        } case custom_object_type:{
          break;
        } case proposal_object_type:{
           const auto& aobj = dynamic_cast<const proposal_object*>(obj);
           FC_ASSERT( aobj != nullptr );
           transaction_get_impacted_accounts( aobj->proposed_transaction, accounts );
           break;
        } case operation_history_object_type:{
           const auto& aobj = dynamic_cast<const operation_history_object*>(obj);
           FC_ASSERT( aobj != nullptr );
           operation_get_impacted_accounts( aobj->op, accounts );
           break;
        } case withdraw_permission_object_type:{
           const auto& aobj = dynamic_cast<const withdraw_permission_object*>(obj);
           FC_ASSERT( aobj != nullptr );
           accounts.insert( aobj->withdraw_from_account );
           accounts.insert( aobj->authorized_account );
           break;
        } case vesting_balance_object_type:{
           const auto& aobj = dynamic_cast<const vesting_balance_object*>(obj);
           FC_ASSERT( aobj != nullptr );
           accounts.insert( aobj->owner );
           break;
        } case worker_object_type:{
           const auto& aobj = dynamic_cast<const worker_object*>(obj);
           FC_ASSERT( aobj != nullptr );
           accounts.insert( aobj->worker_account );
           break;
        } case balance_object_type:{
           /** these are free from any accounts */
           break;
        }
        case flipcoin_object_type:{
           const auto& aobj = dynamic_cast<const flipcoin_object*>(obj);
           FC_ASSERT( aobj != nullptr );
           accounts.insert( aobj->bettor );
           break;
        }
        case lottery_goods_object_type:{
           const auto& aobj = dynamic_cast<const lottery_goods_object*>(obj);
           FC_ASSERT( aobj != nullptr );
           accounts.insert( aobj->owner );
           break;
        }
        case matrix_rooms_object_type:{
           const auto& aobj = dynamic_cast<const matrix_rooms_object*>(obj);
           FC_ASSERT( aobj != nullptr );
           accounts.insert( aobj->matrix_player );
           break;
        } 
        case credit_offer_object_type:{
           const auto& aobj = dynamic_cast<const credit_offer_object*>(obj);
           FC_ASSERT( aobj != nullptr );
           accounts.insert( aobj->creditor );
           break;
        } 
        case pledge_offer_object_type:{
           const auto& aobj = dynamic_cast<const pledge_offer_object*>(obj);
           FC_ASSERT( aobj != nullptr );
           accounts.insert( aobj->creator );           
           break;
        } 
        case approved_transfer_object_type:{
           const auto& aobj = dynamic_cast<const approved_transfer_object*>(obj);
           FC_ASSERT( aobj != nullptr );
           accounts.insert( aobj->from );           
           break;
        } 
        case matrix_object_type:{
           /** these are free from any accounts */
           break;
        }

        case p2p_adv_object_type:{
           const auto& aobj = dynamic_cast<const p2p_adv_object*>(obj);
           FC_ASSERT( aobj != nullptr );
           accounts.insert( aobj->p2p_gateway );
           break;
        }

        case p2p_order_object_type:{
           const auto& aobj = dynamic_cast<const p2p_order_object*>(obj);
           FC_ASSERT( aobj != nullptr );
           accounts.insert( aobj->p2p_client );
           break;
        }

        case gr_team_object_type:{
           const auto& aobj = dynamic_cast<const gr_team_object*>(obj);
           FC_ASSERT( aobj != nullptr );
           accounts.insert( aobj->captain );
           break;
        }

        case gr_invite_object_type:{
           const auto& aobj = dynamic_cast<const gr_invite_object*>(obj);
           FC_ASSERT( aobj != nullptr );
           accounts.insert( aobj->captain );
           break;
        }

        case gr_votes_object_type:{
           const auto& aobj = dynamic_cast<const gr_votes_object*>(obj);
           FC_ASSERT( aobj != nullptr );
           accounts.insert( aobj->player );
           break;
        }
        case gr_range_bet_object_type:{
           /** these are free from any accounts */
           break;
        }
        case gr_team_bet_object_type:{
           /** these are free from any accounts */
           break;
        }
      }
   }
   else if( obj->id.space() == implementation_ids )
   {
      switch( (impl_object_type)obj->id.type() )
      {
             case impl_global_property_object_type:
              break;
             case impl_dynamic_global_property_object_type:
              break;
             case impl_reserved0_object_type:
              break;
             case impl_asset_dynamic_data_type:
              break;
             case impl_asset_bitasset_data_type:
              break;
             case impl_account_balance_object_type:{
              const auto& aobj = dynamic_cast<const account_balance_object*>(obj);
              FC_ASSERT( aobj != nullptr );
              accounts.insert( aobj->owner );
              break;
           } case impl_account_statistics_object_type:{
              const auto& aobj = dynamic_cast<const account_statistics_object*>(obj);
              FC_ASSERT( aobj != nullptr );
              accounts.insert( aobj->owner );
              break;
           } case impl_transaction_object_type:{
              const auto& aobj = dynamic_cast<const transaction_object*>(obj);
              FC_ASSERT( aobj != nullptr );
              transaction_get_impacted_accounts( aobj->trx, accounts );
              break;
           } case impl_blinded_balance_object_type:{
              const auto& aobj = dynamic_cast<const blinded_balance_object*>(obj);
              FC_ASSERT( aobj != nullptr );
              for( const auto& a : aobj->owner.account_auths )
                accounts.insert( a.first );
              break;
           } case impl_block_summary_object_type:
              break;
             case impl_account_transaction_history_object_type: {
              const auto& aobj = dynamic_cast<const account_transaction_history_object*>(obj);
              FC_ASSERT( aobj != nullptr );
              accounts.insert( aobj->account );
              break;
           } case impl_chain_property_object_type:
              break;
             case impl_witness_schedule_object_type:
              break;
             case impl_budget_record_object_type:
              break;
             case impl_special_authority_object_type:
              break;
             case impl_buyback_object_type:
              break;
             case impl_fba_accumulator_object_type:
              break;
             case impl_collateral_bid_object_type:{
              const auto& aobj = dynamic_cast<const collateral_bid_object*>(obj);
              FC_ASSERT( aobj != nullptr );
              accounts.insert( aobj->bidder );
              break;
           }
      }
   }
} // end get_relevant_accounts( const object* obj, flat_set<account_id_type>& accounts )

namespace graphene { namespace chain {

void database::notify_applied_block( const signed_block& block )
{
   GRAPHENE_TRY_NOTIFY( applied_block, block )
}

void database::notify_on_pending_transaction( const signed_transaction& tx )
{
   GRAPHENE_TRY_NOTIFY( on_pending_transaction, tx )
}

void database::notify_changed_objects()
{ try {
   if( _undo_db.enabled() ) 
   {
      const auto& head_undo = _undo_db.head();

      // New
      if( !new_objects.empty() )
      {
        vector<object_id_type> new_ids;  new_ids.reserve(head_undo.new_ids.size());
        flat_set<account_id_type> new_accounts_impacted;
        for( const auto& item : head_undo.new_ids )
        {
          new_ids.push_back(item);
          auto obj = find_object(item);
          if(obj != nullptr)
            get_relevant_accounts(obj, new_accounts_impacted);
        }

        if( new_ids.size() )
           GRAPHENE_TRY_NOTIFY( new_objects, new_ids, new_accounts_impacted)
      }

      // Changed
      if( !changed_objects.empty() )
      {
        vector<object_id_type> changed_ids;  changed_ids.reserve(head_undo.old_values.size());
        flat_set<account_id_type> changed_accounts_impacted;
        for( const auto& item : head_undo.old_values )
        {
          changed_ids.push_back(item.first);
          get_relevant_accounts(item.second.get(), changed_accounts_impacted);
        }

        if( changed_ids.size() )
           GRAPHENE_TRY_NOTIFY( changed_objects, changed_ids, changed_accounts_impacted)
      }

      // Removed
      if( !removed_objects.empty() )
      {
        vector<object_id_type> removed_ids; removed_ids.reserve( head_undo.removed.size() );
        vector<const object*> removed; removed.reserve( head_undo.removed.size() );
        flat_set<account_id_type> removed_accounts_impacted;
        for( const auto& item : head_undo.removed )
        {
          removed_ids.emplace_back( item.first );
          auto obj = item.second.get();
          removed.emplace_back( obj );
          get_relevant_accounts(obj, removed_accounts_impacted);
        }

        if( removed_ids.size() )
           GRAPHENE_TRY_NOTIFY( removed_objects, removed_ids, removed, removed_accounts_impacted)
      }
   }
} FC_CAPTURE_AND_LOG( (0) ) }

} }
