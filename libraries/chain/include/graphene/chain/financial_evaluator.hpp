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

} } // graphene::chain 