#include <graphene/chain/send_message_evaluator.hpp>
#include <graphene/chain/account_object.hpp>
#include <graphene/chain/exceptions.hpp>
#include <graphene/chain/hardfork.hpp>
#include <graphene/chain/is_authorized_asset.hpp>

namespace graphene { namespace chain {
void_result send_message_evaluator::do_evaluate( const send_message_operation& op )
{ try {
      return void_result();
}  FC_CAPTURE_AND_RETHROW( (op) ) }

void_result send_message_evaluator::do_apply( const send_message_operation& op )
{ try {
   return void_result();
} FC_CAPTURE_AND_RETHROW( (op) ) }


} } // graphene::chain