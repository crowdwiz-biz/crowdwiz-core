/*
 * Copyright (c) 2019 CrowdWiz., and contributors.
 *
 * The MIT License
 */

#pragma once
#include <graphene/chain/protocol/base.hpp>

namespace graphene { namespace chain { 
	struct gr_team_create_operation : public base_operation {
		struct fee_parameters_type { uint64_t fee = 200 * GRAPHENE_BLOCKCHAIN_PRECISION; };

		asset				fee;
		account_id_type		captain;
		string 				name;
		string 				description;
		string 				logo;

		account_id_type		fee_payer()const { return captain; }
		void				validate()const;
	};

	struct gr_team_delete_operation : public base_operation {
		struct fee_parameters_type { uint64_t fee = 2 * GRAPHENE_BLOCKCHAIN_PRECISION; };

		asset				fee;

		account_id_type 	captain;
		gr_team_id_type 	team;

		account_id_type		fee_payer()const { return captain; }
		void				validate()const;
	};

	struct gr_invite_send_operation : public base_operation {
		struct fee_parameters_type { uint64_t fee = 20 * GRAPHENE_BLOCKCHAIN_PRECISION; };

		asset				fee;

		account_id_type		captain;
		account_id_type		player;
		gr_team_id_type		team;

		account_id_type		fee_payer()const { return captain; }
		void				validate()const;
	};

	struct gr_invite_accept_operation : public base_operation {
		struct fee_parameters_type { uint64_t fee = 0; };

		asset				fee;

		account_id_type		captain;
		account_id_type		player;
		gr_team_id_type		team;
		gr_invite_id_type	invite;

		account_id_type		fee_payer()const { return player; }
		void				validate()const;
	};

	struct gr_player_remove_operation : public base_operation {
		struct fee_parameters_type { uint64_t fee = 2 * GRAPHENE_BLOCKCHAIN_PRECISION; };

		asset				fee;

		account_id_type		captain;
		account_id_type		player;
		gr_team_id_type		team;
		
		account_id_type		fee_payer()const { return captain; }
		void				validate()const;
	};

	struct gr_team_leave_operation : public base_operation {
		struct fee_parameters_type { uint64_t fee = 2 * GRAPHENE_BLOCKCHAIN_PRECISION; };

		asset				fee;

		account_id_type		captain;
		account_id_type		player;
		gr_team_id_type		team;

		account_id_type		fee_payer()const { return player; }
		void				validate()const;
	};

	struct gr_vote_operation : public base_operation {
		struct fee_parameters_type { uint64_t fee = 2 * GRAPHENE_BLOCKCHAIN_PRECISION; };

		asset				fee;

		account_id_type		player;
		share_type	        gr_iron_volume;
		share_type	        gr_bronze_volume;
		share_type	        gr_silver_volume;
		share_type	        gr_gold_volume;
		share_type	        gr_platinum_volume;
		share_type	        gr_diamond_volume;
		share_type	        gr_elite_volume;
		share_type	        gr_iron_reward;
		share_type	        gr_bronze_reward;
		share_type	        gr_silver_reward;
		share_type	        gr_gold_reward;
		share_type	        gr_platinum_reward;
		share_type	        gr_diamond_reward;
		share_type	        gr_elite_reward;
		share_type	        gr_master_reward;

		account_id_type		fee_payer()const { return player; }
		void				validate()const;
	};

	struct gr_assign_rank_operation : public base_operation { // VIRTUAL
		struct fee_parameters_type { uint64_t fee = 0; };

		asset				fee;

		account_id_type		player;
		gr_team_id_type		team;
		uint8_t 			rank;

		account_id_type		fee_payer()const { return player; }
		void				validate()const;
	};

	struct gr_pay_rank_reward_operation : public base_operation { // VIRTUAL
		struct fee_parameters_type { uint64_t fee = 0; };

		asset				fee;

		account_id_type		captain;
		gr_team_id_type		team;
		asset 				amount;
		uint8_t 			rank;
		
		account_id_type		fee_payer()const { return captain; }
		void				validate()const;
	};

	struct gr_pay_top_reward_operation : public base_operation { // VIRTUAL
		struct fee_parameters_type { uint64_t fee = 0; };

		asset				fee;

		account_id_type		captain;
		gr_team_id_type		team;
		asset 				amount;
		uint8_t 			interval;
		
		account_id_type		fee_payer()const { return captain; }
		void				validate()const;
	};

	struct gr_apostolos_operation : public base_operation { // VIRTUAL
		struct fee_parameters_type { uint64_t fee = 0; };

		asset				fee;

		account_id_type		player;
		gr_team_id_type		team;

		account_id_type		fee_payer()const { return player; }
		void				validate()const;
	};

   struct gr_range_bet_operation : public base_operation
   {
      struct fee_parameters_type
      {
         share_type fee = 0;
         uint16_t gr_range_bet_fee_percent = 10*GRAPHENE_1_PERCENT;
      };

		asset				fee;

		gr_team_id_type		team;
		uint8_t 			lower_rank;
		uint8_t 			upper_rank;
		bool 				result;
		account_id_type		bettor;
		asset               bet;
      
      account_id_type         fee_payer()const { return bettor; }
      void                    validate()const;  
      share_type              calculate_fee(const fee_parameters_type& k)const;   
   };

   struct gr_team_bet_operation : public base_operation
   {
      struct fee_parameters_type
      {
         share_type fee = 0;
         uint16_t gr_team_bet_fee_percent = 10*GRAPHENE_1_PERCENT;
      };

		asset				fee;

		gr_team_id_type		team1;
		gr_team_id_type		team2;
		gr_team_id_type		winner;
		account_id_type		bettor;
		asset               bet;
      
      account_id_type         fee_payer()const { return bettor; }
      void                    validate()const;  
      share_type              calculate_fee(const fee_parameters_type& k)const;   
   };

	struct gr_range_bet_win_operation : public base_operation { // VIRTUAL
		struct fee_parameters_type { uint64_t fee = 0; };

		asset					fee;
		gr_range_bet_id_type	gr_range_bet;
		gr_team_id_type 		team;
		uint8_t 				lower_rank;
		uint8_t 				upper_rank;
		uint64_t				result;
		share_type				total_bets;
		share_type				total_wins;
		share_type				bettor_part;
		share_type				reward;
		account_id_type			bettor;

		account_id_type		fee_payer()const { return bettor; }
		void				validate()const;
	};

	struct gr_range_bet_loose_operation : public base_operation { // VIRTUAL
		struct fee_parameters_type { uint64_t fee = 0; };

		asset					fee;
		gr_range_bet_id_type	gr_range_bet;
		uint64_t				result;
		gr_team_id_type 		team;
		uint8_t 				lower_rank;
		uint8_t 				upper_rank;
		account_id_type			bettor;

		account_id_type		fee_payer()const { return bettor; }
		void				validate()const;
	};

	struct gr_team_bet_win_operation : public base_operation { // VIRTUAL
		struct fee_parameters_type { uint64_t fee = 0; };

		asset					fee;
		gr_team_bet_id_type		gr_team_bet;
		gr_team_id_type 		team1;
		gr_team_id_type 		team2;
		gr_team_id_type 		winner;
		share_type				total_bets;
		share_type				total_wins;
		share_type				bettor_part;
		share_type				reward;
		account_id_type			bettor;

		account_id_type		fee_payer()const { return bettor; }
		void				validate()const;
	};

	struct gr_team_bet_loose_operation : public base_operation { // VIRTUAL
		struct fee_parameters_type { uint64_t fee = 0; };

		asset					fee;
		gr_team_bet_id_type		gr_team_bet;
		gr_team_id_type 		team1;
		gr_team_id_type 		team2;
		gr_team_id_type 		winner;
		account_id_type			bettor;

		account_id_type		fee_payer()const { return bettor; }
		void				validate()const;
	};

} } // graphene::chain

FC_REFLECT( graphene::chain::gr_team_create_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::gr_team_create_operation,
	(fee)
	(captain)
	(name)
	(description)
	(logo)
)

FC_REFLECT( graphene::chain::gr_team_delete_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::gr_team_delete_operation,
	(fee)
	(captain)
	(team)
)

FC_REFLECT( graphene::chain::gr_invite_send_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::gr_invite_send_operation,
	(fee)
	(captain)
	(player)
	(team)
)

FC_REFLECT( graphene::chain::gr_invite_accept_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::gr_invite_accept_operation,
	(fee)
	(captain)
	(player)
	(team)
	(invite)
)

FC_REFLECT( graphene::chain::gr_player_remove_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::gr_player_remove_operation,
	(fee)
	(captain)
	(player)
	(team)
)

FC_REFLECT( graphene::chain::gr_team_leave_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::gr_team_leave_operation,
	(fee)
	(captain)
	(player)
	(team)
)

FC_REFLECT( graphene::chain::gr_vote_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::gr_vote_operation,
	(fee)
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

FC_REFLECT( graphene::chain::gr_assign_rank_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::gr_assign_rank_operation,
	(fee)
	(player)
	(team)
	(rank)
)

FC_REFLECT( graphene::chain::gr_pay_rank_reward_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::gr_pay_rank_reward_operation,
	(fee)
	(captain)
	(team)
	(amount)
	(rank)
)

FC_REFLECT( graphene::chain::gr_pay_top_reward_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::gr_pay_top_reward_operation,
	(fee)
	(captain)
	(team)
	(amount)
	(interval)
)

FC_REFLECT( graphene::chain::gr_apostolos_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::gr_apostolos_operation,
	(fee)
	(player)
	(team)
)

FC_REFLECT( graphene::chain::gr_range_bet_operation::fee_parameters_type, (fee)(gr_range_bet_fee_percent) )
FC_REFLECT( graphene::chain::gr_range_bet_operation,
	(fee)
	(team)
	(lower_rank)
	(upper_rank)
	(result)
	(bettor)
	(bet)
)

FC_REFLECT( graphene::chain::gr_team_bet_operation::fee_parameters_type, (fee)(gr_team_bet_fee_percent) )
FC_REFLECT( graphene::chain::gr_team_bet_operation,
	(fee)
	(team1)
	(team2)
	(winner)
	(bettor)
	(bet)
)

FC_REFLECT( graphene::chain::gr_range_bet_win_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::gr_range_bet_win_operation,
	(fee)
	(gr_range_bet)
	(result)
	(team)
	(lower_rank)
	(upper_rank)
	(total_bets)
	(total_wins)
	(bettor_part)
	(reward)
	(bettor)
)

FC_REFLECT( graphene::chain::gr_range_bet_loose_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::gr_range_bet_loose_operation,
	(fee)
	(gr_range_bet)
	(result)
	(team)
	(lower_rank)
	(upper_rank)
	(bettor)
)

FC_REFLECT( graphene::chain::gr_team_bet_win_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::gr_team_bet_win_operation,
	(fee)
	(gr_team_bet)
	(team1)
	(team2)
	(winner)
	(total_bets)
	(total_wins)
	(bettor_part)
	(reward)
	(bettor)
)
FC_REFLECT( graphene::chain::gr_team_bet_loose_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::gr_team_bet_loose_operation,
	(fee)
	(gr_team_bet)
	(team1)
	(team2)
	(winner)
	(bettor)
)