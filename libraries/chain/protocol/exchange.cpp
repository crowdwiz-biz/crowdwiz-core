#include <graphene/chain/protocol/exchange.hpp>

namespace graphene { namespace chain {

share_type create_p2p_adv_operation::calculate_fee( const fee_parameters_type& schedule )const
{
	share_type core_fee_required = schedule.fee;
	core_fee_required += calculate_data_fee( fc::raw::pack_size(adv_description), schedule.price_per_kbyte );
	core_fee_required += calculate_data_fee( fc::raw::pack_size(geo), schedule.price_per_kbyte );
	return core_fee_required;
}


void create_p2p_adv_operation::validate()const
{
	FC_ASSERT( fee.amount >= 0 );
	FC_ASSERT( timelimit_for_approve >= 3600 );
	FC_ASSERT( timelimit_for_reply >= 3600 );
	FC_ASSERT( fc::raw::pack_size(adv_description)>=10 );
	FC_ASSERT( fc::raw::pack_size(adv_description)<257 );
	FC_ASSERT( fc::raw::pack_size(geo)<65 );
	FC_ASSERT( fc::raw::pack_size(currency)<20 );
	FC_ASSERT( max_cwd > 0 );
	FC_ASSERT( min_cwd > 0 );
	FC_ASSERT( max_cwd >= min_cwd );
	FC_ASSERT( price > 0 );
	FC_ASSERT( min_account_status >= 0 );
	FC_ASSERT( min_account_status <= 4 );
	FC_ASSERT( min_p2p_complete_deals >= 0 );
}


share_type edit_p2p_adv_operation::calculate_fee( const fee_parameters_type& schedule )const
{
	share_type core_fee_required = schedule.fee;
	core_fee_required += calculate_data_fee( fc::raw::pack_size(adv_description), schedule.price_per_kbyte );
	core_fee_required += calculate_data_fee( fc::raw::pack_size(geo), schedule.price_per_kbyte );
	return core_fee_required;
}


void edit_p2p_adv_operation::validate()const
{
	FC_ASSERT( fee.amount >= 0 );
	FC_ASSERT( status >= 0 );
	FC_ASSERT( status <= 1 );
	FC_ASSERT( timelimit_for_approve >= 3600 );
	FC_ASSERT( timelimit_for_reply >= 3600 );
	FC_ASSERT( fc::raw::pack_size(adv_description)>=10 );
	FC_ASSERT( fc::raw::pack_size(adv_description)<257 );
	FC_ASSERT( fc::raw::pack_size(geo)<65 );
	FC_ASSERT( fc::raw::pack_size(currency)<20 );
	FC_ASSERT( max_cwd > 0 );
	FC_ASSERT( min_cwd > 0 );
	FC_ASSERT( max_cwd >= min_cwd );
	FC_ASSERT( price > 0 );
	FC_ASSERT( min_account_status >= 0 );
	FC_ASSERT( min_account_status <= 4 );
	FC_ASSERT( min_p2p_complete_deals >= 0 );
}

void clear_p2p_adv_black_list_operation::validate()const
{
	FC_ASSERT( fee.amount >= 0 );
}

void remove_from_p2p_adv_black_list_operation::validate()const
{
	FC_ASSERT( fee.amount >= 0 );
}

void create_p2p_order_operation::validate()const
{
	FC_ASSERT( fee.amount >= 0 );
	FC_ASSERT( amount.amount > 0 );
	FC_ASSERT( price > 0 );
	FC_ASSERT( p2p_gateway != p2p_client );

	if (payment_details) {
		FC_ASSERT( fc::raw::pack_size(payment_details)<1000 );
		FC_ASSERT( fc::raw::pack_size(payment_details)>=10 );
	}
}

void cancel_p2p_order_operation::validate()const
{
	FC_ASSERT( fee.amount >= 0 );
}

void autocancel_p2p_order_operation::validate()const
{
	FC_ASSERT( fee.amount >= 0 );
}

void autorefund_p2p_order_operation::validate()const
{
	FC_ASSERT( fee.amount >= 0 );
}

void call_p2p_order_operation::validate()const
{
	FC_ASSERT( fee.amount >= 0 );
	FC_ASSERT( amount.amount > 0 );
	FC_ASSERT( price > 0 );
	FC_ASSERT( fc::raw::pack_size(payment_details)>=10 );
	FC_ASSERT( fc::raw::pack_size(payment_details)<1000 );
}

void payment_p2p_order_operation::validate()const
{
	FC_ASSERT( fee.amount >= 0 );
	if (file_hash)
		FC_ASSERT( fc::raw::pack_size(file_hash)>=10);
		FC_ASSERT( fc::raw::pack_size(file_hash)<1000 );
}

void release_p2p_order_operation::validate()const
{
	FC_ASSERT( fee.amount >= 0 );
}

void open_p2p_dispute_operation::validate()const
{
	FC_ASSERT( fee.amount >= 0 );
	FC_ASSERT( fc::raw::pack_size(contact_details)>=10 );
	FC_ASSERT( fc::raw::pack_size(contact_details)<1000 );
}

void reply_p2p_dispute_operation::validate()const
{
	FC_ASSERT( fee.amount >= 0 );
	FC_ASSERT( fc::raw::pack_size(contact_details)>=10 );
	FC_ASSERT( fc::raw::pack_size(contact_details)<1000 );
}

void resolve_p2p_dispute_operation::validate()const
{
	FC_ASSERT( fee.amount >= 0 );
	FC_ASSERT( winner != looser );
}

} } // graphene::chain