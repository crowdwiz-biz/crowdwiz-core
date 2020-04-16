/*
 * Copyright (c) 2019 CrowdWiz., and contributors.
 *
 * The MIT License
 */

#pragma once
#include <graphene/chain/protocol/base.hpp>
#include <graphene/chain/protocol/memo.hpp>


namespace graphene { namespace chain { 

	struct create_p2p_adv_operation : public base_operation
	{
		struct fee_parameters_type {
			uint64_t fee             = 5 * GRAPHENE_BLOCKCHAIN_PRECISION;
			uint32_t price_per_kbyte = 1 * GRAPHENE_BLOCKCHAIN_PRECISION;
		};

		asset                        fee;
		account_id_type              p2p_gateway;
		bool                         adv_type = true;
		string                       adv_description;
		share_type                   max_cwd;
		share_type                   min_cwd;
		share_type                   price;
		string                       currency;
		uint32_t                     min_p2p_complete_deals = 0;
		uint8_t                      min_account_status = 0;
		uint32_t                     timelimit_for_reply = 3600;
		uint32_t           			 timelimit_for_approve = 3600;
		string                       geo;

		account_id_type              fee_payer() const { return p2p_gateway; }
		void                         validate()const;
		share_type                   calculate_fee(const fee_parameters_type& k)const;
	};

	struct edit_p2p_adv_operation : public base_operation
	{
		struct fee_parameters_type {
			uint64_t fee             = 1 * GRAPHENE_BLOCKCHAIN_PRECISION;
			uint32_t price_per_kbyte = 1 * GRAPHENE_BLOCKCHAIN_PRECISION;
		};

		asset                        fee;
		p2p_adv_id_type              p2p_adv;
		account_id_type              p2p_gateway;
		bool                         adv_type = true;
		string                       adv_description;
		share_type                   max_cwd;
		share_type                   min_cwd;
		share_type                   price;
		string                       currency;
		uint32_t                     min_p2p_complete_deals = 0;
		uint8_t                      min_account_status = 0;
		uint32_t                     timelimit_for_reply = 3600;
		uint32_t			         timelimit_for_approve = 3600;
		string                       geo;
		uint8_t                      status;
		
		account_id_type              fee_payer() const { return p2p_gateway; }
		void                         validate()const;
		share_type                   calculate_fee(const fee_parameters_type& k)const;
	};

	struct clear_p2p_adv_black_list_operation : public base_operation
	{
		struct fee_parameters_type { share_type fee = 1 * GRAPHENE_BLOCKCHAIN_PRECISION; };
		asset           fee;
		p2p_adv_id_type p2p_adv;
		account_id_type p2p_gateway;

		account_id_type fee_payer() const { return p2p_gateway; }
		void            validate()const;
	};

	struct remove_from_p2p_adv_black_list_operation : public base_operation
	{
		struct fee_parameters_type { share_type fee = 0.1 * GRAPHENE_BLOCKCHAIN_PRECISION; };
		asset           fee;
		p2p_adv_id_type p2p_adv;
		account_id_type p2p_gateway;
		account_id_type blacklisted;

		account_id_type fee_payer() const { return p2p_gateway; }
		void            validate()const;
	};
	
	struct create_p2p_order_operation : public base_operation
	{
		struct fee_parameters_type {
			uint64_t fee             = 0;
		};
		asset                        fee;
		p2p_adv_id_type              p2p_adv;
		asset                        amount;
		share_type                   price;
		account_id_type 			 p2p_gateway;
		account_id_type              p2p_client;
		optional<memo_data>          payment_details;

		account_id_type              fee_payer() const { return p2p_client; }
		void                         validate()const;
	};

	struct cancel_p2p_order_operation : public base_operation
	{
		struct fee_parameters_type { share_type fee = 0.5 * GRAPHENE_BLOCKCHAIN_PRECISION; };
		asset             fee;
		account_id_type   p2p_gateway;
		account_id_type   p2p_client;
		p2p_order_id_type p2p_order;
		bool			  blacklist;

		account_id_type   fee_payer() const { return p2p_gateway; }
		void              validate()const;
	};

	struct autocancel_p2p_order_operation : public base_operation //virtual
	{
		struct fee_parameters_type { share_type fee = 0; };
		asset             fee;
		account_id_type   p2p_gateway;
		account_id_type   p2p_client;		
		p2p_order_id_type p2p_order;

		account_id_type   fee_payer() const { return p2p_gateway; }
		void              validate()const;
	};

	struct autorefund_p2p_order_operation : public base_operation //virtual
	{
		struct fee_parameters_type { share_type fee = 0; };
		asset             fee;
		account_id_type   p2p_gateway;
		account_id_type   p2p_client;
		account_id_type   refund_to;
		p2p_order_id_type p2p_order;
		asset			  amount;

		account_id_type   fee_payer() const { return refund_to; }
		void              validate()const;
	};

	struct call_p2p_order_operation : public base_operation
	{
		struct fee_parameters_type {
			uint64_t fee             = 0;
		};

		asset             		fee;
		p2p_order_id_type 		p2p_order;
		account_id_type   		p2p_gateway;
		account_id_type   		p2p_client;
		asset 			  		amount;
		share_type              price;
		memo_data     			payment_details;

		account_id_type   fee_payer() const { return p2p_gateway; }
		void              validate()const;
	};

	struct payment_p2p_order_operation : public base_operation
	{
		struct fee_parameters_type { share_type fee = 0; };
		asset             			fee;
		p2p_order_id_type 			p2p_order;
		account_id_type   			paying_account;
		account_id_type   			recieving_account;
		optional<memo_data>         file_hash;

		account_id_type   fee_payer() const { return paying_account; }
		void              validate()const;
	};

	struct release_p2p_order_operation : public base_operation
	{
		struct fee_parameters_type { share_type fee = 2 * GRAPHENE_BLOCKCHAIN_PRECISION; };
		asset             fee;
		p2p_order_id_type p2p_order;
		account_id_type   paying_account;
		account_id_type   recieving_account;

		account_id_type   fee_payer() const { return recieving_account; }
		void              validate()const;
	};

	struct open_p2p_dispute_operation : public base_operation
	{
		struct fee_parameters_type { share_type fee = 0; };
		asset             fee;
		p2p_order_id_type p2p_order;
		account_id_type   account;
		account_id_type   defendant;
		account_id_type   arbitr;
		memo_data         contact_details;

		account_id_type   fee_payer() const { return account; }
		void              validate()const;
	};

	struct reply_p2p_dispute_operation : public base_operation
	{
		struct fee_parameters_type { share_type fee = 0; };
		asset             fee;
		p2p_order_id_type p2p_order;
		account_id_type   account;
		account_id_type   arbitr;
		memo_data         contact_details;

		account_id_type   fee_payer() const { return account; }
		void              validate()const;
	};

	struct resolve_p2p_dispute_operation : public base_operation
	{
		struct fee_parameters_type { share_type fee = 2 * GRAPHENE_BLOCKCHAIN_PRECISION; };
		asset             fee;
		p2p_order_id_type p2p_order;
		account_id_type   arbitr;
		account_id_type   winner;
		account_id_type   looser;

		account_id_type   fee_payer() const { return arbitr; }
		void              validate()const;
	};
} } // graphene::chain

FC_REFLECT( graphene::chain::create_p2p_adv_operation::fee_parameters_type,
	(fee)
	(price_per_kbyte)
)
FC_REFLECT( graphene::chain::create_p2p_adv_operation,
	(fee)
	(p2p_gateway)
	(adv_type)
	(adv_description)
	(max_cwd)
	(min_cwd)
	(price)
	(currency)
	(min_p2p_complete_deals)
	(min_account_status)
	(timelimit_for_reply)
	(timelimit_for_approve)
	(geo)
)
FC_REFLECT( graphene::chain::edit_p2p_adv_operation::fee_parameters_type,
	(fee)
	(price_per_kbyte)
)
FC_REFLECT( graphene::chain::edit_p2p_adv_operation,
	(fee)
	(p2p_adv)
	(p2p_gateway)
	(adv_type)
	(adv_description)
	(max_cwd)
	(min_cwd)
	(price)
	(currency)
	(min_p2p_complete_deals)
	(min_account_status)
	(timelimit_for_reply)
	(timelimit_for_approve)
	(geo)
	(status)
)
FC_REFLECT( graphene::chain::clear_p2p_adv_black_list_operation::fee_parameters_type,
	(fee)
)
FC_REFLECT( graphene::chain::clear_p2p_adv_black_list_operation,
	(fee)
	(p2p_adv)
	(p2p_gateway)
)
FC_REFLECT( graphene::chain::remove_from_p2p_adv_black_list_operation::fee_parameters_type,
	(fee)
)
FC_REFLECT( graphene::chain::remove_from_p2p_adv_black_list_operation,
	(fee)
	(p2p_adv)
	(p2p_gateway)
	(blacklisted)
)
FC_REFLECT( graphene::chain::create_p2p_order_operation::fee_parameters_type,
	(fee)
)
FC_REFLECT( graphene::chain::create_p2p_order_operation,
	(fee)
	(p2p_adv)
	(amount)
	(price)
	(p2p_gateway)
	(p2p_client)
	(payment_details)
)
FC_REFLECT( graphene::chain::cancel_p2p_order_operation::fee_parameters_type,
	(fee)
)
FC_REFLECT( graphene::chain::cancel_p2p_order_operation,
	(fee)
	(p2p_gateway)
	(p2p_client)
	(p2p_order)
	(blacklist)
)
FC_REFLECT( graphene::chain::autocancel_p2p_order_operation::fee_parameters_type,
	(fee)
)
FC_REFLECT( graphene::chain::autocancel_p2p_order_operation,
	(fee)
	(p2p_gateway)
	(p2p_client)
	(p2p_order)
)
FC_REFLECT( graphene::chain::autorefund_p2p_order_operation::fee_parameters_type,
	(fee)
)
FC_REFLECT( graphene::chain::autorefund_p2p_order_operation,
	(fee)
	(p2p_gateway)
	(p2p_client)
	(refund_to)
	(p2p_order)
	(amount)
)
FC_REFLECT( graphene::chain::call_p2p_order_operation::fee_parameters_type,
	(fee)
)
FC_REFLECT( graphene::chain::call_p2p_order_operation,
	(fee)
	(p2p_order)
	(p2p_gateway)
	(p2p_client)
	(amount)
	(price)
	(payment_details)
)
FC_REFLECT( graphene::chain::payment_p2p_order_operation::fee_parameters_type,
	(fee)
)
FC_REFLECT( graphene::chain::payment_p2p_order_operation,
	(fee)
	(p2p_order)
	(paying_account)
	(recieving_account)
	(file_hash)
)
FC_REFLECT( graphene::chain::release_p2p_order_operation::fee_parameters_type,
	(fee)
)
FC_REFLECT( graphene::chain::release_p2p_order_operation,
	(fee)
	(p2p_order)
	(paying_account)
	(recieving_account)
)
FC_REFLECT( graphene::chain::open_p2p_dispute_operation::fee_parameters_type,
	(fee)
)
FC_REFLECT( graphene::chain::open_p2p_dispute_operation,
	(fee)
	(p2p_order)
	(account)
	(defendant)
	(arbitr)
	(contact_details)
)
FC_REFLECT( graphene::chain::reply_p2p_dispute_operation::fee_parameters_type,
	(fee)
)
FC_REFLECT( graphene::chain::reply_p2p_dispute_operation,
	(fee)
	(p2p_order)
	(account)
	(arbitr)
	(contact_details)
)
FC_REFLECT( graphene::chain::resolve_p2p_dispute_operation::fee_parameters_type,
	(fee)
)
FC_REFLECT( graphene::chain::resolve_p2p_dispute_operation,
	(fee)
	(p2p_order)
	(arbitr)
	(winner)
	(looser)
)