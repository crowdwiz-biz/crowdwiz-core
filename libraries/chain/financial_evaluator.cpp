/*
 * Copyright (c) 2019 CrowdWiz., and contributors.
 *
 * The MIT License
 */

#include <graphene/chain/financial_evaluator.hpp>
#include <graphene/chain/financial_object.hpp>
#include <graphene/chain/account_object.hpp>
#include <graphene/chain/database.hpp>
#include <graphene/chain/hardfork.hpp>

namespace graphene
{
namespace chain
{

void_result credit_system_get_evaluator::do_evaluate( const credit_system_get_operation &op)
{
    try
    {
		database& d = db();

		FC_ASSERT(d.head_block_time() >= HARDFORK_CWD5_TIME, "HF5 not yet activated");

		const account_object& debitor = op.debitor(d);
		const auto& debitor_stats = debitor.statistics(d);
		share_type max_credit = debitor_stats.first_month_income+debitor_stats.second_month_income+debitor_stats.third_month_income;
		
		FC_ASSERT(debitor_stats.total_credit == 0, "You already have credit");
		FC_ASSERT( op.credit_amount.asset_id == asset_id_type(), "Credit must be in core asset");
		FC_ASSERT(op.credit_amount.amount <= max_credit, "Credit must be lower or equal 3 month income");

        return void_result();
    }
    FC_CAPTURE_AND_RETHROW((op))
}
void_result credit_system_get_evaluator::do_apply( const credit_system_get_operation &op)
{
    try
    {
		database& d = db();
		const account_object& debitor = op.debitor(d);
		const auto& debitor_stats = debitor.statistics(d);

		d.modify( get_core_dynamic_data(), [&](asset_dynamic_data_object& dd) {
			dd.current_supply += op.credit_amount.amount;
		});

		d.modify(debitor_stats, [&](account_statistics_object &s) {
			s.total_credit = op.credit_amount.amount * 2;
			s.allowed_to_repay = 0;
			s.credit_repaid = 0;
			s.creditor = account_id_type(0);
		});

		d.adjust_balance( op.debitor, op.credit_amount);

        return void_result();
    }
    FC_CAPTURE_AND_RETHROW((op))
}


void_result credit_repay_evaluator::do_evaluate( const credit_repay_operation &op)
{
    try
    {
		database& d = db();
		FC_ASSERT(d.head_block_time() >= HARDFORK_CWD5_TIME, "HF5 not yet activated");

		const account_object& debitor = op.debitor(d);
		const auto& debitor_stats = debitor.statistics(d);

		FC_ASSERT(op.credit_amount.asset_id == asset_id_type(), "Credit repay must be in core asset");
		FC_ASSERT(debitor_stats.allowed_to_repay - debitor_stats.credit_repaid >= op.repay_amount.amount, "Not enough allowed to repay");
		FC_ASSERT(debitor_stats.creditor != account_id_type(0), "Not allowed for system account");

        return void_result();
    }
    FC_CAPTURE_AND_RETHROW((op))
}
void_result credit_repay_evaluator::do_apply( const credit_repay_operation &op)
{
    try
    {
		database& d = db();
		const account_object& debitor = op.debitor(d);
		const auto& debitor_stats = debitor.statistics(d);
        account_id_type creditor = debitor_stats.creditor;

        d.modify(debitor_stats, [&](account_statistics_object &s) {
            s.credit_repaid += op.repay_amount.amount;
        });

		d.adjust_balance(creditor, op.repay_amount);
        return void_result();
    }
    FC_CAPTURE_AND_RETHROW((op))
}

void_result credit_offer_create_evaluator::do_evaluate( const credit_offer_create_operation &op)
{
    try
    {
		database& d = db();
		FC_ASSERT(d.head_block_time() >= HARDFORK_CWD5_TIME, "HF5 not yet activated");
		FC_ASSERT( op.credit_amount.asset_id == asset_id_type(), "Credit must be in core asset");

        return void_result();
    }
    FC_CAPTURE_AND_RETHROW((op))
}
object_id_type credit_offer_create_evaluator::do_apply( const credit_offer_create_operation &op)
{
    try
    {
        const auto& new_credit_offer_object = db().create<credit_offer_object>([&](credit_offer_object &obj) {
            obj.min_income = op.min_income;
            obj.creditor = op.creditor;
            obj.credit_amount = op.credit_amount;
			obj.repay_amount = op.repay_amount;
        });
        return new_credit_offer_object.id;
    }
    FC_CAPTURE_AND_RETHROW((op))
}

void_result credit_offer_cancel_evaluator::do_evaluate( const credit_offer_cancel_operation &op)
{
    try
    {
		database& d = db();
		FC_ASSERT(d.head_block_time() >= HARDFORK_CWD5_TIME, "HF5 not yet activated");
		const credit_offer_object& credit_offer = op.credit_offer(d);
		FC_ASSERT(credit_offer, "No such credit offer");
		FC_ASSERT(credit_offer.creditor == op.creditor, "You not owner of this credit offer");
        return void_result();
    }
    FC_CAPTURE_AND_RETHROW((op))
}
void_result credit_offer_cancel_evaluator::do_apply( const credit_offer_cancel_operation &op)
{
    try
    {
		database& d = db();
		const credit_offer_object& credit_offer = op.credit_offer(d);
		d.remove(credit_offer);

        return void_result();
    }
    FC_CAPTURE_AND_RETHROW((op))
}

void_result credit_offer_fill_evaluator::do_evaluate( const credit_offer_fill_operation &op)
{
    try
    {
		database& d = db();
		FC_ASSERT(d.head_block_time() >= HARDFORK_CWD5_TIME, "HF5 not yet activated");
		const account_object& debitor = op.debitor(d);
		const auto& debitor_stats = debitor.statistics(d);
		share_type debitor_income = debitor_stats.first_month_income+debitor_stats.second_month_income+debitor_stats.third_month_income;
		const credit_offer_object& credit_offer = op.credit_offer(d);
		const account_object& creditor = credit_offer.creditor(d);
		const asset_object& asset_type = op.credit_amount.asset_id(d);

		bool insufficient_balance = d.get_balance( creditor, asset_type ).amount >= op.credit_amount.amount;
		FC_ASSERT( insufficient_balance,
					"Insufficient creditor Balance: ${balance}, unable to get credit '${total_credit}' from account '${a}'", 
					("a",creditor.name)("total_credit",d.to_pretty_string(op.credit_amount))("balance",d.to_pretty_string(d.get_balance(creditor, asset_type))) );

		FC_ASSERT(credit_offer, "No such credit offer");
		FC_ASSERT(debitor_stats.total_credit == 0, "You already have credit");
		FC_ASSERT(op.credit_amount.asset_id == asset_id_type(), "Credit must be in core asset");
		FC_ASSERT(credit_offer.min_income <= debitor_income, "Not enough 3 month income");
		FC_ASSERT(credit_offer.credit_amount == op.credit_amount, "Amounts must match");

        return void_result();
    }
    FC_CAPTURE_AND_RETHROW((op))
}
void_result credit_offer_fill_evaluator::do_apply( const credit_offer_fill_operation &op)
{
    try
    {
		database& d = db();
		const account_object& debitor = op.debitor(d);
		const auto& debitor_stats = debitor.statistics(d);
		const credit_offer_object& credit_offer = op.credit_offer(d);

         d.modify(debitor_stats, [&](account_statistics_object &s) {
            s.total_credit = credit_offer.repay_amount;
            s.allowed_to_repay = 0;
            s.credit_repaid = 0;
            s.creditor = credit_offer.creditor;
         });
		d.adjust_balance( credit_offer.creditor, -op.credit_amount);
		d.adjust_balance( op.debitor, op.credit_amount);

		d.remove(credit_offer);

        return void_result();
    }
    FC_CAPTURE_AND_RETHROW((op))
}

void_result pledge_offer_give_create_evaluator::do_evaluate( const pledge_offer_give_create_operation &op)
{
    try
    {
		database& d = db();
		FC_ASSERT(d.head_block_time() >= HARDFORK_CWD5_TIME, "HF5 not yet activated");
		FC_ASSERT(op.creator == op.creditor, "Creator must be creditor");
        return void_result();
    }
    FC_CAPTURE_AND_RETHROW((op))
}
object_id_type pledge_offer_give_create_evaluator::do_apply( const pledge_offer_give_create_operation &op)
{
    try
    {
        const auto& new_pledge_offer_object = db().create<pledge_offer_object>([&](pledge_offer_object &obj) {
			obj.creator = op.creditor;
			obj.creditor = op.creditor;
			obj.pledge_amount = op.pledge_amount;
			obj.credit_amount = op.credit_amount;
			obj.repay_amount = op.repay_amount;
			obj.pledge_days = op.pledge_days;
        });
        return new_pledge_offer_object.id;
    }
    FC_CAPTURE_AND_RETHROW((op))
}

void_result pledge_offer_take_create_evaluator::do_evaluate( const pledge_offer_take_create_operation &op)
{
    try
    {
		database& d = db();
		FC_ASSERT(d.head_block_time() >= HARDFORK_CWD5_TIME, "HF5 not yet activated");
		FC_ASSERT(op.creator == op.debitor, "Creator must be creditor");
        return void_result();
    }
    FC_CAPTURE_AND_RETHROW((op))
}
object_id_type pledge_offer_take_create_evaluator::do_apply( const pledge_offer_take_create_operation &op)
{
    try
    {
        const auto& new_pledge_offer_object = db().create<pledge_offer_object>([&](pledge_offer_object &obj) {
			obj.creator = op.debitor;
			obj.debitor = op.debitor;
			obj.pledge_amount = op.pledge_amount;
			obj.credit_amount = op.credit_amount;
			obj.repay_amount = op.repay_amount;
			obj.pledge_days = op.pledge_days;
        });
        return new_pledge_offer_object.id;
    }
    FC_CAPTURE_AND_RETHROW((op))
}

void_result pledge_offer_cancel_evaluator::do_evaluate( const pledge_offer_cancel_operation &op)
{
    try
    {
		database& d = db();
		FC_ASSERT(d.head_block_time() >= HARDFORK_CWD5_TIME, "HF5 not yet activated");
		const pledge_offer_object& pledge_offer = op.pledge_offer(d);
		FC_ASSERT(pledge_offer, "No such pledge offer");
		FC_ASSERT(pledge_offer.creator == op.creator, "You not owner of this pledge offer");
        return void_result();
    }
    FC_CAPTURE_AND_RETHROW((op))
}
void_result pledge_offer_cancel_evaluator::do_apply( const pledge_offer_cancel_operation &op)
{
    try
    {
		database& d = db();
		const pledge_offer_object& pledge_offer = op.pledge_offer(d);
		d.remove(pledge_offer);
        return void_result();
    }
    FC_CAPTURE_AND_RETHROW((op))
}

void_result pledge_offer_fill_evaluator::do_evaluate( const pledge_offer_fill_operation &op)
{
    try
    {
		database& d = db();
		FC_ASSERT(d.head_block_time() >= HARDFORK_CWD5_TIME, "HF5 not yet activated");
		const pledge_offer_object& pledge_offer = op.pledge_offer(d);
		const account_object& creditor = op.creditor(d);
		const account_object& debitor = op.debitor(d);

		const asset_object& credit_asset_type = op.credit_amount.asset_id(d);
		const asset_object& pledge_asset_type = op.pledge_amount.asset_id(d);

		bool insufficient_balance_creditor = d.get_balance( creditor, credit_asset_type ).amount >= op.credit_amount.amount;
		bool insufficient_balance_debitor = d.get_balance( debitor, pledge_asset_type ).amount >= op.pledge_amount.amount;

		FC_ASSERT(pledge_offer, "No such pledge offer");
		FC_ASSERT(pledge_offer.status == 0, "This pledge offer already filled");

		FC_ASSERT(pledge_offer.pledge_amount == op.pledge_amount, "Pledge amount must match");
		FC_ASSERT(pledge_offer.credit_amount == op.credit_amount, "Credit amount must match");
		FC_ASSERT(pledge_offer.repay_amount == op.repay_amount, "Repay amount must match");
		FC_ASSERT(pledge_offer.pledge_days == op.pledge_days, "Pledge days amount must match");

		if (pledge_offer.debitor) {
			FC_ASSERT(pledge_offer.debitor == op.debitor, "Wrong debitor");
			FC_ASSERT(op.account == op.creditor, "Account must be creditor");
		}
		else {
			FC_ASSERT(pledge_offer.creditor == op.creditor, "Wrong creditor");
			FC_ASSERT(op.account == op.debitor, "Account must be debitor");
		}

		FC_ASSERT( insufficient_balance_creditor,
					"Insufficient creditor unable to get credit '${total_credit}' from account '${a}'", 
					("a",creditor.name)("total_credit",d.to_pretty_string(op.credit_amount)) );
		FC_ASSERT( insufficient_balance_debitor,
					"Insufficient debitor unable to get pledge '${total_pledge}' from account '${a}'", 
					("a",debitor.name)("total_pledge",d.to_pretty_string(op.pledge_amount)) );		

        return void_result();

    }
    FC_CAPTURE_AND_RETHROW((op))
}
void_result pledge_offer_fill_evaluator::do_apply( const pledge_offer_fill_operation &op)
{
    try
    {
		database& d = db();
		const pledge_offer_object& pledge_offer = op.pledge_offer(d);

		if (pledge_offer.debitor) {
			d.modify(d.get(op.pledge_offer), [&]( pledge_offer_object& po_obj )
			{
				po_obj.creditor = op.creditor;
				po_obj.status = 1;
				po_obj.expiration = d.head_block_time() + fc::days(lg_obj.pledge_days);
			});
		}
		else {
			d.modify(d.get(op.pledge_offer), [&]( pledge_offer_object& po_obj )
			{
				po_obj.debitor = op.debitor;
				po_obj.status = 1;
				po_obj.expiration = d.head_block_time() + fc::days(lg_obj.pledge_days);
			});
		}


		d.adjust_balance( op.creditor, -op.credit_amount);
		d.adjust_balance( op.debitor, -op.pledge_amount);
		d.adjust_balance( op.debitor, op.credit_amount);

        return void_result();
    }
    FC_CAPTURE_AND_RETHROW((op))
}

void_result pledge_offer_repay_evaluator::do_evaluate( const pledge_offer_repay_operation &op)
{
    try
    {
		database& d = db();
		FC_ASSERT(d.head_block_time() >= HARDFORK_CWD5_TIME, "HF5 not yet activated");
		const pledge_offer_object& pledge_offer = op.pledge_offer(d);
		const account_object& creditor = pledge_offer.creditor(d);
		const account_object& debitor = pledge_offer.debitor(d);

		const asset_object& credit_asset_type = op.credit_amount.asset_id(d);
		const asset_object& repay_asset_type = op.repay_amount.asset_id(d);

		FC_ASSERT(pledge_offer, "No such pledge offer");
		FC_ASSERT(pledge_offer.status == 1, "This pledge offer not filled yet");

		FC_ASSERT(pledge_offer.pledge_amount == op.pledge_amount, "Pledge amount must match");
		FC_ASSERT(pledge_offer.repay_amount == op.repay_amount, "Repay amount must match");

		FC_ASSERT(pledge_offer.creditor == op.creditor, "Creditor must match");
		FC_ASSERT(pledge_offer.debitor == op.debitor, "Debitor must match");

		bool insufficient_balance_debitor = d.get_balance( debitor, repay_asset_type ).amount >= op.repay_amount.amount;

		FC_ASSERT( insufficient_balance_debitor,
					"Insufficient debitor unable to repay '${total_repay}' from account '${a}'", 
					("a",debitor.name)("total_repay",d.to_pretty_string(op.repay_amount)) );	
        return void_result();
    }
    FC_CAPTURE_AND_RETHROW((op))
}
void_result pledge_offer_repay_evaluator::do_apply( const pledge_offer_repay_operation &op)
{
    try
    {
		database& d = db();
		const pledge_offer_object& pledge_offer = op.pledge_offer(d);
		d.adjust_balance( op.debitor, -op.repay_amount);
		d.adjust_balance( op.creditor, op.repay_amount);
		d.adjust_balance( op.debitor, op.pledge_amount);

		d.remove(pledge_offer);

        return void_result();
    }
    FC_CAPTURE_AND_RETHROW((op))
}


}} // namespace chain