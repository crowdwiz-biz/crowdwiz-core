/*
 * Copyright (c) 2019 CrowdWiz., and contributors.
 *
 * The MIT License
 */

#pragma once
#include <graphene/chain/protocol/base.hpp>
#include <graphene/chain/protocol/memo.hpp>


namespace graphene { namespace chain { 

   struct flipcoin_bet_operation : public base_operation
   {
      struct fee_parameters_type { uint64_t fee = 0; };

      asset             fee;
      account_id_type   bettor;
      asset             bet;
      uint8_t           nonce;

      account_id_type 	fee_payer()const { return bettor; }
      void            	validate()const;
   };

   struct flipcoin_call_operation : public base_operation
   {
      struct fee_parameters_type { share_type fee = 0; };

      asset             fee;
      flipcoin_id_type   flipcoin;
      account_id_type   caller;
      asset             bet;

      account_id_type 	fee_payer()const { return caller; }
      void            	validate()const;
   };

   struct flipcoin_win_operation : public base_operation
   {
      struct fee_parameters_type { share_type fee = 0; };

      asset             fee;
      flipcoin_id_type   flipcoin;
      account_id_type   winner;
      asset             payout;
      asset             referral_payout;

      account_id_type 	fee_payer()const { return winner; }
      void            	validate()const;
   };

   struct flipcoin_loose_operation : public base_operation
   {
      struct fee_parameters_type { share_type fee = 0; };

      asset             fee;
      flipcoin_id_type   flipcoin;
      account_id_type   looser;
      asset             bet;

      account_id_type 	fee_payer()const { return looser; }
      void            	validate()const;
   };

   struct flipcoin_cancel_operation : public base_operation
   {
      struct fee_parameters_type { share_type fee = 0; };

      asset             fee;
      flipcoin_id_type   flipcoin;
      account_id_type   bettor;
      asset             bet;

      account_id_type 	fee_payer()const { return bettor; }
      void            	validate()const;
   };

struct lottery_goods_create_lot_operation : public base_operation
   {
      struct fee_parameters_type { share_type fee = 20 * GRAPHENE_BLOCKCHAIN_PRECISION; };
      
      asset                   fee;
      account_id_type         owner;
      uint32_t                total_participants;
      asset                   ticket_price;
      uint16_t                latency_sec;
      string                  img_url;
      string                  description;

      account_id_type         fee_payer()const { return owner; }
      void                    validate()const;
   };

   struct lottery_goods_buy_ticket_operation : public base_operation
   {
      struct fee_parameters_type
      {
         share_type fee = 0;
         uint16_t lottery_goods_fee_percent = 10*GRAPHENE_1_PERCENT;
      };

      asset                   fee;
      lottery_goods_id_type   lot_id;
      account_id_type         participant;
      asset                   ticket_price;
      
      account_id_type         fee_payer()const { return participant; }
      void                    validate()const;  
      share_type              calculate_fee(const fee_parameters_type& k)const;   
   };

   struct lottery_goods_send_contacts_operation : public base_operation
   {
      struct fee_parameters_type { share_type fee = 0; };
      asset                      fee;
      lottery_goods_id_type      lot_id;
      account_id_type            winner;
      account_id_type            owner;
      memo_data                  winner_contacts;

      account_id_type            fee_payer()const { return winner; }
      void                       validate()const;        
   };

   struct lottery_goods_confirm_delivery_operation : public base_operation
   {
      struct fee_parameters_type { share_type fee = 0; };
      asset                      fee;
      lottery_goods_id_type      lot_id;
      account_id_type            winner;
      account_id_type            owner;
      
      account_id_type         fee_payer()const { return winner; }
      void                    validate()const;  
   };

   struct lottery_goods_win_operation : public base_operation // VIRTUAL
   {
      struct fee_parameters_type { share_type fee = 0; };
      asset                      fee;
      lottery_goods_id_type      lot_id;
      account_id_type            winner;
     
      account_id_type         fee_payer()const { return winner; }
      void                    validate()const;  
   };

   struct lottery_goods_loose_operation : public base_operation // VIRTUAL
   {
      struct fee_parameters_type { share_type fee = 0; }; 
      asset                      fee;
      lottery_goods_id_type      lot_id;
      account_id_type            looser;
     
      account_id_type         fee_payer()const { return looser; }
      void                    validate()const;  
   };
   struct lottery_goods_refund_operation : public base_operation // VIRTUAL
   {
      struct fee_parameters_type { share_type fee = 0; }; 
      asset                   fee;
      lottery_goods_id_type   lot_id;
      account_id_type         participant;
      asset                   ticket_price;
     
      account_id_type         fee_payer()const { return participant; }
      void                    validate()const;  
   };

   struct matrix_open_room_operation : public base_operation
   {
      struct fee_parameters_type
      {
         share_type fee = 0;
         uint16_t matrix_fee_percent = 10*GRAPHENE_1_PERCENT;
      };

      asset                   fee;
      matrix_id_type          matrix_id;
      account_id_type         player;
      uint8_t                 matrix_level;
      asset                   level_price;
      
      account_id_type         fee_payer()const { return player; }
      void                    validate()const;  
      share_type              calculate_fee(const fee_parameters_type& k)const;   
   };

   struct matrix_room_closed_operation : public base_operation // VIRTUAL
   {
      struct fee_parameters_type { share_type fee = 0; }; 
      asset                      fee;
      account_id_type            player;
      matrix_rooms_id_type       matrix_room;
      uint8_t                    matrix_level;
      asset                      level_reward;
    
      account_id_type         fee_payer()const { return player; }
      void                    validate()const;  
   };

   struct matrix_cell_filled_operation : public base_operation // VIRTUAL
   {
      struct fee_parameters_type { share_type fee = 0; }; 
      asset                      fee;
      account_id_type            player;
      matrix_rooms_id_type       matrix_room;
      uint8_t                    matrix_level;
      account_id_type            cell_player;
    
      account_id_type         fee_payer()const { return player; }
      void                    validate()const;  
   };

} } // graphene::chain
FC_REFLECT( graphene::chain::flipcoin_bet_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::flipcoin_bet_operation, (fee)(bettor)(bet)(nonce))

FC_REFLECT( graphene::chain::flipcoin_call_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::flipcoin_call_operation, (fee)(flipcoin)(caller)(bet))

FC_REFLECT( graphene::chain::flipcoin_win_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::flipcoin_win_operation, (fee)(flipcoin)(winner)(payout)(referral_payout))

FC_REFLECT( graphene::chain::flipcoin_loose_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::flipcoin_loose_operation, (fee)(flipcoin)(looser)(bet))

FC_REFLECT( graphene::chain::flipcoin_cancel_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::flipcoin_cancel_operation, (fee)(flipcoin)(bettor)(bet) )

FC_REFLECT( graphene::chain::lottery_goods_create_lot_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::lottery_goods_create_lot_operation, (fee)(owner)(total_participants)(ticket_price)(latency_sec)(img_url)(description) )

FC_REFLECT( graphene::chain::lottery_goods_buy_ticket_operation::fee_parameters_type, (fee)(lottery_goods_fee_percent) )
FC_REFLECT( graphene::chain::lottery_goods_buy_ticket_operation, (fee)(lot_id)(participant)(ticket_price) )

FC_REFLECT( graphene::chain::lottery_goods_send_contacts_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::lottery_goods_send_contacts_operation, (fee)(lot_id)(winner)(owner)(winner_contacts) )

FC_REFLECT( graphene::chain::lottery_goods_confirm_delivery_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::lottery_goods_confirm_delivery_operation, (fee)(lot_id)(winner)(owner) )

FC_REFLECT( graphene::chain::lottery_goods_win_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::lottery_goods_win_operation, (fee)(lot_id)(winner) )

FC_REFLECT( graphene::chain::lottery_goods_loose_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::lottery_goods_loose_operation, (fee)(lot_id)(looser) )

FC_REFLECT( graphene::chain::lottery_goods_refund_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::lottery_goods_refund_operation, (fee)(lot_id)(participant)(ticket_price) )

FC_REFLECT( graphene::chain::matrix_open_room_operation::fee_parameters_type, (fee)(matrix_fee_percent) )
FC_REFLECT( graphene::chain::matrix_open_room_operation, (fee)(matrix_id)(player)(matrix_level)(level_price))

FC_REFLECT( graphene::chain::matrix_room_closed_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::matrix_room_closed_operation, (fee)(player)(matrix_room)(matrix_level)(level_reward) )

FC_REFLECT( graphene::chain::matrix_cell_filled_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::matrix_cell_filled_operation, (fee)(player)(matrix_room)(matrix_level)(cell_player) )