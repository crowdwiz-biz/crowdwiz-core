#include <graphene/chain/protocol/send_message.hpp>

namespace graphene { namespace chain {

share_type send_message_operation::calculate_fee( const fee_parameters_type& schedule )const
{
   share_type core_fee_required = schedule.fee;
   core_fee_required += calculate_data_fee( fc::raw::pack_size(memo), schedule.price_per_kbyte );
   return core_fee_required;
}


void send_message_operation::validate()const
{
   FC_ASSERT( fee.amount >= 0 );
   FC_ASSERT( from != to );
   FC_ASSERT( fc::raw::pack_size(memo)>0 );
}


} } // graphene::chain