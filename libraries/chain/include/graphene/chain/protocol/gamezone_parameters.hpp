#pragma once
#include <graphene/chain/protocol/base.hpp>
#include <graphene/chain/protocol/types.hpp>

namespace graphene
{
	namespace chain
	{
		struct gamezone_chain_parameters
		{
			uint32_t matrix_lasts_blocks = uint32_t(177);
			uint32_t matrix_idle_blocks = uint32_t(16671);

			uint8_t matrix_level_1_cells = uint8_t(2);
			uint8_t matrix_level_2_cells = uint8_t(2);
			uint8_t matrix_level_3_cells = uint8_t(2);
			uint8_t matrix_level_4_cells = uint8_t(2);
			uint8_t matrix_level_5_cells = uint8_t(2);
			uint8_t matrix_level_6_cells = uint8_t(2);
			uint8_t matrix_level_7_cells = uint8_t(2);
			uint8_t matrix_level_8_cells = uint8_t(2);

			share_type matrix_level_1_price = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(9));
			share_type matrix_level_2_price = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(16));
			share_type matrix_level_3_price = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(29));
			share_type matrix_level_4_price = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(52));
			share_type matrix_level_5_price = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(94));
			share_type matrix_level_6_price = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(170));
			share_type matrix_level_7_price = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(309));
			share_type matrix_level_8_price = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(555));
			share_type matrix_level_1_prize = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(18));
			share_type matrix_level_2_prize = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(32));
			share_type matrix_level_3_prize = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(58));
			share_type matrix_level_4_prize = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(104));
			share_type matrix_level_5_prize = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(188));
			share_type matrix_level_6_prize = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(340));
			share_type matrix_level_7_prize = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(618));
			share_type matrix_level_8_prize = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(1110));

			uint32_t lottery_goods_total_participants = uint32_t(5000);
			uint32_t lottery_goods_expiration = uint32_t(2592000);

			share_type flipcoin_min_bet_amount = int64_t(1000);
		};
	} } // namespace graphene::chain
FC_REFLECT(graphene::chain::gamezone_chain_parameters, 
	(matrix_lasts_blocks)
	(matrix_idle_blocks)
	(matrix_level_1_cells)
	(matrix_level_2_cells)
	(matrix_level_3_cells)
	(matrix_level_4_cells)
	(matrix_level_5_cells)
	(matrix_level_6_cells)
	(matrix_level_7_cells)
	(matrix_level_8_cells)
	(matrix_level_1_price)
	(matrix_level_2_price)
	(matrix_level_3_price)
	(matrix_level_4_price)
	(matrix_level_5_price)
	(matrix_level_6_price)
	(matrix_level_7_price)
	(matrix_level_8_price)
	(matrix_level_1_prize)
	(matrix_level_2_prize)
	(matrix_level_3_prize)
	(matrix_level_4_prize)
	(matrix_level_5_prize)
	(matrix_level_6_prize)
	(matrix_level_7_prize)
	(matrix_level_8_prize)
	(lottery_goods_total_participants)
	(lottery_goods_expiration)
	(flipcoin_min_bet_amount)
)