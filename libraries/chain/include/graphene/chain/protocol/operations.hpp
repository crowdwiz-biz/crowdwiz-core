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
#pragma once
#include <graphene/chain/protocol/base.hpp>
#include <graphene/chain/protocol/account.hpp>
#include <graphene/chain/protocol/assert.hpp>
#include <graphene/chain/protocol/asset_ops.hpp>
#include <graphene/chain/protocol/balance.hpp>
#include <graphene/chain/protocol/custom.hpp>
#include <graphene/chain/protocol/committee_member.hpp>
#include <graphene/chain/protocol/confidential.hpp>
#include <graphene/chain/protocol/fba.hpp>
#include <graphene/chain/protocol/market.hpp>
#include <graphene/chain/protocol/proposal.hpp>
#include <graphene/chain/protocol/transfer.hpp>
#include <graphene/chain/protocol/vesting.hpp>
#include <graphene/chain/protocol/withdraw_permission.hpp>
#include <graphene/chain/protocol/witness.hpp>
#include <graphene/chain/protocol/worker.hpp>
#include <graphene/chain/protocol/gamezone.hpp>
#include <graphene/chain/protocol/send_message.hpp>
#include <graphene/chain/protocol/exchange.hpp>
#include <graphene/chain/protocol/financial.hpp>
#include <graphene/chain/protocol/poc.hpp>
#include <graphene/chain/protocol/greatrace.hpp>

namespace graphene { namespace chain {

   /**
    * @ingroup operations
    *
    * Defines the set of valid operations as a discriminated union type.
    */
   typedef fc::static_variant<
      /* 0   */   transfer_operation,
      /* 1   */   limit_order_create_operation,
      /* 2   */   limit_order_cancel_operation,
      /* 3   */   call_order_update_operation,
      /* 4   */   fill_order_operation,           // VIRTUAL
      /* 5   */   account_create_operation,
      /* 6   */   account_update_operation,
      /* 7   */   account_whitelist_operation,
      /* 8   */   account_upgrade_operation,
      /* 9   */   account_transfer_operation,
      /* 10  */   asset_create_operation,
      /* 11  */   asset_update_operation,
      /* 12  */   asset_update_bitasset_operation,
      /* 13  */   asset_update_feed_producers_operation,
      /* 14  */   asset_issue_operation,
      /* 15  */   asset_reserve_operation,
      /* 16  */   asset_fund_fee_pool_operation,
      /* 17  */   asset_settle_operation,
      /* 18  */   asset_global_settle_operation,
      /* 19  */   asset_publish_feed_operation,
      /* 20  */   witness_create_operation,
      /* 21  */   witness_update_operation,
      /* 22  */   proposal_create_operation,
      /* 23  */   proposal_update_operation,
      /* 24  */   proposal_delete_operation,
      /* 25  */   withdraw_permission_create_operation,
      /* 26  */   withdraw_permission_update_operation,
      /* 27  */   withdraw_permission_claim_operation,
      /* 28  */   withdraw_permission_delete_operation,
      /* 29  */   committee_member_create_operation,
      /* 30  */   committee_member_update_operation,
      /* 31  */   committee_member_update_global_parameters_operation,
      /* 32  */   vesting_balance_create_operation,
      /* 33  */   vesting_balance_withdraw_operation,
      /* 34  */   worker_create_operation,
      /* 35  */   custom_operation,
      /* 36  */   assert_operation,
      /* 37  */   balance_claim_operation,
      /* 38  */   override_transfer_operation,
      /* 39  */   transfer_to_blind_operation,
      /* 40  */   blind_transfer_operation,
      /* 41  */   transfer_from_blind_operation,
      /* 42  */   asset_settle_cancel_operation,  // VIRTUAL
      /* 43  */   asset_claim_fees_operation,
      /* 44  */   fba_distribute_operation,       // VIRTUAL
      /* 45  */   bid_collateral_operation,
      /* 46  */   execute_bid_operation,          // VIRTUAL
      /* 47  */   asset_claim_pool_operation,
      /* 48  */   asset_update_issuer_operation,
      /* 49  */   account_status_upgrade_operation,
      /* 50  */   flipcoin_bet_operation,  //GAMEZONE
      /* 51  */   flipcoin_call_operation,  //GAMEZONE
      /* 52  */   flipcoin_win_operation,  //VOP
      /* 53  */   flipcoin_cancel_operation,  //VOP
      /* 54  */   flipcoin_loose_operation,  //VOP
      /* 55  */   lottery_goods_create_lot_operation,  // GAMEZONE
      /* 56  */   lottery_goods_buy_ticket_operation,  // GAMEZONE
      /* 57  */   lottery_goods_send_contacts_operation,  // GAMEZONE
      /* 58  */   lottery_goods_confirm_delivery_operation,  // GAMEZONE
      /* 59  */   lottery_goods_win_operation,  // GAMEZONE, VIRTUAL
      /* 60  */   lottery_goods_loose_operation,  // GAMEZONE, VIRTUAL
      /* 61  */   send_message_operation,
      /* 62  */   matrix_open_room_operation,
      /* 63  */   matrix_room_closed_operation, // GAMEZONE, VIRTUAL
      /* 64  */   matrix_cell_filled_operation, // GAMEZONE, VIRTUAL
      /* 65  */   create_p2p_adv_operation, // EXCHANGE
      /* 66  */   edit_p2p_adv_operation, // EXCHANGE
      /* 67  */   clear_p2p_adv_black_list_operation, // EXCHANGE
      /* 68  */   remove_from_p2p_adv_black_list_operation, // EXCHANGE
      /* 69  */   create_p2p_order_operation, // EXCHANGE
      /* 70  */   cancel_p2p_order_operation, // EXCHANGE
      /* 71  */   autocancel_p2p_order_operation, // EXCHANGE, VIRTUAL
      /* 72  */   autorefund_p2p_order_operation, // EXCHANGE, VIRTUAL
      /* 73  */   call_p2p_order_operation, // EXCHANGE
      /* 74  */   payment_p2p_order_operation, // EXCHANGE
      /* 75  */   release_p2p_order_operation, // EXCHANGE
      /* 76  */   open_p2p_dispute_operation, // EXCHANGE
      /* 77  */   reply_p2p_dispute_operation, // EXCHANGE
      /* 78  */   resolve_p2p_dispute_operation, // EXCHANGE
      /* 79  */   lottery_goods_refund_operation,  // GAMEZONE, VIRTUAL
      /* 80  */   credit_system_get_operation, //FINANCIAL
      /* 81  */   credit_total_repay_operation, //FINANCIAL, VIRTUAL
      /* 82  */   credit_repay_operation, //FINANCIAL
      /* 83  */   credit_offer_create_operation, //FINANCIAL
      /* 84  */   credit_offer_cancel_operation, //FINANCIAL
      /* 85  */   credit_offer_fill_operation, //FINANCIAL
      /* 86  */   pledge_offer_give_create_operation, //FINANCIAL
      /* 87  */   pledge_offer_take_create_operation, //FINANCIAL
      /* 88  */   pledge_offer_cancel_operation, //FINANCIAL
      /* 89  */   pledge_offer_fill_operation, //FINANCIAL
      /* 90  */   pledge_offer_repay_operation, //FINANCIAL
      /* 91  */   pledge_offer_auto_repay_operation, //FINANCIAL, VIRTUAL
      /* 92  */   committee_member_update_gamezone_parameters_operation,
      /* 93  */   committee_member_update_staking_parameters_operation,
      /* 94  */   poc_vote_operation, //PoC
      /* 95  */   poc_stak_operation, //PoC
      /* 96  */   poc_staking_referal_operation, //PoC, VIRTUAL
      /* 97  */   exchange_silver_operation,//PoC
      /* 98  */   buy_gcwd_operation,
      /* 99  */   approved_transfer_create_operation,
      /* 100 */   approved_transfer_approve_operation,
      /* 101 */   approved_transfer_cancel_operation,
      /* 102 */   approved_transfer_open_dispute_operation,
      /* 103 */   approved_transfer_resolve_dispute_operation,
      /* 104 */   mass_payment_operation,
      /* 105 */   mass_payment_pay_operation,
      /* 106 */   change_referrer_operation,
      /* 107 */   gr_team_create_operation,
      /* 108 */   gr_team_delete_operation,
      /* 109 */   gr_invite_send_operation,
      /* 110 */   gr_invite_accept_operation,
      /* 111 */   gr_player_remove_operation,
      /* 112 */   gr_team_leave_operation,
      /* 113 */   gr_vote_operation,
      /* 114 */   gr_assign_rank_operation,
      /* 115 */   gr_pay_rank_reward_operation,
      /* 116 */   gr_pay_top_reward_operation,
      /* 117 */   gr_apostolos_operation,
      /* 118 */   gr_range_bet_operation,
      /* 119 */   gr_team_bet_operation,
      /* 120 */   gr_range_bet_win_operation,
      /* 121 */   gr_range_bet_loose_operation,
      /* 122 */   gr_team_bet_win_operation,
      /* 123 */   gr_team_bet_loose_operation,
      /* 124 */   gr_range_bet_cancel_operation,
      /* 125 */   gr_team_bet_cancel_operation
         > operation;

   /// @} // operations group

   /**
    *  Appends required authorites to the result vector.  The authorities appended are not the
    *  same as those returned by get_required_auth 
    *
    *  @return a set of required authorities for @ref op
    */
   void operation_get_required_authorities( const operation& op, 
                                            flat_set<account_id_type>& active,
                                            flat_set<account_id_type>& owner,
                                            vector<authority>&  other );

   void operation_validate( const operation& op );

   /**
    *  @brief necessary to support nested operations inside the proposal_create_operation
    */
   struct op_wrapper
   {
      public:
         op_wrapper(const operation& op = operation()):op(op){}
         operation op;
   };

} } // graphene::chain

FC_REFLECT_TYPENAME( graphene::chain::operation )
FC_REFLECT( graphene::chain::op_wrapper, (op) )
