/*
 * Copyright (c) 2019 CrowdWiz., and contributors.
 *
 * The MIT License
 */

#pragma once
#include <graphene/chain/evaluator.hpp>
#include <graphene/chain/gamezone_object.hpp>

namespace graphene { namespace chain {

   class flipcoin_bet_evaluator : public evaluator<flipcoin_bet_evaluator>
   {
      public:
         typedef flipcoin_bet_operation operation_type;

         void_result do_evaluate( const flipcoin_bet_operation& o );
         object_id_type do_apply( const flipcoin_bet_operation& o );
   };

   class flipcoin_call_evaluator : public evaluator<flipcoin_call_evaluator>
   {
      public:
         typedef flipcoin_call_operation operation_type;

         void_result do_evaluate( const flipcoin_call_operation& o );
         void_result do_apply( const flipcoin_call_operation& o );
        const flipcoin_object* flipcoin;
   };

   class lottery_goods_create_lot_evaluator : public evaluator<lottery_goods_create_lot_evaluator>
   {
      public:
         typedef lottery_goods_create_lot_operation operation_type;

         void_result do_evaluate( const lottery_goods_create_lot_operation& o );
         object_id_type do_apply( const lottery_goods_create_lot_operation& o );
   };


   class lottery_goods_buy_ticket_evaluator : public evaluator<lottery_goods_buy_ticket_evaluator>
   {
      public:
         typedef lottery_goods_buy_ticket_operation operation_type;

         void_result do_evaluate( const lottery_goods_buy_ticket_operation& o );
         void_result do_apply( const lottery_goods_buy_ticket_operation& o );
         const lottery_goods_object* lot_obj;
   };
   
   
   class lottery_goods_send_contacts_evaluator : public evaluator<lottery_goods_send_contacts_evaluator>
   {
      public:
         typedef lottery_goods_send_contacts_operation operation_type;

         void_result do_evaluate( const lottery_goods_send_contacts_operation& o );
         void_result do_apply( const lottery_goods_send_contacts_operation& o );
         const lottery_goods_object* lot_obj;
   };


   class lottery_goods_confirm_delivery_evaluator : public evaluator<lottery_goods_confirm_delivery_evaluator>
   {
      public:
         typedef lottery_goods_confirm_delivery_operation operation_type;

         void_result do_evaluate( const lottery_goods_confirm_delivery_operation& o );
         void_result do_apply( const lottery_goods_confirm_delivery_operation& o );
         const lottery_goods_object* lot_obj;
   };
   class matrix_open_room_evaluator : public evaluator<matrix_open_room_evaluator>
   {
      public:
         typedef matrix_open_room_operation operation_type;

         void_result do_evaluate( const matrix_open_room_operation& o );
         object_id_type do_apply( const matrix_open_room_operation& o );
         const matrix_object* matrix;
   };

} } // graphene::chain 