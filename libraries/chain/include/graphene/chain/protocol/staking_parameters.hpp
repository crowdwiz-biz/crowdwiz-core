#pragma once
#include <graphene/chain/protocol/base.hpp>
#include <graphene/chain/protocol/types.hpp>

namespace graphene
{
	namespace chain
	{
		struct staking_chain_parameters
		{
			uint16_t	poc_ref_01					= (5*GRAPHENE_1_PERCENT);
			uint16_t	poc_ref_02					= (2*GRAPHENE_1_PERCENT);
			uint16_t	poc_ref_03					= (1*GRAPHENE_1_PERCENT);
			uint16_t	poc_ref_04					= (1*GRAPHENE_1_PERCENT);
			uint16_t	poc_ref_05					= (1*GRAPHENE_1_PERCENT);
			uint16_t	poc_ref_06					= (1*GRAPHENE_1_PERCENT);
			uint16_t	poc_ref_07					= (1*GRAPHENE_1_PERCENT);
			uint16_t	poc_ref_08					= (1*GRAPHENE_1_PERCENT);
			uint16_t	poc_gcwd					= (5*GRAPHENE_1_PERCENT);
			uint8_t		poc_status_levels_00		= 1;
			uint8_t		poc_status_levels_01		= 3;
			uint8_t		poc_status_levels_02		= 5;
			uint8_t		poc_status_levels_03		= 7;
			uint8_t		poc_status_levels_04		= 8;		
			uint16_t	poc_status_denominator_00	= (20*GRAPHENE_1_PERCENT);
			uint16_t	poc_status_denominator_01	= (80*GRAPHENE_1_PERCENT);
			uint16_t	poc_status_denominator_02	= (100*GRAPHENE_1_PERCENT);
			uint16_t	poc_status_denominator_03	= (100*GRAPHENE_1_PERCENT);
			uint16_t	poc_status_denominator_04	= (100*GRAPHENE_1_PERCENT);
			uint32_t	poc_vote_duration			= (24*60*60);
			uint16_t	poc_vote_interval_days		= 90;
			uint16_t	poc_min_votes				= 160;
			uint16_t	poc_filter_percent			= (15*GRAPHENE_1_PERCENT);
			share_type	poc3_min_amount				= (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(250)); 
			share_type	poc6_min_amount				= (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(250)); 
			share_type	poc12_min_amount			= (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(1000)); 
		};
	} } // namespace graphene::chain

FC_REFLECT(graphene::chain::staking_chain_parameters, 
	(poc_ref_01)
	(poc_ref_02)
	(poc_ref_03)
	(poc_ref_04)
	(poc_ref_05)
	(poc_ref_06)
	(poc_ref_07)
	(poc_ref_08)
	(poc_status_levels_00)
	(poc_status_levels_01)
	(poc_status_levels_02)
	(poc_status_levels_03)
	(poc_status_levels_04)
	(poc_status_denominator_00)
	(poc_status_denominator_01)
	(poc_status_denominator_02)
	(poc_status_denominator_03)
	(poc_status_denominator_04)
	(poc_vote_duration)
	(poc_vote_interval_days)
	(poc_min_votes)
	(poc_filter_percent)
)