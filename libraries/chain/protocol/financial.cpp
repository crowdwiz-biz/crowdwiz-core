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

} } // graphene::chain