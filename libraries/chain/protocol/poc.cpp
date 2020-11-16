/*
 * Copyright (c) 2019 CrowdWiz., and contributors.
 *
 * The MIT License
 */

#include <graphene/chain/protocol/poc.hpp>

namespace graphene { namespace chain {

void poc_vote_operation::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
		FC_ASSERT( poc3_vote.amount >= 0, "Your vote should not be negative" );
		FC_ASSERT( poc3_vote.asset_id == asset_id_type(), "Amount type should be CWD" );
		FC_ASSERT( poc6_vote.amount >= 0, "Your vote should not be negative" );
		FC_ASSERT( poc6_vote.asset_id == asset_id_type(), "Amount type should be CWD" );
		FC_ASSERT( poc12_vote.amount >= 0, "Your vote should not be negative" );
		FC_ASSERT( poc12_vote.asset_id == asset_id_type(), "Amount type should be CWD" );
	}

void poc_stak_operation::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
		FC_ASSERT( stak_amount.amount > 0, "Stak amount should not be negative" );
		FC_ASSERT( stak_amount.asset_id == asset_id_type(), "Amount type should be CWD" );
		FC_ASSERT( (staking_type == 3 || staking_type == 6 || staking_type == 12), "Staking month should be 3, 6 or 12" );
	}


void poc_staking_referal_operation::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
	}


void exchange_silver_operation::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
		FC_ASSERT( amount.amount > 0, "Amount should not be negative" );
		FC_ASSERT( amount.asset_id == asset_id_type(4), "Amount type should be SILVER" );
	}

} } // graphene::chain
