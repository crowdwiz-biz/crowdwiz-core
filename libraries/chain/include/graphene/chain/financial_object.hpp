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

	/* 
	* CREDIT OFFER OBJECT
	*/	

	class credit_offer_object : public abstract_object<credit_offer_object>
	{
		public:
			static const uint8_t space_id = protocol_ids;
			static const uint8_t type_id = credit_offer_object_type;		

			account_id_type creditor;
			share_type min_income;
			asset credit_amount;
			asset repay_amount;		
	};

    struct by_id;
	struct by_creditor;
    using credit_offer_multi_index_type = multi_index_container<
		credit_offer_object,
		indexed_by<
			ordered_unique< 
				tag<by_id>,
				member<object, object_id_type, &object::id>
			>,
			ordered_non_unique< 
				tag<by_creditor>,
				composite_key<
					credit_offer_object,
					member<credit_offer_object, account_id_type, &credit_offer_object::creditor>,
					member< object, object_id_type, &object::id>
				>
			>
		>
	>;

   using credit_offer_index = generic_index<credit_offer_object, credit_offer_multi_index_type>;

	/* 
	* PLEDGE OFFER OBJECT
	*/

	class pledge_offer_object : public abstract_object<pledge_offer_object>
	{
		public:
			static const uint8_t space_id = protocol_ids;
			static const uint8_t type_id = pledge_offer_object_type;		

			account_id_type creator;

			account_id_type debitor;
			account_id_type creditor;

			asset pledge_amount;
			asset credit_amount;
			asset repay_amount;		
			uint16_t pledge_days; 
			time_point_sec expiration;
			uint8_t status = 0; //0 = active, 1 = filled
	};

    struct by_id;
    struct by_status;
    struct by_expiration;

	using pledge_offer_multi_index_type = multi_index_container<
		pledge_offer_object,
		indexed_by<
			ordered_unique< 
				tag<by_id>,
				member<object, object_id_type, &object::id>
			>,
			ordered_non_unique< 
				tag<by_status>,member<pledge_offer_object, uint8_t, &pledge_offer_object::status>
			>,
			ordered_non_unique< 
				tag<by_expiration>,
					member< pledge_offer_object, time_point_sec, &pledge_offer_object::expiration>
			>
		>
	>;
   using pledge_offer_index = generic_index<pledge_offer_object, pledge_offer_multi_index_type>;



} } // graphene::chain

FC_REFLECT_DERIVED( graphene::chain::credit_offer_object, (graphene::db::object),
	(creditor)
	(min_income)
	(credit_amount)
	(repay_amount)
)

FC_REFLECT_DERIVED( graphene::chain::pledge_offer_object, (graphene::db::object),
	(creator)
	(debitor)
	(creditor)
	(pledge_amount)
	(credit_amount)
	(repay_amount)
	(pledge_days)
	(expiration)
	(status)
)