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

struct buy_gcwd_operation : public base_operation //VITRUAL
	{
		struct fee_parameters_type { uint64_t fee = 0; };
		asset				fee;
		account_id_type		account;
		asset				amount;

		account_id_type		fee_payer()const { return account; }
		void				validate()const;
	};

struct approved_transfer_create_operation : public base_operation
	{
		struct fee_parameters_type { uint64_t fee = 5 * GRAPHENE_BLOCKCHAIN_PRECISION; };
		asset				fee;
		account_id_type 	from;
		account_id_type		to;
		account_id_type		arbitr;
		time_point_sec		expiration;
		asset 				amount;

		account_id_type		fee_payer()const { return from; }
		void				validate()const;
	};

struct approved_transfer_approve_operation : public base_operation
	{
		struct fee_parameters_type { uint64_t fee = 0; };
		asset						fee;
		account_id_type 			from;
		account_id_type				to;
		account_id_type				arbitr;
		asset 						amount;
		approved_transfer_id_type 	ato;

		account_id_type		fee_payer()const { return from; }
		void				validate()const;
	};

struct approved_transfer_cancel_operation : public base_operation
	{
		struct fee_parameters_type { uint64_t fee = 0; };
		asset						fee;
		account_id_type 			from;
		account_id_type				to;
		account_id_type				arbitr;
		asset 						amount;
		approved_transfer_id_type 	ato;

		account_id_type		fee_payer()const { return from; }
		void				validate()const;
	};

struct approved_transfer_open_dispute_operation : public base_operation
	{
		struct fee_parameters_type { uint64_t fee = 0; };
		asset						fee;
		account_id_type 			from;
		account_id_type				to;
		account_id_type				arbitr;
		asset 						amount;
		approved_transfer_id_type 	ato;

		account_id_type		fee_payer()const { return to; }
		void				validate()const;
	};

struct approved_transfer_resolve_dispute_operation : public base_operation
	{
		struct fee_parameters_type { uint64_t fee = 0; };
		asset						fee;
		account_id_type 			from;
		account_id_type				to;
		account_id_type				arbitr;
		asset 						amount;
		account_id_type 			winner;
		approved_transfer_id_type 	ato;

		account_id_type		fee_payer()const { return arbitr; }
		void				validate()const;
	};
         

struct mass_payment_operation : public base_operation
	{
		struct fee_parameters_type { 
			uint64_t fee = 5 * GRAPHENE_BLOCKCHAIN_PRECISION; 
			uint64_t price_per_transfer = 0.1 * GRAPHENE_BLOCKCHAIN_PRECISION;
      	};

		asset								fee;
		account_id_type 					from;
        asset_id_type 						asset_id;
		map<account_id_type, share_type> 	payments;

		account_id_type		fee_payer()const { return from; }
		void				validate()const;
        share_type      	calculate_fee(const fee_parameters_type& k)const;

	};

struct mass_payment_pay_operation : public base_operation
	{
		struct fee_parameters_type { uint64_t fee = 0; };
		asset						fee;
		account_id_type 			from;
		account_id_type				to;
		asset 						amount;

		account_id_type		fee_payer()const { return from; }
		void				validate()const;
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

FC_REFLECT( graphene::chain::buy_gcwd_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::buy_gcwd_operation,
	(fee)
	(account)
	(amount)
)

FC_REFLECT( graphene::chain::approved_transfer_create_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::approved_transfer_create_operation,
	(fee)
	(from)
	(to)
	(arbitr)
	(expiration)
	(amount)
)

FC_REFLECT( graphene::chain::approved_transfer_approve_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::approved_transfer_approve_operation,
	(fee)
	(from)
	(to)
	(arbitr)
	(amount)
	(ato)
)

FC_REFLECT( graphene::chain::approved_transfer_cancel_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::approved_transfer_cancel_operation,
	(fee)
	(from)
	(to)
	(arbitr)
	(amount)
	(ato)
)

FC_REFLECT( graphene::chain::approved_transfer_open_dispute_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::approved_transfer_open_dispute_operation,
	(fee)
	(from)
	(to)
	(arbitr)
	(amount)
	(ato)
)

FC_REFLECT( graphene::chain::approved_transfer_resolve_dispute_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::approved_transfer_resolve_dispute_operation,
	(fee)
	(from)
	(to)
	(arbitr)
	(amount)
	(winner)
	(ato)
)

FC_REFLECT( graphene::chain::mass_payment_operation::fee_parameters_type, (fee)(price_per_transfer) )
FC_REFLECT( graphene::chain::mass_payment_operation,
	(fee)
	(from)
	(asset_id)
	(payments)
)

FC_REFLECT( graphene::chain::mass_payment_pay_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::mass_payment_pay_operation,
	(fee)
	(from)
	(to)
	(amount)
)