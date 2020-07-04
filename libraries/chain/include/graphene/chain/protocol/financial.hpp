/*
 * Copyright (c) 2019 CrowdWiz., and contributors.
 *
 * The MIT License
 */

#pragma once
#include <graphene/chain/protocol/base.hpp>
#include <graphene/chain/protocol/memo.hpp>


namespace graphene { namespace chain { 


struct credit_system_get_operation : public base_operation
	{
		struct fee_parameters_type { uint64_t fee = 0; };
		asset				fee;
		account_id_type		debitor;
		asset				credit_amount;

		account_id_type		fee_payer()const { return debitor; }
		void				validate()const;
	};

struct credit_total_repay_operation : public base_operation
	{
		struct fee_parameters_type { uint64_t fee = 0; };
		asset				fee;
		account_id_type 	debitor;
		account_id_type		creditor;
		asset 				repay_amount;

		account_id_type		fee_payer()const { return debitor; }
		void				validate()const;
	};

struct credit_repay_operation : public base_operation
	{
		struct fee_parameters_type { uint64_t fee = 2 * GRAPHENE_BLOCKCHAIN_PRECISION; };
		asset				fee;
		account_id_type 	debitor;
		account_id_type		creditor;
		asset 				repay_amount;

		account_id_type		fee_payer()const { return creditor; }
		void				validate()const;
	};

struct credit_offer_create_operation : public base_operation
	{
		struct fee_parameters_type { uint64_t fee = 2 * GRAPHENE_BLOCKCHAIN_PRECISION; };
		asset				fee;
		share_type			min_income;
		account_id_type		creditor;
		asset				credit_amount;
		asset				repay_amount;

		account_id_type		fee_payer()const { return creditor; }
		void				validate()const;
	};

struct credit_offer_cancel_operation : public base_operation
	{
		struct fee_parameters_type { uint64_t fee = 0; };
		asset             		fee;
		account_id_type			creditor;
		credit_offer_id_type	credit_offer;

		account_id_type			fee_payer()const { return creditor; }
		void					validate()const;
	};

struct credit_offer_fill_operation : public base_operation
	{
		struct fee_parameters_type { uint64_t fee = 0; };
		asset             		fee;
		account_id_type			debitor;
		account_id_type			creditor;
		credit_offer_id_type	credit_offer;
		asset					credit_amount;

		account_id_type			fee_payer()const { return debitor; }
		void					validate()const;
	};

struct pledge_offer_give_create_operation : public base_operation
	{
		struct fee_parameters_type { uint64_t fee = 2 * GRAPHENE_BLOCKCHAIN_PRECISION; };
		asset				fee;
		account_id_type		creditor;
		asset				pledge_amount;
		asset				credit_amount;
		asset				repay_amount;		
		uint16_t			pledge_days; 

		account_id_type 	fee_payer()const { return creditor; }
		void            	validate()const;
	};

struct pledge_offer_take_create_operation : public base_operation
	{
		struct fee_parameters_type { uint64_t fee = 2 * GRAPHENE_BLOCKCHAIN_PRECISION; };
		asset				fee;
		account_id_type		debitor;
		asset				pledge_amount;
		asset				credit_amount;
		asset				repay_amount;		
		uint16_t			pledge_days; 

		account_id_type 	fee_payer()const { return debitor; }
		void            	validate()const;
	};

struct pledge_offer_cancel_operation : public base_operation
	{
		struct fee_parameters_type { uint64_t fee = 0; };
		asset					fee;
		account_id_type			creator;
		pledge_offer_id_type	pledge_offer;

		account_id_type 	fee_payer()const { return creator; }
		void            	validate()const;
	};

struct pledge_offer_fill_operation : public base_operation
	{
		struct fee_parameters_type { uint64_t fee = 0; };
		asset             		fee;
		account_id_type			account;
		account_id_type			debitor;
		account_id_type			creditor;
		asset					pledge_amount;
		asset					credit_amount;
		asset					repay_amount;		
		uint16_t				pledge_days; 
		pledge_offer_id_type	pledge_offer;

		account_id_type 	fee_payer()const { return account; }
		void            	validate()const;
	};

struct pledge_offer_repay_operation : public base_operation
	{
		struct fee_parameters_type { uint64_t fee = 0; };
		asset					fee;
		account_id_type			debitor;
		account_id_type			creditor;
		asset					repay_amount;
		asset					pledge_amount;
		pledge_offer_id_type	pledge_offer;

		account_id_type 	fee_payer()const { return debitor; }
		void            	validate()const;
	};

struct pledge_offer_auto_repay_operation : public base_operation
	{
		struct fee_parameters_type { uint64_t fee = 0; };
		asset					fee;
		account_id_type			debitor;
		account_id_type			creditor;
		asset					pledge_amount;
		asset					credit_amount;
		asset					repay_amount;		
		pledge_offer_id_type	pledge_offer;

		account_id_type 	fee_payer()const { return debitor; }
		void            	validate()const;
	};


} } // graphene::chain

FC_REFLECT( graphene::chain::credit_system_get_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::credit_system_get_operation,
	(fee)
	(debitor)
	(credit_amount)
)

FC_REFLECT( graphene::chain::credit_total_repay_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::credit_total_repay_operation,
	(fee)
	(debitor)
	(creditor)
	(repay_amount)
)

FC_REFLECT( graphene::chain::credit_repay_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::credit_repay_operation,
	(fee)
	(debitor)
	(creditor)
	(repay_amount)
)

FC_REFLECT( graphene::chain::credit_offer_create_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::credit_offer_create_operation,
	(fee)
	(min_income)
	(creditor)
	(credit_amount)
	(repay_amount)
)

FC_REFLECT( graphene::chain::credit_offer_cancel_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::credit_offer_cancel_operation,
	(fee)
	(creditor)
	(credit_offer)
)

FC_REFLECT( graphene::chain::credit_offer_fill_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::credit_offer_fill_operation,
	(fee)
	(debitor)
	(creditor)
	(credit_offer)
	(credit_amount)
)

FC_REFLECT( graphene::chain::pledge_offer_give_create_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::pledge_offer_give_create_operation,
	(fee)
	(creditor)
	(pledge_amount)
	(credit_amount)
	(repay_amount)
	(pledge_days)
)

FC_REFLECT( graphene::chain::pledge_offer_take_create_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::pledge_offer_take_create_operation,
	(fee)
	(debitor)
	(pledge_amount)
	(credit_amount)
	(repay_amount)
	(pledge_days)
)

FC_REFLECT( graphene::chain::pledge_offer_cancel_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::pledge_offer_cancel_operation,
	(fee)
	(creator)
	(pledge_offer)
)

FC_REFLECT( graphene::chain::pledge_offer_fill_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::pledge_offer_fill_operation,
	(fee)
	(account)
	(debitor)
	(creditor)
	(pledge_amount)
	(credit_amount)
	(repay_amount)
	(pledge_days)
	(pledge_offer)
)

FC_REFLECT( graphene::chain::pledge_offer_repay_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::pledge_offer_repay_operation,
	(fee)
	(debitor)
	(creditor)
	(repay_amount)
	(pledge_amount)
	(pledge_offer)
)

FC_REFLECT( graphene::chain::pledge_offer_auto_repay_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::pledge_offer_auto_repay_operation,
	(fee)
	(debitor)
	(creditor)
	(pledge_amount)
	(credit_amount)
	(repay_amount)
	(pledge_offer)
)