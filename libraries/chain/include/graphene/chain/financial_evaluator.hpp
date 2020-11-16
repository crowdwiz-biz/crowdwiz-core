/*
 * Copyright (c) 2019 CrowdWiz., and contributors.
 *
 * The MIT License
 */

#pragma once
#include <graphene/chain/evaluator.hpp>
#include <graphene/chain/financial_object.hpp>

namespace graphene { namespace chain {

   class credit_system_get_evaluator : public evaluator<credit_system_get_evaluator>
   {
      public:
         typedef credit_system_get_operation operation_type;

         void_result do_evaluate( const credit_system_get_operation& o );
         void_result do_apply( const credit_system_get_operation& o );
   };

   class credit_total_repay_evaluator : public evaluator<credit_total_repay_evaluator>
   {
      public:
         typedef credit_total_repay_operation operation_type;

         void_result do_evaluate( const credit_total_repay_operation& o );
         void_result do_apply( const credit_total_repay_operation& o );
   };

   class credit_repay_evaluator : public evaluator<credit_repay_evaluator>
   {
      public:
         typedef credit_repay_operation operation_type;

         void_result do_evaluate( const credit_repay_operation& o );
         void_result do_apply( const credit_repay_operation& o );
   };

   class credit_offer_create_evaluator : public evaluator<credit_offer_create_evaluator>
   {
      public:
         typedef credit_offer_create_operation operation_type;

         void_result do_evaluate( const credit_offer_create_operation& o );
         object_id_type do_apply( const credit_offer_create_operation& o );
   };

   class credit_offer_cancel_evaluator : public evaluator<credit_offer_cancel_evaluator>
   {
      public:
         typedef credit_offer_cancel_operation operation_type;

         void_result do_evaluate( const credit_offer_cancel_operation& o );
         void_result do_apply( const credit_offer_cancel_operation& o );
   };

   class credit_offer_fill_evaluator : public evaluator<credit_offer_fill_evaluator>
   {
      public:
         typedef credit_offer_fill_operation operation_type;

         void_result do_evaluate( const credit_offer_fill_operation& o );
         void_result do_apply( const credit_offer_fill_operation& o );
   };

   class pledge_offer_give_create_evaluator : public evaluator<pledge_offer_give_create_evaluator>
   {
      public:
         typedef pledge_offer_give_create_operation operation_type;

         void_result do_evaluate( const pledge_offer_give_create_operation& o );
         object_id_type do_apply( const pledge_offer_give_create_operation& o );
   };

   class pledge_offer_take_create_evaluator : public evaluator<pledge_offer_take_create_evaluator>
   {
      public:
         typedef pledge_offer_take_create_operation operation_type;

         void_result do_evaluate( const pledge_offer_take_create_operation& o );
         object_id_type do_apply( const pledge_offer_take_create_operation& o );
   };

   class pledge_offer_cancel_evaluator : public evaluator<pledge_offer_cancel_evaluator>
   {
      public:
         typedef pledge_offer_cancel_operation operation_type;

         void_result do_evaluate( const pledge_offer_cancel_operation& o );
         void_result do_apply( const pledge_offer_cancel_operation& o );
   };

   class pledge_offer_fill_evaluator : public evaluator<pledge_offer_fill_evaluator>
   {
      public:
         typedef pledge_offer_fill_operation operation_type;

         void_result do_evaluate( const pledge_offer_fill_operation& o );
         void_result do_apply( const pledge_offer_fill_operation& o );
   };

   class pledge_offer_repay_evaluator : public evaluator<pledge_offer_repay_evaluator>
   {
      public:
         typedef pledge_offer_repay_operation operation_type;

         void_result do_evaluate( const pledge_offer_repay_operation& o );
         void_result do_apply( const pledge_offer_repay_operation& o );
   };

   class pledge_offer_auto_repay_evaluator : public evaluator<pledge_offer_auto_repay_evaluator>
   {
      public:
         typedef pledge_offer_auto_repay_operation operation_type;

         void_result do_evaluate( const pledge_offer_auto_repay_operation& o );
         void_result do_apply( const pledge_offer_auto_repay_operation& o );
   };

   class buy_gcwd_evaluator : public evaluator<buy_gcwd_evaluator>
   {
      public:
         typedef buy_gcwd_operation operation_type;

         void_result do_evaluate( const buy_gcwd_operation& o );
         void_result do_apply( const buy_gcwd_operation& o );
   };

   class approved_transfer_create_evaluator : public evaluator<approved_transfer_create_evaluator>
   {
      public:
         typedef approved_transfer_create_operation operation_type;

         void_result do_evaluate( const approved_transfer_create_operation& o );
         object_id_type do_apply( const approved_transfer_create_operation& o );
   };

   class approved_transfer_approve_evaluator : public evaluator<approved_transfer_approve_evaluator>
   {
      public:
         typedef approved_transfer_approve_operation operation_type;

         void_result do_evaluate( const approved_transfer_approve_operation& o );
         void_result do_apply( const approved_transfer_approve_operation& o );
   };

   class approved_transfer_open_dispute_evaluator : public evaluator<approved_transfer_open_dispute_evaluator>
   {
      public:
         typedef approved_transfer_open_dispute_operation operation_type;

         void_result do_evaluate( const approved_transfer_open_dispute_operation& o );
         void_result do_apply( const approved_transfer_open_dispute_operation& o );
   };

   class approved_transfer_resolve_dispute_evaluator : public evaluator<approved_transfer_resolve_dispute_evaluator>
   {
      public:
         typedef approved_transfer_resolve_dispute_operation operation_type;

         void_result do_evaluate( const approved_transfer_resolve_dispute_operation& o );
         void_result do_apply( const approved_transfer_resolve_dispute_operation& o );
   };

   class mass_payment_evaluator : public evaluator<mass_payment_evaluator>
   {
      public:
         typedef mass_payment_operation operation_type;

         void_result do_evaluate( const mass_payment_operation& o );
         void_result do_apply( const mass_payment_operation& o );
   };

} } // graphene::chain 