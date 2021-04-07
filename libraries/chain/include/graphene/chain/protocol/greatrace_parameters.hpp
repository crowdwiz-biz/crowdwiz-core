#pragma once
#include <graphene/chain/protocol/base.hpp>
#include <graphene/chain/protocol/types.hpp>

namespace graphene
{
	namespace chain
	{
		struct greatrace_chain_parameters
		{
			uint32_t	vote_duration 		= (24*60*60);
			uint8_t		min_team_status 	= 4;
			share_type	min_team_volume 	= (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(15000));
			uint16_t	min_votes 			= 50;
			uint8_t		min_vote_last_rank	= 7;
			uint16_t	interval_1 			= 7;
			uint16_t	interval_2 			= 21;
			uint16_t	interval_3 			= 7;
			uint16_t	interval_4 			= 21;
			uint16_t	interval_5 			= 7;
			uint16_t	interval_6 			= 21;
			uint16_t	interval_7 			= 7;
			uint16_t	interval_8 			= 7;
			uint16_t	interval_9 			= 21;
			uint16_t	interval_10 		= 7;
			uint16_t	interval_11 		= 21;
			uint16_t	interval_12 		= 7;
			uint16_t	interval_13 		= 21;
			uint16_t	interval_14 		= 7; 
		};
	} } // namespace graphene::chain

FC_REFLECT(graphene::chain::greatrace_chain_parameters, 
	(vote_duration)
	(min_team_status)
	(min_team_volume)
	(min_votes)
	(min_vote_last_rank)
	(interval_1)
	(interval_2)
	(interval_3)
	(interval_4)
	(interval_5)
	(interval_6)
	(interval_7)
	(interval_8)
	(interval_9)
	(interval_10)
	(interval_11)
	(interval_12)
	(interval_13)
	(interval_14)
)