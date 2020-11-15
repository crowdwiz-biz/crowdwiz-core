/*
 * Copyright (c) 2019 CrowdWiz., and contributors.
 *
 * The MIT License
 */

#include <graphene/chain/protocol/financial.hpp>

namespace graphene { namespace chain {

void credit_system_get_operation::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
		FC_ASSERT( credit_amount.amount > 0, "Credit amount should not be negative" );
	}

void credit_total_repay_operation::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
		FC_ASSERT( repay_amount.amount > 0, "Repay amount should not be negative" );
	}

void credit_repay_operation::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
		FC_ASSERT( repay_amount.amount > 0, "Repay amount should not be negative" );
	}

void credit_offer_create_operation::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
		FC_ASSERT( credit_amount.amount > 0, "Credit amount should not be negative" );
		FC_ASSERT( repay_amount.amount > 0, "Repay amount should not be negative" );
	}

void credit_offer_cancel_operation::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
	}

void credit_offer_fill_operation::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
		FC_ASSERT( credit_amount.amount > 0, "Credit amount should not be negative" );
	}

void pledge_offer_give_create_operation::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
		FC_ASSERT( credit_amount.amount > 0, "Credit amount should not be negative" );
		FC_ASSERT( repay_amount.amount > 0, "Repay amount should not be negative" );
		FC_ASSERT( pledge_amount.amount > 0, "Pledge amount should not be negative" );
		FC_ASSERT( pledge_days > 0, "Pledge amount should not be negative" );
	}

void pledge_offer_take_create_operation::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
		FC_ASSERT( credit_amount.amount > 0, "Credit amount should not be negative" );
		FC_ASSERT( repay_amount.amount > 0, "Repay amount should not be negative" );
		FC_ASSERT( pledge_amount.amount > 0, "Pledge amount should not be negative" );
		FC_ASSERT( pledge_days > 0, "Pledge amount should not be negative" );
}

void pledge_offer_cancel_operation::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
	}

void pledge_offer_fill_operation::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
		FC_ASSERT( credit_amount.amount > 0, "Credit amount should not be negative" );
		FC_ASSERT( repay_amount.amount > 0, "Repay amount should not be negative" );
		FC_ASSERT( pledge_amount.amount > 0, "Pledge amount should not be negative" );
		FC_ASSERT( pledge_days > 0, "Pledge amount should not be negative" );
	}

void pledge_offer_repay_operation::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
		FC_ASSERT( repay_amount.amount > 0, "Repay amount should not be negative" );
	}

void pledge_offer_auto_repay_operation::validate()const
	{
		FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
		FC_ASSERT( credit_amount.amount > 0, "Credit amount should not be negative" );
		FC_ASSERT( repay_amount.amount > 0, "Repay amount should not be negative" );
		FC_ASSERT( pledge_amount.amount > 0, "Pledge amount should not be negative" );
	}

void buy_gcwd_operation::validate()const {
	FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
}
void approved_transfer_create_operation::validate()const {
	FC_ASSERT( from != arbitr, "Arbitr must not participate in the transaction" );
	FC_ASSERT( to != arbitr, "Arbitr must not participate in the transaction" );
	FC_ASSERT( from != to, "Нou should not transfer to yourself" );
	FC_ASSERT( amount.amount > 0, "Amount should be greater than zero" );
	FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
}
void approved_transfer_approve_operation::validate()const {
	FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
}
void approved_transfer_cancel_operation::validate()const {
	FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
}
void approved_transfer_open_dispute_operation::validate()const {
	FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
}
void approved_transfer_resolve_dispute_operation::validate()const {
	FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
}
void mass_payment_operation::validate()const {
	FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
	FC_ASSERT( payments.size() > 0, "Payments quantity must be greater than zero" );
	FC_ASSERT( payments.size() <= 1000, "Payments quantity must be lower or equal than 1000" );
}
void mass_payment_pay_operation::validate()const {
	FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
}

share_type mass_payment_operation::calculate_fee(const fee_parameters_type& k) const
{
	share_type result = k.fee + (payments.size() * k.price_per_transfer);
	return result;
}


} } // graphene::chain