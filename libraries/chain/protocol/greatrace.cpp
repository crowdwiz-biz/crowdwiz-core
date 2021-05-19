/*
 * Copyright (c) 2021 CrowdWiz., and contributors.
 *
 * The MIT License
 */
#include <graphene/chain/protocol/greatrace.hpp>

namespace graphene { namespace chain {
    share_type cut_fee_greatrace(share_type a, uint16_t p)
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
	void gr_team_create_operation::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
		FC_ASSERT( fc::raw::pack_size(name)<65 );
		FC_ASSERT( fc::raw::pack_size(description)<257 );
		FC_ASSERT( fc::raw::pack_size(logo)<257 );
	}
	void gr_team_delete_operation::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
	}
	void gr_invite_send_operation::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
		FC_ASSERT( captain != player );
	}
	void gr_invite_accept_operation::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
	}
	void gr_player_remove_operation::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
		FC_ASSERT( captain != player );
	}
	void gr_team_leave_operation::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
		FC_ASSERT( captain != player );
	}
	void gr_vote_operation::validate()const
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
	void gr_assign_rank_operation::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
	}
	void gr_pay_rank_reward_operation::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
	}
	void gr_pay_top_reward_operation::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
	}
	void gr_apostolos_operation::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
	}

	void gr_range_bet_operation::validate()const
	{
		FC_ASSERT( upper_rank > 0, "Rank should not be negative" );
		FC_ASSERT( upper_rank <= 100, "Rank should be lower than 100" );
		FC_ASSERT( lower_rank > 0, "Rank should not be negative" );
		FC_ASSERT( lower_rank <= 100, "Rank should be lower than 100" );
		FC_ASSERT( lower_rank <= upper_rank, "Wrong rank order" );
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
	}

    share_type gr_range_bet_operation::calculate_fee(const fee_parameters_type& k) const
    {
		share_type core_fee_required;
		core_fee_required = cut_fee_greatrace(bet.amount, k.gr_range_bet_fee_percent);
		return core_fee_required; 
    }

	void gr_team_bet_operation::validate()const
	{
		FC_ASSERT( (winner == team1 || winner == team2), "Winner should be team1 or team2" );
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
	}

    share_type gr_team_bet_operation::calculate_fee(const fee_parameters_type& k) const
    {
		share_type core_fee_required;
		core_fee_required = cut_fee_greatrace(bet.amount, k.gr_team_bet_fee_percent);
		return core_fee_required; 
    }
	void gr_range_bet_win_operation::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
	}
	void gr_range_bet_loose_operation::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
	}
	void gr_team_bet_win_operation::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
	}
	void gr_team_bet_loose_operation::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
	}
} } // graphene::chain