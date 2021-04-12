/*
 * Copyright (c) 2019 CrowdWiz., and contributors.
 *
 * The MIT License
 */

#pragma once
#include <graphene/chain/protocol/base.hpp>

namespace graphene { namespace chain { 
	struct gr_team_create_operaton : public base_operation {
		struct fee_parameters_type { uint64_t fee = 200 * GRAPHENE_BLOCKCHAIN_PRECISION; };
		
		account_id_type		captain;
		string 				name;
		string 				description;
		string 				logo;

		account_id_type		fee_payer()const { return captain; }
		void				validate()const;
	};

	struct gr_team_delete_operaton : public base_operation {
		struct fee_parameters_type { uint64_t fee = 2 * GRAPHENE_BLOCKCHAIN_PRECISION; };

		account_id_type 	captain;
		gr_team_id_type 	team;

		account_id_type		fee_payer()const { return captain; }
		void				validate()const;
	};

	struct gr_invite_send_operaton : public base_operation {
		struct fee_parameters_type { uint64_t fee = 20 * GRAPHENE_BLOCKCHAIN_PRECISION; };

		account_id_type		captain;
		account_id_type		player;
		gr_team_id_type		team;

		account_id_type		fee_payer()const { return captain; }
		void				validate()const;
	};

	struct gr_invite_accept_operaton : public base_operation {
		struct fee_parameters_type { uint64_t fee = 0; };
		
		account_id_type		captain;
		account_id_type		player;
		gr_team_id_type		team;
		gr_invite_id_type	invite;

		account_id_type		fee_payer()const { return player; }
		void				validate()const;
	};

	struct gr_player_remove_operaton : public base_operation {
		struct fee_parameters_type { uint64_t fee = 2 * GRAPHENE_BLOCKCHAIN_PRECISION; };
		
		account_id_type		captain;
		account_id_type		player;
		gr_team_id_type		team;
		
		account_id_type		fee_payer()const { return captain; }
		void				validate()const;
	};

	struct gr_team_leave_operaton : public base_operation {
		struct fee_parameters_type { uint64_t fee = 2 * GRAPHENE_BLOCKCHAIN_PRECISION; };

		account_id_type		captain;
		account_id_type		player;
		gr_team_id_type		team;

		account_id_type		fee_payer()const { return player; }
		void				validate()const;
	};

	struct gr_vote_operaton : public base_operation {
		struct fee_parameters_type { uint64_t fee = 2 * GRAPHENE_BLOCKCHAIN_PRECISION; };
		
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

	struct gr_assign_rank_operaton : public base_operation { // VIRTUAL
		struct fee_parameters_type { uint64_t fee = 0; };
		
		account_id_type		player;
		gr_team_id_type		team;
		uint8_t 			rank;

		account_id_type		fee_payer()const { return player; }
		void				validate()const;
	};

	struct gr_pay_rank_reward_operaton : public base_operation { // VIRTUAL
		struct fee_parameters_type { uint64_t fee = 0; };

		account_id_type		captain;
		gr_team_id_type		team;
		asset 				amount;
		uint8_t 			rank;
		
		account_id_type		fee_payer()const { return captain; }
		void				validate()const;
	};

	struct gr_pay_top_reward_operaton : public base_operation { // VIRTUAL
		struct fee_parameters_type { uint64_t fee = 0; };

		account_id_type		captain;
		gr_team_id_type		team;
		asset 				amount;
		uint8_t 			interval;
		
		account_id_type		fee_payer()const { return captain; }
		void				validate()const;
	};

	struct gr_apostolos_operaton : public base_operation { // VIRTUAL
		struct fee_parameters_type { uint64_t fee = 0; };

		account_id_type		player;
		gr_team_id_type		team;

		account_id_type		fee_payer()const { return player; }
		void				validate()const;
	};
} } // graphene::chain

FC_REFLECT( graphene::chain::gr_team_create_operaton::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::gr_team_create_operaton,
	(captain)
	(name)
	(description)
	(logo)
)

FC_REFLECT( graphene::chain::gr_team_delete_operaton::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::gr_team_delete_operaton,
	(captain)
	(team)
)

FC_REFLECT( graphene::chain::gr_invite_send_operaton::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::gr_invite_send_operaton,
	(captain)
	(player)
	(team)
)

FC_REFLECT( graphene::chain::gr_invite_accept_operaton::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::gr_invite_accept_operaton,
	(captain)
	(player)
	(team)
	(invite)
)

FC_REFLECT( graphene::chain::gr_player_remove_operaton::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::gr_player_remove_operaton,
	(captain)
	(player)
	(team)
)

FC_REFLECT( graphene::chain::gr_team_leave_operaton::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::gr_team_leave_operaton,
	(captain)
	(player)
	(team)
)

FC_REFLECT( graphene::chain::gr_vote_operaton::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::gr_vote_operaton,
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

FC_REFLECT( graphene::chain::gr_assign_rank_operaton::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::gr_assign_rank_operaton,
	(player)
	(team)
	(rank)
)

FC_REFLECT( graphene::chain::gr_pay_rank_reward_operaton::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::gr_pay_rank_reward_operaton,
	(captain)
	(team)
	(amount)
	(rank)
)

FC_REFLECT( graphene::chain::gr_pay_top_reward_operaton::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::gr_pay_top_reward_operaton,
	(captain)
	(team)
	(amount)
	(interval)
)

FC_REFLECT( graphene::chain::gr_apostolos_operaton::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::gr_apostolos_operaton,
	(player)
	(team)
)