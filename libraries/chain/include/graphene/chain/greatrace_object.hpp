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

	class gr_team_object : public abstract_object<gr_team_object>
	{
		public:
			static const uint8_t space_id = protocol_ids;
			static const uint8_t type_id = gr_team_object_type;

			string name;
			string description;
			string logo;
			account_id_type captain;
			uint8_t players_num;
			share_type gr_interval_2_volume;
			share_type gr_interval_4_volume;
			share_type gr_interval_6_volume;
			share_type gr_interval_9_volume;
			share_type gr_interval_11_volume;
			share_type gr_interval_13_volume;
			uint8_t last_gr_rank;

			inline share_type first_half_volume() const {return gr_interval_2_volume + gr_interval_4_volume + gr_interval_6_volume;}
			inline share_type second_half_volume() const {return gr_interval_9_volume + gr_interval_11_volume + gr_interval_13_volume;}
			inline share_type total_volume() const {return gr_interval_2_volume + gr_interval_4_volume + gr_interval_6_volume + gr_interval_9_volume + gr_interval_11_volume + gr_interval_13_volume;}
	};

	struct by_id;
	struct by_gr_interval_2_volume;
	struct by_gr_interval_4_volume;
	struct by_gr_interval_6_volume;
	struct by_gr_interval_9_volume;
	struct by_gr_interval_11_volume;
	struct by_gr_interval_13_volume;
	struct by_last_gr_rank;
	struct by_total_first_half_volume;
	struct by_total_second_half_volume;
	struct by_total_volume;

	typedef multi_index_container<
		gr_team_object,
		indexed_by<
		ordered_unique< tag<by_id>, member< object, object_id_type, &object::id > >,
		ordered_non_unique<tag<by_gr_interval_2_volume>, member<gr_team_object, share_type, &gr_team_object::gr_interval_2_volume>>,
		ordered_non_unique<tag<by_gr_interval_4_volume>, member<gr_team_object, share_type, &gr_team_object::gr_interval_4_volume>>,
		ordered_non_unique<tag<by_gr_interval_6_volume>, member<gr_team_object, share_type, &gr_team_object::gr_interval_6_volume>>,
		ordered_non_unique<tag<by_gr_interval_9_volume>, member<gr_team_object, share_type, &gr_team_object::gr_interval_9_volume>>,
		ordered_non_unique<tag<by_gr_interval_11_volume>, member<gr_team_object, share_type, &gr_team_object::gr_interval_11_volume>>,
		ordered_non_unique<tag<by_gr_interval_13_volume>, member<gr_team_object, share_type, &gr_team_object::gr_interval_13_volume>>,
		ordered_non_unique<tag<by_last_gr_rank>, member<gr_team_object, uint8_t, &gr_team_object::last_gr_rank>>,
		ordered_non_unique<tag<by_total_first_half_volume>, member<gr_team_object, share_type, &gr_team_object::first_half_volume>>,
		ordered_non_unique<tag<by_total_second_half_volume>, member<gr_team_object, share_type, &gr_team_object::second_half_volume>>,
		ordered_non_unique<tag<by_total_volume>, member<gr_team_object, share_type, &gr_team_object::total_volume>>
	> gr_team_multi_index_type;
	using gr_team_index = generic_index<gr_team_object, gr_team_multi_index_type>;

	class gr_invite_object : public abstract_object<gr_invite_object>
	{
		public:
			static const uint8_t space_id = protocol_ids;
			static const uint8_t type_id = gr_invite_object_type;

			account_id_type captain;
			account_id_type player;
			gr_team_id_type team;
	};

	struct by_id;
	struct by_player;

	typedef multi_index_container<
		gr_invite_object,
		indexed_by<
		ordered_unique< tag<by_id>, member< object, object_id_type, &object::id > >,
		ordered_non_unique<tag<by_player>, member<gr_invite_object, account_id_type, &gr_invite_object::player>>
	> gr_invite_multi_index_type;
	using gr_invite_index = generic_index<gr_invite_object, gr_invite_multi_index_type>;


	class gr_votes_object : public abstract_object<gr_votes_object>
	{
		public:
			static const uint8_t space_id = protocol_ids;
			static const uint8_t type_id = gr_votes_object_type;

		account_id_type player;
		share_type gr_iron_volume;
		share_type gr_bronze_volume;
		share_type gr_silver_volume;
		share_type gr_gold_volume;
		share_type gr_platinum_volume;
		share_type gr_diamond_volume;
		share_type gr_elite_volume;
		share_type gr_iron_reward;
		share_type gr_bronze_reward;
		share_type gr_silver_reward;
		share_type gr_gold_reward;
		share_type gr_platinum_reward;
		share_type gr_diamond_reward;
		share_type gr_elite_reward;
		share_type gr_master_reward;
	};

	struct by_id;
	struct by_player;

	typedef multi_index_container<
		gr_votes_object,
		indexed_by<
		ordered_unique< tag<by_id>, member< object, object_id_type, &object::id > >,
		ordered_non_unique<tag<by_player>, member<gr_votes_object, account_id_type, &gr_votes_object::player>>
	> gr_votes_multi_index_type;
	using gr_votes_index = generic_index<gr_votes_object, gr_votes_multi_index_type>;	

} } // graphene::chain

FC_REFLECT_DERIVED(graphene::chain::gr_team_object,
					(graphene::db::object),
					(name)
					(description)
					(logo)
					(captain)
					(players_num)
					(gr_interval_2_volume)
					(gr_interval_4_volume)
					(gr_interval_6_volume)
					(gr_interval_9_volume)
					(gr_interval_11_volume)
					(gr_interval_13_volume)
					(last_gr_rank)
)

FC_REFLECT_DERIVED(graphene::chain::gr_invite_object,
					(graphene::db::object),
					(captain)
					(player)
					(team)
)

FC_REFLECT_DERIVED(graphene::chain::gr_votes_object,
					(graphene::db::object),
					(player)
					(gr_iron_volume)
					(gr_bronze_volume)
					(gr_silver_volume)
					(gr_gold_volume)
					(gr_platinum_volume)
					(gr_diamond_volume)
					(gr_elite_volume)
					(gr_iron_reward)
					(gr_bronze_reward)
					(gr_silver_reward)
					(gr_gold_reward)
					(gr_platinum_reward)
					(gr_diamond_reward)
					(gr_elite_reward)
					(gr_master_reward)
)