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
			flat_set<account_id_type> players;
			share_type gr_interval_2_volume = 0;
			share_type gr_interval_4_volume = 0;
			share_type gr_interval_6_volume = 0;
			share_type gr_interval_9_volume = 0;
			share_type gr_interval_11_volume = 0;
			share_type gr_interval_13_volume = 0;
			uint8_t last_gr_rank = 0;


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
	struct by_name;
	struct by_captain;

	typedef multi_index_container<
		gr_team_object,
		indexed_by<
		ordered_unique< tag<by_id>, member< object, object_id_type, &object::id > >,
		ordered_unique<tag<by_name>, member<gr_team_object, string, &gr_team_object::name>>,
		ordered_unique<tag<by_captain>, member<gr_team_object, account_id_type, &gr_team_object::captain>>,
		ordered_non_unique<tag<by_gr_interval_2_volume>, member<gr_team_object, share_type, &gr_team_object::gr_interval_2_volume>>,
		ordered_non_unique<tag<by_gr_interval_4_volume>, member<gr_team_object, share_type, &gr_team_object::gr_interval_4_volume>>,
		ordered_non_unique<tag<by_gr_interval_6_volume>, member<gr_team_object, share_type, &gr_team_object::gr_interval_6_volume>>,
		ordered_non_unique<tag<by_gr_interval_9_volume>, member<gr_team_object, share_type, &gr_team_object::gr_interval_9_volume>>,
		ordered_non_unique<tag<by_gr_interval_11_volume>, member<gr_team_object, share_type, &gr_team_object::gr_interval_11_volume>>,
		ordered_non_unique<tag<by_gr_interval_13_volume>, member<gr_team_object, share_type, &gr_team_object::gr_interval_13_volume>>,
		ordered_non_unique<tag<by_last_gr_rank>, member<gr_team_object, uint8_t, &gr_team_object::last_gr_rank>>,
		ordered_non_unique<tag<by_total_first_half_volume>, const_mem_fun<gr_team_object, share_type, &gr_team_object::first_half_volume>>,
		ordered_non_unique<tag<by_total_second_half_volume>, const_mem_fun<gr_team_object, share_type, &gr_team_object::second_half_volume>>,
		ordered_non_unique<tag<by_total_volume>, const_mem_fun<gr_team_object, share_type, &gr_team_object::total_volume>>
	>> gr_team_multi_index_type;
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
	>> gr_invite_multi_index_type;
	using gr_invite_index = generic_index<gr_invite_object, gr_invite_multi_index_type>;


	class gr_votes_object : public abstract_object<gr_votes_object>
	{
		public:
			static const uint8_t space_id = protocol_ids;
			static const uint8_t type_id = gr_votes_object_type;

		account_id_type player;
		share_type gr_iron_volume = 0;
		share_type gr_bronze_volume = 0;
		share_type gr_silver_volume = 0;
		share_type gr_gold_volume = 0;
		share_type gr_platinum_volume = 0;
		share_type gr_diamond_volume = 0;
		share_type gr_elite_volume = 0;
		share_type gr_iron_reward = 0;
		share_type gr_bronze_reward = 0;
		share_type gr_silver_reward = 0;
		share_type gr_gold_reward = 0;
		share_type gr_platinum_reward = 0;
		share_type gr_diamond_reward = 0;
		share_type gr_elite_reward = 0;
		share_type gr_master_reward = 0;
	};

	struct by_id;
	struct by_player;

	typedef multi_index_container<
		gr_votes_object,
		indexed_by<
		ordered_unique< tag<by_id>, member< object, object_id_type, &object::id > >,
		ordered_non_unique<tag<by_player>, member<gr_votes_object, account_id_type, &gr_votes_object::player>>
	>> gr_votes_multi_index_type;
	using gr_votes_index = generic_index<gr_votes_object, gr_votes_multi_index_type>;	

	class gr_range_bet_object : public abstract_object<gr_range_bet_object>
	{
		public:
			static const uint8_t space_id = protocol_ids;
			static const uint8_t type_id = gr_range_bet_object_type;

			gr_team_id_type team;
			uint8_t lower_rank;
			uint8_t upper_rank;
			share_type total_prize = 0;
			share_type total_true_bets = 0;
			share_type total_false_bets = 0;

			map<account_id_type, share_type> true_bets;
			map<account_id_type, share_type> false_bets;
	
	};

	struct by_id;
	struct by_bet;

	typedef multi_index_container<
		gr_range_bet_object,
		indexed_by<
		ordered_unique< tag<by_id>, member< object, object_id_type, &object::id > >,
		ordered_unique< tag<by_bet>,
			composite_key< gr_range_bet_object,
			member< gr_range_bet_object, gr_team_id_type, &gr_range_bet_object::team>,
			member< gr_range_bet_object, uint8_t, &gr_range_bet_object::lower_rank>,
			member< gr_range_bet_object, uint8_t, &gr_range_bet_object::upper_rank>
			>
		>
	>> gr_range_bet_multi_index_type;
	using gr_range_bet_index = generic_index<gr_range_bet_object, gr_range_bet_multi_index_type>;

	class gr_team_bet_object : public abstract_object<gr_team_bet_object>
	{
		public:
			static const uint8_t space_id = protocol_ids;
			static const uint8_t type_id = gr_team_bet_object_type;

			gr_team_id_type team1;
			gr_team_id_type team2;
			share_type total_prize = 0;
			share_type total_team1_bets = 0;
			share_type total_team2_bets = 0;
			map<account_id_type, share_type> team1_bets;
			map<account_id_type, share_type> team2_bets;
	};

	struct by_id;
	struct by_bet;

	typedef multi_index_container<
		gr_team_bet_object,
		indexed_by<
		ordered_unique< tag<by_id>, member< object, object_id_type, &object::id > >,
		ordered_unique< tag<by_bet>,
			composite_key< gr_team_bet_object,
			member< gr_team_bet_object, gr_team_id_type, &gr_team_bet_object::team1>,
			member< gr_team_bet_object, gr_team_id_type, &gr_team_bet_object::team2>
			>
		>
	>> gr_team_bet_multi_index_type;
	using gr_team_bet_index = generic_index<gr_team_bet_object, gr_team_bet_multi_index_type>;
} } // graphene::chain

FC_REFLECT_DERIVED(graphene::chain::gr_team_object,
					(graphene::db::object),
					(name)
					(description)
					(logo)
					(captain)
					(players)
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

FC_REFLECT_DERIVED(graphene::chain::gr_range_bet_object,
					(graphene::db::object),
					(team)
					(lower_rank)
					(upper_rank)
					(total_prize)
					(total_true_bets)
					(total_false_bets)
					(true_bets)
					(false_bets)
)

FC_REFLECT_DERIVED(graphene::chain::gr_team_bet_object,
					(graphene::db::object),
					(team1)
					(team2)
					(total_prize)
					(total_team1_bets)
					(total_team2_bets)
					(team1_bets)
					(team2_bets)
)