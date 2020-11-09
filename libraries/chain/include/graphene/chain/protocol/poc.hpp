/*
 * Copyright (c) 2019 CrowdWiz., and contributors.
 *
 * The MIT License
 */

#pragma once
#include <graphene/chain/protocol/base.hpp>

namespace graphene { namespace chain { 

struct poc_vote_operation : public base_operation
	{
		struct fee_parameters_type { uint64_t fee = 0; };
		asset				fee;
		account_id_type		account;
		asset				poc3_vote;
		asset				poc6_vote;
		asset				poc12_vote;

		account_id_type		fee_payer()const { return account; }
		void				validate()const;
	};

struct poc_stak_operation : public base_operation
	{
		struct fee_parameters_type { uint64_t fee = 20 * GRAPHENE_BLOCKCHAIN_PRECISION; };
		asset				fee;
		account_id_type		account;
		asset				stak_amount;
		uint8_t				staking_type;

		account_id_type		fee_payer()const { return account; }
		void				validate()const;
	};

struct poc_staking_referal_operation : public base_operation //VITRUAL
	{
		struct fee_parameters_type { uint64_t fee = 0; };
		asset				fee;
		account_id_type		referrer;
		account_id_type		account;
		uint8_t				level;
		asset				reward;

		account_id_type		fee_payer()const { return referrer; }
		void				validate()const;
	};

struct exchange_silver_operation : public base_operation //VITRUAL
	{
		struct fee_parameters_type { uint64_t fee = 2 * GRAPHENE_BLOCKCHAIN_PRECISION; };
		asset				fee;
		account_id_type		account;
		asset				amount;

		account_id_type		fee_payer()const { return account; }
		void				validate()const;
	};


} } // graphene::chain

FC_REFLECT( graphene::chain::poc_vote_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::poc_vote_operation,
	(fee)
	(account)
	(poc3_vote)
	(poc6_vote)
	(poc12_vote)
)
FC_REFLECT( graphene::chain::poc_stak_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::poc_stak_operation,
	(fee)
	(account)
	(stak_amount)
	(staking_type)
)
FC_REFLECT( graphene::chain::poc_staking_referal_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::poc_staking_referal_operation,
	(fee)
	(referrer)
	(account)
	(level)
	(reward)
)
FC_REFLECT( graphene::chain::exchange_silver_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::exchange_silver_operation,
	(fee)
	(account)
	(amount)
)