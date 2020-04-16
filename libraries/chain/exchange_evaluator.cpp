#include <graphene/chain/exchange_evaluator.hpp>
#include <graphene/chain/exchange_object.hpp>
#include <graphene/chain/account_object.hpp>
#include <graphene/chain/exceptions.hpp>
#include <graphene/chain/hardfork.hpp>
#include <graphene/chain/is_authorized_asset.hpp>

namespace graphene { namespace chain {

share_type cut_fee_exchange(share_type a, uint16_t p)
{
	if( a == 0 || p == 0 )
		return 0;
	if( p == GRAPHENE_100_PERCENT )
		return a;

	fc::uint128 r(a.value);
	r *= p;
	r /= GRAPHENE_100_PERCENT;
	return r.to_uint64();
}

void_result create_p2p_adv_evaluator::do_evaluate( const create_p2p_adv_operation& op )
{ try {
	const database& d = db();
	const auto head_time = d.head_block_time();
/*	const auto& ch_parameters = d.get_global_properties().parameters;
	FC_ASSERT(*ch_parameters.allow_p2p_exchange);*/

	const account_object& from_account = op.p2p_gateway(d);
	FC_ASSERT(from_account.referral_status_type == 4);
	FC_ASSERT(from_account.referral_status_expiration_date >= head_time);

	return void_result();
} FC_CAPTURE_AND_RETHROW( (op) ) }

object_id_type create_p2p_adv_evaluator::do_apply( const create_p2p_adv_operation& op )
{ try {
	const auto& new_p2p_adv_object = db().create<p2p_adv_object>([&](p2p_adv_object &obj) {
		obj.p2p_gateway = op.p2p_gateway;
		obj.adv_type = op.adv_type;
		obj.adv_description = op.adv_description;
		obj.max_cwd = op.max_cwd;
		obj.min_cwd = op.min_cwd;
		obj.price = op.price;
		obj.currency = op.currency;
		obj.status = 1;
		obj.min_p2p_complete_deals = op.min_p2p_complete_deals;
		obj.min_account_status = op.min_account_status;
		obj.timelimit_for_approve = op.timelimit_for_approve;
		obj.timelimit_for_reply = op.timelimit_for_reply;
		obj.geo = op.geo;
	});
	return new_p2p_adv_object.id;
} FC_CAPTURE_AND_RETHROW( (op) ) }


void_result edit_p2p_adv_evaluator::do_evaluate( const edit_p2p_adv_operation& op )
{ try {
	const database& d = db();
	const auto& p2p_adv_object = d.get(op.p2p_adv);
	FC_ASSERT(p2p_adv_object.p2p_gateway == op.p2p_gateway);
	return void_result();
} FC_CAPTURE_AND_RETHROW( (op) ) }

void_result edit_p2p_adv_evaluator::do_apply( const edit_p2p_adv_operation& op )
{ try {
	database& d = db();

	d.modify(d.get(op.p2p_adv), [&]( p2p_adv_object &obj )
	{
		obj.p2p_gateway = op.p2p_gateway;
		obj.adv_type = op.adv_type; 
		obj.adv_description = op.adv_description;
		obj.max_cwd = op.max_cwd;
		obj.min_cwd = op.min_cwd;
		obj.price = op.price;
		obj.currency = op.currency;
		obj.status = op.status;
		obj.min_p2p_complete_deals = op.min_p2p_complete_deals;
		obj.min_account_status = op.min_account_status;
		obj.timelimit_for_approve = op.timelimit_for_approve;
		obj.timelimit_for_reply = op.timelimit_for_reply;
		obj.geo = op.geo;
	});

	return void_result();

} FC_CAPTURE_AND_RETHROW( (op) ) }

void_result clear_p2p_adv_black_list_evaluator::do_evaluate( const clear_p2p_adv_black_list_operation& op )
{ try {
	const database& d = db();
	const auto& p2p_adv_object = d.get(op.p2p_adv);
	FC_ASSERT(p2p_adv_object.p2p_gateway == op.p2p_gateway);

	return void_result();
} FC_CAPTURE_AND_RETHROW( (op) ) }

void_result clear_p2p_adv_black_list_evaluator::do_apply( const clear_p2p_adv_black_list_operation& op )
{ try {
	database& d = db();
	d.modify(d.get(op.p2p_adv), [&]( p2p_adv_object &obj )
	{
		obj.black_list.clear();
	});
	return void_result();
} FC_CAPTURE_AND_RETHROW( (op) ) }

void_result remove_from_p2p_adv_black_list_evaluator::do_evaluate( const remove_from_p2p_adv_black_list_operation& op )
{ try {
	const database& d = db();
	const auto& p2p_adv_object = d.get(op.p2p_adv);
	FC_ASSERT(p2p_adv_object.p2p_gateway == op.p2p_gateway);

	return void_result();
} FC_CAPTURE_AND_RETHROW( (op) ) }

void_result remove_from_p2p_adv_black_list_evaluator::do_apply( const remove_from_p2p_adv_black_list_operation& op )
{ try {
	database& d = db();
	d.modify(d.get(op.p2p_adv), [&]( p2p_adv_object &obj )
	{
		obj.black_list.erase(op.blacklisted);
	});
	return void_result();
} FC_CAPTURE_AND_RETHROW( (op) ) }


void_result create_p2p_order_evaluator::do_evaluate( const create_p2p_order_operation& op )
{ try {
	const database& d = db();

	const account_object& client = op.p2p_client(d);
	const account_object& gateway = op.p2p_gateway(d);
	
	const asset_object& asset_type     = op.amount.asset_id(d);
	const auto head_time = d.head_block_time();
	FC_ASSERT(asset_type.symbol == "CWD");

	const auto& client_stats = d.get_account_stats_by_owner(client.id);
	FC_ASSERT(client_stats.p2p_banned < head_time);

	const auto& gateway_stats = d.get_account_stats_by_owner(gateway.id);
	FC_ASSERT(gateway_stats.p2p_gateway_active);

	// check that only one order for client in this gateway
	vector<p2p_order_object> total_open_orders;
	const auto& by_client_gateway_idx = d.get_index_type<p2p_order_index>().indices().get<by_client_gateway>();
	auto po_itr_by_client_gateway = by_client_gateway_idx.equal_range(boost::make_tuple( op.p2p_client, op.p2p_gateway ));
	std::for_each(po_itr_by_client_gateway.first, po_itr_by_client_gateway.second, [&total_open_orders](const p2p_order_object& po_obj) {
		total_open_orders.emplace_back(po_obj);
	});
	FC_ASSERT(total_open_orders.size() == 0);

	const auto& by_client_idx = d.get_index_type<p2p_order_index>().indices().get<by_client>();
	auto po_itr_by_client = by_client_idx.equal_range(op.p2p_client);
	std::for_each(po_itr_by_client.first, po_itr_by_client.second, [&total_open_orders](const p2p_order_object& po_obj) {
		total_open_orders.emplace_back(po_obj);
	});

	if (client.referral_status_expiration_date >= head_time) {
		switch( client.referral_status_type ) {
			case 1:
				FC_ASSERT(total_open_orders.size() < 2);
				break;
			case 2:
				FC_ASSERT(total_open_orders.size() < 3);
				break;
			case 3:
				FC_ASSERT(total_open_orders.size() < 4);
				break;
			case 4:
				FC_ASSERT(total_open_orders.size() < 5);
				break;
			default:
				FC_ASSERT(total_open_orders.size() < 1);
				break;
		}
	} else {
		FC_ASSERT(total_open_orders.size() == 0);
	}

	const auto& p2p_adv_object = d.get(op.p2p_adv);
	FC_ASSERT(p2p_adv_object.status == 1);
	FC_ASSERT(p2p_adv_object.p2p_gateway == op.p2p_gateway);
	FC_ASSERT(p2p_adv_object.price == op.price);

	if (p2p_adv_object.adv_type == false) {
		bool insufficient_balance = d.get_balance( client, asset_type ).amount >= op.amount.amount;
		FC_ASSERT(insufficient_balance);
		FC_ASSERT(op.payment_details);
	}
	else {
		bool insufficient_balance = d.get_balance( gateway, asset_type ).amount >= op.amount.amount;
		FC_ASSERT(insufficient_balance);
	}
	if (p2p_adv_object.min_account_status>0) {
		FC_ASSERT(client.referral_status_type >= p2p_adv_object.min_account_status);
		FC_ASSERT(client.referral_status_expiration_date >= head_time);
	}

	FC_ASSERT(client_stats.p2p_complete_deals >= p2p_adv_object.min_p2p_complete_deals);
	FC_ASSERT((p2p_adv_object.max_cwd >= op.amount.amount)&&(p2p_adv_object.min_cwd <= op.amount.amount));

	if (!p2p_adv_object.black_list.empty()) {
		FC_ASSERT( p2p_adv_object.black_list.find(client.id) == p2p_adv_object.black_list.end(), "Your account blacklisted by gateway!" );
	}

	return void_result();
} FC_CAPTURE_AND_RETHROW( (op) ) }

object_id_type create_p2p_order_evaluator::do_apply( const create_p2p_order_operation& op )
{ try {
	database& d = db();
	const auto& p2p_adv_object = d.get(op.p2p_adv);
	const auto head_time = d.head_block_time();
	const auto& new_p2p_order_object = d.create<p2p_order_object>([&](p2p_order_object &obj) {
		obj.p2p_adv = op.p2p_adv;
		obj.order_type = p2p_adv_object.adv_type;
		obj.amount = op.amount;
		obj.price = p2p_adv_object.price;
		obj.currency = p2p_adv_object.currency;
		obj.p2p_gateway = p2p_adv_object.p2p_gateway;
		obj.p2p_client = op.p2p_client;

		obj.status = (p2p_adv_object.adv_type ? 1 : 2);
		if (p2p_adv_object.adv_type) {
			obj.time_for_approve = head_time + p2p_adv_object.timelimit_for_approve;
			obj.time_for_reply = fc::time_point_sec();
		}
		else {
			obj.time_for_approve = fc::time_point_sec();
			obj.time_for_reply = head_time + p2p_adv_object.timelimit_for_reply;
		}
		if (op.payment_details) {
			obj.payment_details = op.payment_details;
		}
	});

	if (p2p_adv_object.adv_type == false) {
		d.adjust_balance( op.p2p_client, -op.amount );
	}

	return new_p2p_order_object.id;
} FC_CAPTURE_AND_RETHROW( (op) ) }

void_result cancel_p2p_order_evaluator::do_evaluate( const cancel_p2p_order_operation& op )
{ try {
	const database& d = db();
	const auto& p2p_order_obj = d.get(op.p2p_order);
	FC_ASSERT(p2p_order_obj.p2p_gateway == op.p2p_gateway);
	if (p2p_order_obj.order_type) {
		FC_ASSERT(p2p_order_obj.status == 1);
	} else {
		FC_ASSERT(p2p_order_obj.status == 2);
	}
	return void_result();
} FC_CAPTURE_AND_RETHROW( (op) ) }

void_result cancel_p2p_order_evaluator::do_apply( const cancel_p2p_order_operation& op )
{ try {
	database& d = db();
	const auto& p2p_order_obj = d.get(op.p2p_order);

	if (op.blacklist) {
		d.modify(d.get(p2p_order_obj.p2p_adv), [&]( p2p_adv_object &obj )
		{
			obj.black_list.insert(p2p_order_obj.p2p_client);
		});
	}

	d.modify(d.get_account_stats_by_owner(p2p_order_obj.p2p_gateway), [&]( account_statistics_object &obj )
	{
		obj.p2p_canceled_deals++;
	});

	if (p2p_order_obj.order_type == false) {
		d.adjust_balance( p2p_order_obj.p2p_client, p2p_order_obj.amount );
	}

	d.remove(p2p_order_obj);

	return void_result();
} FC_CAPTURE_AND_RETHROW( (op) ) }


void_result call_p2p_order_evaluator::do_evaluate( const call_p2p_order_operation& op )
{ try {
	const database& d = db();
	const auto& p2p_order_obj = d.get(op.p2p_order);
	FC_ASSERT(p2p_order_obj.order_type);
	FC_ASSERT(p2p_order_obj.status == 1);
	FC_ASSERT(p2p_order_obj.p2p_gateway == op.p2p_gateway);
	FC_ASSERT(p2p_order_obj.price == op.price);
	FC_ASSERT(p2p_order_obj.amount == op.amount);
	const account_object& p2p_gateway = d.get(p2p_order_obj.p2p_gateway);
	const asset_object&   asset_type   = p2p_order_obj.amount.asset_id(d);
	bool insufficient_balance = d.get_balance( p2p_gateway, asset_type ).amount >= p2p_order_obj.amount.amount;
	FC_ASSERT( insufficient_balance );
	return void_result();
} FC_CAPTURE_AND_RETHROW( (op) ) }

void_result call_p2p_order_evaluator::do_apply( const call_p2p_order_operation& op )
{ try {
	database& d = db();
	const auto& p2p_order_obj = d.get(op.p2p_order);
	const auto& p2p_adv_object = d.get(p2p_order_obj.p2p_adv);
	const auto head_time = d.head_block_time();

	d.modify(d.get(op.p2p_order), [&]( p2p_order_object &obj )
	{
		obj.status = 2;
		obj.time_for_reply = head_time + p2p_adv_object.timelimit_for_reply;
		obj.time_for_approve = fc::time_point_sec();
		obj.payment_details = op.payment_details;
	});
	d.adjust_balance( op.p2p_gateway, -p2p_order_obj.amount );

	return void_result();
} FC_CAPTURE_AND_RETHROW( (op) ) }


void_result payment_p2p_order_evaluator::do_evaluate( const payment_p2p_order_operation& op )
{ try {
	const database& d = db();

	const auto& p2p_order_obj = d.get(op.p2p_order);
	FC_ASSERT(p2p_order_obj.status == 2);

	if (p2p_order_obj.order_type) {
		FC_ASSERT(p2p_order_obj.p2p_client == op.paying_account);
	} else {
		FC_ASSERT(p2p_order_obj.p2p_gateway == op.paying_account);
	}

	return void_result();
} FC_CAPTURE_AND_RETHROW( (op) ) }

void_result payment_p2p_order_evaluator::do_apply( const payment_p2p_order_operation& op )
{ try {
	database& d = db();

	d.modify(d.get(op.p2p_order), [&]( p2p_order_object &obj )
	{
		obj.status = 3;
		obj.time_for_reply = fc::time_point_sec();
		if (op.file_hash)
			obj.file_hash = op.file_hash;
	});
	return void_result();
} FC_CAPTURE_AND_RETHROW( (op) ) }


void_result release_p2p_order_evaluator::do_evaluate( const release_p2p_order_operation& op )
{ try {
	const database& d = db();
	const auto& p2p_order_obj = d.get(op.p2p_order);
	FC_ASSERT(p2p_order_obj.status >= 2);

	if (p2p_order_obj.order_type) {
		FC_ASSERT(p2p_order_obj.p2p_client == op.paying_account);
		FC_ASSERT(p2p_order_obj.p2p_gateway == op.recieving_account);
	} else {
		FC_ASSERT(p2p_order_obj.p2p_gateway == op.paying_account);
		FC_ASSERT(p2p_order_obj.p2p_client == op.recieving_account);
	}	
	return void_result();
} FC_CAPTURE_AND_RETHROW( (op) ) }

void_result release_p2p_order_evaluator::do_apply( const release_p2p_order_operation& op )
{ try {
	database& d = db();
	auto& p2p_order_obj = d.get(op.p2p_order);

	if (p2p_order_obj.order_type) {
		d.adjust_balance( p2p_order_obj.p2p_client, p2p_order_obj.amount );
	} else {
		d.adjust_balance( p2p_order_obj.p2p_gateway, p2p_order_obj.amount );
	};

	d.modify(d.get_account_stats_by_owner(p2p_order_obj.p2p_gateway), [&]( account_statistics_object &obj )
	{
		obj.p2p_complete_deals++;
		obj.p2p_deals_volume += p2p_order_obj.amount.amount;
	});

	d.modify(d.get_account_stats_by_owner(p2p_order_obj.p2p_client), [&]( account_statistics_object &obj )
	{
		obj.p2p_complete_deals++;
		obj.p2p_deals_volume += p2p_order_obj.amount.amount;
	});

	d.remove(p2p_order_obj);

	return void_result();
} FC_CAPTURE_AND_RETHROW( (op) ) }

void_result open_p2p_dispute_evaluator::do_evaluate( const open_p2p_dispute_operation& op )
{ try {
	const database& d = db();
	const auto& p2p_order_obj = d.get(op.p2p_order);
	const auto head_time = d.head_block_time();

	FC_ASSERT(p2p_order_obj.status == 3);
	FC_ASSERT(p2p_order_obj.p2p_gateway == op.account || p2p_order_obj.p2p_client == op.account);

    account_id_type admin_whitelist = account_id_type(3334);
    auto& admin = d.get<account_object>(admin_whitelist);
    FC_ASSERT( admin.whitelisted_accounts.find(op.arbitr) != admin.whitelisted_accounts.end(), "Not admin account!" );

	return void_result();
} FC_CAPTURE_AND_RETHROW( (op) ) }

void_result open_p2p_dispute_evaluator::do_apply( const open_p2p_dispute_operation& op )
{ try {
	database& d = db();
	d.modify(d.get(op.p2p_order), [&]( p2p_order_object &obj )
	{
		obj.status = 4;
	});
	return void_result();
} FC_CAPTURE_AND_RETHROW( (op) ) }


void_result reply_p2p_dispute_evaluator::do_evaluate( const reply_p2p_dispute_operation& op )
{ try {
	const database& d = db();
	const auto& p2p_order_obj = d.get(op.p2p_order);
	FC_ASSERT( p2p_order_obj.status == 4 );

	if (p2p_order_obj.order_type) {
		FC_ASSERT(p2p_order_obj.p2p_client == op.account);
	}
	else {
		FC_ASSERT(p2p_order_obj.p2p_gateway == op.account);
	}

    account_id_type admin_whitelist = account_id_type(3334);
    auto& admin = d.get<account_object>(admin_whitelist);
    FC_ASSERT( admin.whitelisted_accounts.find(op.arbitr) != admin.whitelisted_accounts.end(), "Not admin account!" );

	return void_result();
} FC_CAPTURE_AND_RETHROW( (op) ) }

void_result reply_p2p_dispute_evaluator::do_apply( const reply_p2p_dispute_operation& op )
{ try {
	database& d = db();
	d.modify(d.get(op.p2p_order), [&]( p2p_order_object &obj )
	{
		obj.status = 5;
	});
	return void_result();
} FC_CAPTURE_AND_RETHROW( (op) ) }


void_result resolve_p2p_dispute_evaluator::do_evaluate( const resolve_p2p_dispute_operation& op )
{ try {
	const database& d = db();
	const auto& p2p_order_obj = d.get(op.p2p_order);
	FC_ASSERT(p2p_order_obj.status == 4 || p2p_order_obj.status == 5);

	FC_ASSERT((p2p_order_obj.p2p_gateway == op.winner)
	||(p2p_order_obj.p2p_client == op.winner));

	FC_ASSERT((p2p_order_obj.p2p_gateway == op.looser)
	||(p2p_order_obj.p2p_client == op.looser));

    account_id_type admin_whitelist = account_id_type(3334);
    auto& admin = d.get<account_object>(admin_whitelist);
    FC_ASSERT( admin.whitelisted_accounts.find(op.arbitr) != admin.whitelisted_accounts.end(), "Not arbitr account!" );

	return void_result();
} FC_CAPTURE_AND_RETHROW( (op) ) }

void_result resolve_p2p_dispute_evaluator::do_apply( const resolve_p2p_dispute_operation& op )
{ try {
	database& d = db();
	auto& p2p_order_obj = d.get(op.p2p_order);
	//const auto& ch_parameters = d.get_global_properties().parameters;
	uint64_t ch_parameters_arbitr_comission = (1*GRAPHENE_1_PERCENT);

	share_type arbitr_comission = cut_fee_exchange(p2p_order_obj.amount.amount, ch_parameters_arbitr_comission);
	d.adjust_balance( op.arbitr, arbitr_comission );
	d.adjust_balance( op.winner, p2p_order_obj.amount.amount-arbitr_comission );


	d.modify(d.get_account_stats_by_owner(op.winner), [&]( account_statistics_object &obj )
	{
		obj.p2p_complete_deals++;
		obj.p2p_deals_volume += p2p_order_obj.amount.amount-arbitr_comission;
	});

	d.modify(d.get_account_stats_by_owner(op.looser), [&]( account_statistics_object &obj )
	{
		obj.p2p_arbitrage_loose++;
	});


	d.remove(p2p_order_obj);

	return void_result();
} FC_CAPTURE_AND_RETHROW( (op) ) }

} } // graphene::chain