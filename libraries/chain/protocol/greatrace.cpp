/*
 * Copyright (c) 2021 CrowdWiz., and contributors.
 *
 * The MIT License
 */
#include <graphene/chain/protocol/greatrace.hpp>

namespace graphene { namespace chain {

	void gr_team_create_operaton::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
		FC_ASSERT( fc::raw::pack_size(name)<65 );
		FC_ASSERT( fc::raw::pack_size(description)<257 );
		FC_ASSERT( fc::raw::pack_size(logo)<257 );
	}
	void gr_team_delete_operaton::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
	}
	void gr_invite_send_operaton::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
		FC_ASSERT( captain != player );
	}
	void gr_invite_accept_operaton::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
	}
	void gr_player_remove_operaton::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
		FC_ASSERT( captain != player );
	}
	void gr_team_leave_operaton::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
		FC_ASSERT( captain != player );
	}
	void gr_vote_operaton::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
		FC_ASSERT( gr_iron_volume >= 0, "Volume amount should not be negative" );
		FC_ASSERT( gr_bronze_volume >= 0, "Volume amount should not be negative" );
		FC_ASSERT( gr_silver_volume >= 0, "Volume amount should not be negative" );
		FC_ASSERT( gr_gold_volume >= 0, "Volume amount should not be negative" );
		FC_ASSERT( gr_platinum_volume >= 0, "Volume amount should not be negative" );
		FC_ASSERT( gr_diamond_volume >= 0, "Volume amount should not be negative" );
		FC_ASSERT( gr_elite_volume >= 0, "Volume amount should not be negative" );
		FC_ASSERT( gr_iron_reward >= 0, "Reward amount should not be negative" );
		FC_ASSERT( gr_bronze_reward >= 0, "Reward amount should not be negative" );
		FC_ASSERT( gr_silver_reward >= 0, "Reward amount should not be negative" );
		FC_ASSERT( gr_gold_reward >= 0, "Reward amount should not be negative" );
		FC_ASSERT( gr_platinum_reward >= 0, "Reward amount should not be negative" );
		FC_ASSERT( gr_diamond_reward >= 0, "Reward amount should not be negative" );
		FC_ASSERT( gr_elite_reward >= 0, "Reward amount should not be negative" );
		FC_ASSERT( gr_master_reward >= 0, "Reward amount should not be negative" );
	}
	void gr_assign_rank_operaton::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
	}
	void gr_pay_rank_reward_operaton::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
	}
	void gr_pay_top_reward_operaton::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
	}
	void gr_apostolos_operaton::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
	}

} } // graphene::chain