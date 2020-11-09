/*
 * Copyright (c) 2019 CrowdWiz., and contributors.
 *
 * The MIT License
 */

#pragma once
#include <graphene/chain/evaluator.hpp>
namespace graphene { namespace chain {

   class poc_vote_evaluator : public evaluator<poc_vote_evaluator>
   {
      public:
         typedef poc_vote_operation operation_type;

         void_result do_evaluate( const poc_vote_operation& o );
         void_result do_apply( const poc_vote_operation& o );
   };

   class poc_stak_evaluator : public evaluator<poc_stak_evaluator>
   {
      public:
         typedef poc_stak_operation operation_type;

         void_result do_evaluate( const poc_stak_operation& o );
         void_result do_apply( const poc_stak_operation& o );
   };

   class exchange_silver_evaluator : public evaluator<exchange_silver_evaluator>
   {
      public:
         typedef exchange_silver_operation operation_type;

         void_result do_evaluate( const exchange_silver_operation& o );
         void_result do_apply( const exchange_silver_operation& o );
   };

} } // graphene::chain 