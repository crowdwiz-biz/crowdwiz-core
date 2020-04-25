/*
 * Copyright (c) 2019 CrowdWiz., and contributors.
 *
 * The MIT License
 */

#pragma once
#include <graphene/chain/protocol/asset.hpp>
#include <graphene/chain/protocol/types.hpp>
#include <graphene/chain/protocol/operations.hpp>

#include <graphene/db/object.hpp>
#include <graphene/db/generic_index.hpp>
#include <boost/multi_index/composite_key.hpp>

namespace graphene { namespace chain {
	using namespace graphene::db;

	class p2p_adv_object : public abstract_object<p2p_adv_object>
	{
		public:
			static const uint8_t space_id = protocol_ids;
			static const uint8_t type_id = p2p_adv_object_type;

			account_id_type              p2p_gateway;
			bool                         adv_type = true;
			string                       adv_description;
			share_type                   max_cwd;
			share_type                   min_cwd;
			share_type                   price;
			set<account_id_type> 		 black_list;
			uint8_t                      status = 1;
			uint32_t                     min_p2p_complete_deals = 0;
			uint8_t                      min_account_status = 0;
			uint32_t           			 timelimit_for_approve = 3600;
			uint32_t                     timelimit_for_reply = 3600;
			string                       geo;
			string                       currency;
	};

	struct by_id;
	struct by_status;
	struct by_gateway_status;

	using p2p_adv_multi_index_type = multi_index_container<
		p2p_adv_object,
		indexed_by<
			ordered_unique< tag<by_id>,
				member<object, object_id_type, &object::id>
			>,
			ordered_non_unique< tag<by_status>,
				composite_key<
					p2p_adv_object,
					member<p2p_adv_object, uint8_t, &p2p_adv_object::status>,
					member< object, object_id_type, &object::id>
				>
			>,
			ordered_non_unique< tag<by_gateway_status>,
				composite_key< p2p_adv_object,
					member<p2p_adv_object, account_id_type, &p2p_adv_object::p2p_gateway>,
					member<p2p_adv_object, uint8_t, &p2p_adv_object::status>,
					member< object, object_id_type, &object::id >
				>
			>
		>
	>;
	using p2p_adv_index = generic_index<p2p_adv_object, p2p_adv_multi_index_type>;

	class p2p_order_object : public abstract_object<p2p_order_object>
	{
		public:
			static const uint8_t space_id = protocol_ids;
			static const uint8_t type_id = p2p_order_object_type;

			p2p_adv_id_type              p2p_adv;
			bool                         order_type;
			asset                        amount;
			share_type					 price;
			string                       currency;
			account_id_type              p2p_gateway;
			account_id_type              p2p_client;
			uint8_t                      status = 1;
			time_point_sec     			 time_for_approve;
			time_point_sec               time_for_reply;
			optional<memo_data>          payment_details;
			optional<memo_data>          file_hash;
			optional<account_id_type>     arbitrage_initiator;

	};
	struct by_id;
	struct by_status;
	struct by_client_status;
	struct by_client;
	struct by_gateway_status;
	struct by_client_gateway;
	struct by_client_gateway_status;
	struct by_time_for_approve;
	struct by_time_for_reply;
	struct by_status_time_for_approve;
	struct by_status_time_for_reply;

	using p2p_order_multi_index_type = multi_index_container<
		p2p_order_object,
		indexed_by<
			ordered_unique< tag<by_id>,
				member<object, object_id_type, &object::id>
			>,
			ordered_non_unique< tag<by_status>,
				composite_key<
					p2p_order_object,
					member<p2p_order_object, uint8_t, &p2p_order_object::status>,
					member< object, object_id_type, &object::id>
				>
			>,
			ordered_non_unique< tag<by_client>,
				composite_key< p2p_order_object,
					member<p2p_order_object, account_id_type, &p2p_order_object::p2p_client>,
					member< object, object_id_type, &object::id >
				>
			>,
			ordered_non_unique< tag<by_client_status>,
				composite_key< p2p_order_object,
					member<p2p_order_object, account_id_type, &p2p_order_object::p2p_client>,
					member<p2p_order_object, uint8_t, &p2p_order_object::status>,
					member< object, object_id_type, &object::id >
				>
			>,
			ordered_non_unique< tag<by_gateway_status>,
				composite_key< p2p_order_object,
					member<p2p_order_object, account_id_type, &p2p_order_object::p2p_gateway>,
					member<p2p_order_object, uint8_t, &p2p_order_object::status>,
					member< object, object_id_type, &object::id >
				>
			>,
			ordered_non_unique< tag<by_client_gateway>,
				composite_key< p2p_order_object,
					member<p2p_order_object, account_id_type, &p2p_order_object::p2p_client>,
					member<p2p_order_object, account_id_type, &p2p_order_object::p2p_gateway>,
					member< object, object_id_type, &object::id >
				>
			>,
			ordered_non_unique< tag<by_client_gateway_status>,
				composite_key< p2p_order_object,
					member<p2p_order_object, account_id_type, &p2p_order_object::p2p_client>,
					member<p2p_order_object, account_id_type, &p2p_order_object::p2p_gateway>,
					member<p2p_order_object, uint8_t, &p2p_order_object::status>,
					member< object, object_id_type, &object::id >
				>
			>,
			ordered_non_unique< tag<by_time_for_approve>,
				composite_key< p2p_order_object,
					member<p2p_order_object, time_point_sec, &p2p_order_object::time_for_approve>,
					member< object, object_id_type, &object::id >
				>
			>,
			ordered_non_unique< tag<by_time_for_reply>,
				composite_key< p2p_order_object,
					member<p2p_order_object, time_point_sec, &p2p_order_object::time_for_reply>,
					member< object, object_id_type, &object::id >
				>
			>,
			ordered_non_unique< tag<by_status_time_for_approve>,
				composite_key< p2p_order_object,
					member<p2p_order_object, uint8_t, &p2p_order_object::status>,
					member<p2p_order_object, time_point_sec, &p2p_order_object::time_for_approve>,
					member< object, object_id_type, &object::id >
				>
			>,
			ordered_non_unique< tag<by_status_time_for_reply>,
				composite_key< p2p_order_object,
					member<p2p_order_object, uint8_t, &p2p_order_object::status>,
					member<p2p_order_object, time_point_sec, &p2p_order_object::time_for_reply>,
					member< object, object_id_type, &object::id >
				>
			>
		>
	>;
	using p2p_order_index = generic_index<p2p_order_object, p2p_order_multi_index_type>;

} } // graphene::chain

FC_REFLECT_DERIVED( graphene::chain::p2p_adv_object, (graphene::db::object),
	(p2p_gateway)
	(adv_type)
	(adv_description)
	(max_cwd)
	(min_cwd)
	(price)
	(black_list)
	(status)
	(min_p2p_complete_deals)
	(min_account_status)
	(timelimit_for_approve)
	(timelimit_for_reply)
	(geo)
	(currency)
) 

FC_REFLECT_DERIVED( graphene::chain::p2p_order_object, (graphene::db::object),
	(p2p_adv)
	(order_type)
	(amount)
	(price)
	(currency)
	(p2p_gateway)
	(p2p_client)
	(status)
	(time_for_approve)
	(time_for_reply)
	(payment_details)
	(file_hash)
	(arbitrage_initiator)
) 
