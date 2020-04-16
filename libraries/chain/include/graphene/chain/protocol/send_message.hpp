#pragma once
#include <graphene/chain/protocol/base.hpp>
#include <graphene/chain/protocol/memo.hpp>

namespace graphene { namespace chain {

   struct send_message_operation : public base_operation
   {
      struct fee_parameters_type {
         uint64_t fee       = 0.1 * GRAPHENE_BLOCKCHAIN_PRECISION;
         uint32_t price_per_kbyte = 0.1 * GRAPHENE_BLOCKCHAIN_PRECISION; 
      };

      asset            fee;
      account_id_type  from;
      account_id_type  to;
      memo_data        memo;

      account_id_type fee_payer()const { return from; }
      void            validate()const;
      share_type      calculate_fee(const fee_parameters_type& k)const;
   };
}} // graphene::chain

FC_REFLECT( graphene::chain::send_message_operation::fee_parameters_type, (fee)(price_per_kbyte) )
FC_REFLECT( graphene::chain::send_message_operation, (fee)(from)(to)(memo) )