#pragma once
#include <graphene/chain/protocol/operations.hpp>
#include <graphene/chain/evaluator.hpp>
#include <graphene/chain/database.hpp>

namespace graphene { namespace chain {

   class send_message_evaluator : public evaluator<send_message_evaluator>
   {
      public:
         typedef send_message_operation operation_type;

         void_result do_evaluate( const send_message_operation& o );
         void_result do_apply( const send_message_operation& o );
   };
} } // graphene::chain