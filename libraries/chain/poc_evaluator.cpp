/*
 * Copyright (c) 2019 CrowdWiz., and contributors.
 *
 * The MIT License
 */

#include <graphene/chain/poc_evaluator.hpp>
#include <graphene/chain/account_object.hpp>
#include <graphene/chain/database.hpp>
#include <graphene/chain/hardfork.hpp>
#include <graphene/chain/vesting_balance_object.hpp>
#include <graphene/chain/greatrace_object.hpp>


namespace graphene
{
	namespace chain
	{

		share_type staking_reward(share_type a, uint64_t p)
		{
			if (a == 0 || p == 0)
				return 0;
			if (p == GRAPHENE_100_PERCENT)
				return a;

			fc::uint128 r(a.value);
			r *= p;
			r /= GRAPHENE_100_PERCENT;
			return r.to_uint64();
		}

		share_type pay_staking_reward(const account_object& account, const account_object& acc_ref, asset stak_amount, uint8_t level, uint16_t poc_ref, database& d)
		{
			asset ref_amount;
			ref_amount.asset_id=asset_id_type();
			ref_amount.amount=0;
			const global_property_object& gpo = d.get_global_properties();
			const dynamic_global_property_object& dgpo = d.get_dynamic_global_properties();
			const auto& poc_params = gpo.staking_parameters;
			const account_statistics_object& stats = acc_ref.statistics(d);

			if (acc_ref.referral_status_type == 0 && poc_params.poc_status_levels_00>=level) {
				ref_amount.amount=staking_reward(stak_amount.amount,poc_ref);
				if (poc_params.poc_status_denominator_00 < (100*GRAPHENE_1_PERCENT)) {
					ref_amount.amount=staking_reward(ref_amount.amount,poc_params.poc_status_denominator_00);
				}
			}
			if (acc_ref.referral_status_type == 1 && poc_params.poc_status_levels_01>=level) {
				ref_amount.amount=staking_reward(stak_amount.amount,poc_ref);
				if (poc_params.poc_status_denominator_01 < (100*GRAPHENE_1_PERCENT)) {
					ref_amount.amount=staking_reward(ref_amount.amount,poc_params.poc_status_denominator_01);
				}
			}
			if (acc_ref.referral_status_type == 2 && poc_params.poc_status_levels_02>=level) {
				ref_amount.amount=staking_reward(stak_amount.amount,poc_ref);
				if (poc_params.poc_status_denominator_02 < (100*GRAPHENE_1_PERCENT)) {
					ref_amount.amount=staking_reward(ref_amount.amount,poc_params.poc_status_denominator_02);
				}
			}
			if (acc_ref.referral_status_type == 3 && poc_params.poc_status_levels_03>=level) {
				ref_amount.amount=staking_reward(stak_amount.amount,poc_ref);
				if (poc_params.poc_status_denominator_03 < (100*GRAPHENE_1_PERCENT)) {
					ref_amount.amount=staking_reward(ref_amount.amount,poc_params.poc_status_denominator_03);
				}
			}
			if (acc_ref.referral_status_type == 4 && poc_params.poc_status_levels_04>=level) {
				ref_amount.amount=staking_reward(stak_amount.amount,poc_ref);
				if (poc_params.poc_status_denominator_04 < (100*GRAPHENE_1_PERCENT)) {
					ref_amount.amount=staking_reward(ref_amount.amount,poc_params.poc_status_denominator_04);
				}
			}
			if (ref_amount.amount>0) {
					//GR_REWARD
					if (  level == 1 && (dgpo.current_gr_interval == 2  ||
							dgpo.current_gr_interval == 4  ||
							dgpo.current_gr_interval == 6  ||
							dgpo.current_gr_interval == 9  ||
							dgpo.current_gr_interval == 11 ||
							dgpo.current_gr_interval == 13)
						)
						{
							d.modify(d.get(account.statistics), [&](account_statistics_object &s) {
								s.current_period_gr += ref_amount.amount;
							});

							if (account.gr_team.valid()) {
								d.modify(d.get(*account.gr_team), [&](gr_team_object &t) {
									if (dgpo.current_gr_interval == 2) {
										t.gr_interval_2_volume += ref_amount.amount;
									}
									if (dgpo.current_gr_interval == 4) {
										t.gr_interval_4_volume += ref_amount.amount;
									}
									if (dgpo.current_gr_interval == 6) {
										t.gr_interval_6_volume += ref_amount.amount;
									}
									if (dgpo.current_gr_interval == 9) {
										t.gr_interval_9_volume += ref_amount.amount;
									}
									if (dgpo.current_gr_interval == 11) {
										t.gr_interval_11_volume += ref_amount.amount;
									}
									if (dgpo.current_gr_interval == 13) {
										t.gr_interval_13_volume += ref_amount.amount;
									}
								});       
							}
						}


				asset credit_ref_amount;
				credit_ref_amount.asset_id=asset_id_type();
				credit_ref_amount.amount=ref_amount.amount;
				d.modify( stats, [credit_ref_amount]( account_statistics_object& aso )
				{
					aso.current_month_income += credit_ref_amount.amount;
				} );
				if (stats.total_credit > 0) {
					share_type credit = stats.total_credit-stats.allowed_to_repay;

					if (credit > credit_ref_amount.amount) {
						d.modify( stats, [credit_ref_amount]( account_statistics_object& aso )
						{
							aso.allowed_to_repay += credit_ref_amount.amount;
						} );
						credit_ref_amount.amount = 0;
					}
					else {
						credit_ref_amount.amount -= credit;
						account_id_type creditor = stats.creditor;
						if (creditor == account_id_type(0)) {
							share_type total_credit = stats.total_credit;
							d.modify( d.get_core_dynamic_data(), [total_credit](asset_dynamic_data_object& dd) {
								dd.current_supply -= total_credit;
							});

							credit_total_repay_operation credit_repay;
							credit_repay.debitor = acc_ref.id;
							credit_repay.creditor = creditor;
							credit_repay.repay_amount = total_credit;
							d.push_applied_operation( credit_repay );  
						}
						else {
							d.adjust_balance(creditor, stats.total_credit-stats.credit_repaid);

							credit_total_repay_operation credit_repay;
							credit_repay.debitor = acc_ref.id;
							credit_repay.creditor = creditor;
							credit_repay.repay_amount = stats.total_credit-stats.credit_repaid;
							d.push_applied_operation( credit_repay );  

						}
						d.modify( stats, []( account_statistics_object& aso )
						{
							aso.allowed_to_repay = 0;
							aso.total_credit = 0;
							aso.credit_repaid = 0;
							aso.creditor = account_id_type(0);
						} );
					}
				}

				if (credit_ref_amount.amount>0) {
					d.adjust_balance( acc_ref.id, credit_ref_amount );					
					poc_staking_referal_operation poc_ref_op;
					poc_ref_op.referrer = acc_ref.id;
					poc_ref_op.account = account.id;
					poc_ref_op.level = level;
					poc_ref_op.reward = credit_ref_amount;
					d.push_applied_operation( poc_ref_op );
				}
			}
			else {
				ref_amount.amount = 0;
			}

			return ref_amount.amount;
		}

		void_result poc_vote_evaluator::do_evaluate( const poc_vote_operation &op)
		{
			try
			{
				database& d = db();
				const account_object& account = op.account(d);
				const auto& account_stats = account.statistics(d);

				const dynamic_global_property_object& dgpo = db().get_dynamic_global_properties();
				FC_ASSERT( account_stats.had_staking, "You don't had staking yet" );
				FC_ASSERT( ((account_stats.poc3_vote + account_stats.poc6_vote + account_stats.poc12_vote) == 0), "You already voted this time" );
				FC_ASSERT( dgpo.end_poc_vote_time>=d.head_block_time(), "PoC voting not started yet" );
				return void_result();
			}
			FC_CAPTURE_AND_RETHROW((op))
		}
		void_result poc_vote_evaluator::do_apply( const poc_vote_operation &op)
		{
			try
			{
				database& d = db();
				const account_object& account = op.account(d);
				const auto& account_stats = account.statistics(d);

				d.adjust_balance( op.account, (-op.poc3_vote-op.poc6_vote-op.poc12_vote) );
				
				d.modify(asset_dynamic_data_id_type()(d), [op](asset_dynamic_data_object &addo) {
					addo.accumulated_fees += op.poc3_vote.amount+op.poc6_vote.amount+op.poc12_vote.amount;
				});

				d.modify(account_stats, [&](account_statistics_object &s) {
					s.poc3_vote = op.poc3_vote.amount;
					s.poc6_vote = op.poc6_vote.amount;
					s.poc12_vote = op.poc12_vote.amount;
				});

				return void_result();
			}
			FC_CAPTURE_AND_RETHROW((op))
		}



		// ================== STAKING ===================
		void_result poc_stak_evaluator::do_evaluate( const poc_stak_operation &op)
		{
			try
			{
				FC_ASSERT( op.stak_amount.asset_id == asset_id_type() );
				database& d = db();
				FC_ASSERT( d.head_block_time() >= HARDFORK_CWD6_TIME );
				
				const dynamic_global_property_object& dgpo = d.get_dynamic_global_properties();
				const global_property_object& gpo = d.get_global_properties();
				const auto& poc_params = gpo.staking_parameters;
				const asset_object& core_asset = d.get_core_asset();
				const asset_dynamic_data_object* core_dyn_data = &core_asset.dynamic_asset_data_id(d);
				share_type posible_reward = 0;

				if (op.staking_type == 3) {
					posible_reward = staking_reward(op.stak_amount.amount,(dgpo.poc3_percent+poc_params.poc_ref_01+poc_params.poc_ref_02+poc_params.poc_ref_03+poc_params.poc_ref_04+poc_params.poc_ref_05+poc_params.poc_ref_06+poc_params.poc_ref_07+poc_params.poc_ref_08+poc_params.poc_gcwd));
					FC_ASSERT( op.stak_amount.amount >= poc_params.poc3_min_amount, "Your stak amount lower than minimal allowed amount" );
				}
				if (op.staking_type == 6) {
					posible_reward = staking_reward(op.stak_amount.amount,(dgpo.poc6_percent+poc_params.poc_ref_01+poc_params.poc_ref_02+poc_params.poc_ref_03+poc_params.poc_ref_04+poc_params.poc_ref_05+poc_params.poc_ref_06+poc_params.poc_ref_07+poc_params.poc_ref_08+poc_params.poc_gcwd));
					FC_ASSERT( op.stak_amount.amount >= poc_params.poc6_min_amount, "Your stak amount lower than minimal allowed amount" );
				}
				if (op.staking_type == 12) {
					posible_reward = staking_reward(op.stak_amount.amount,(dgpo.poc12_percent+poc_params.poc_ref_01+poc_params.poc_ref_02+poc_params.poc_ref_03+poc_params.poc_ref_04+poc_params.poc_ref_05+poc_params.poc_ref_06+poc_params.poc_ref_07+poc_params.poc_ref_08+poc_params.poc_gcwd));
					FC_ASSERT( op.stak_amount.amount >= poc_params.poc12_min_amount, "Your stak amount lower than minimal allowed amount" );
				}
				FC_ASSERT( (core_dyn_data->current_supply + op.stak_amount.amount + posible_reward) <= core_asset.options.max_supply, "System has reached max_supply CWD limit" );

				return void_result();
			}
			FC_CAPTURE_AND_RETHROW((op))
		}
		void_result poc_stak_evaluator::do_apply( const poc_stak_operation &op)
		{
			try
			{
				database& d = db();
				const dynamic_global_property_object& dgpo = d.get_dynamic_global_properties();
				const time_point_sec now = d.head_block_time();
				const uint32_t seconds_in_month = 2592000;
				d.adjust_balance( op.account, -op.stak_amount );
				const account_object& account = op.account(d);

				const auto& account_stats = account.statistics(d);
				if (!account_stats.had_staking) {
					d.modify(account_stats, [](account_statistics_object &s) {
						s.had_staking = true;
					});
				}
				asset stak_amount;
				stak_amount.asset_id=asset_id_type();
				stak_amount.amount=0;

				share_type current_supply_increase = 0;

				if (op.staking_type == 3) {
					stak_amount.amount = staking_reward(op.stak_amount.amount,dgpo.poc3_percent);
					current_supply_increase += stak_amount.amount;

					// STAKING 3 Month Percent
					d.create<vesting_balance_object>([&](vesting_balance_object& b) {
							b.owner = op.account;
							b.balance = stak_amount;
							linear_vesting_policy policy;
							policy.begin_timestamp = now;
							policy.vesting_cliff_seconds = 0;
							policy.vesting_duration_seconds = seconds_in_month * 3;
							policy.begin_balance = stak_amount.amount;
							b.policy = policy;
						});
					
					// STAKING 3 Month Body
					stak_amount.amount = staking_reward(op.stak_amount.amount,dgpo.poc3_percent);
					d.create<vesting_balance_object>([&](vesting_balance_object& b) {
							b.owner = op.account;
							b.balance = op.stak_amount;
							linear_vesting_policy policy;
							policy.begin_timestamp = now + fc::seconds(seconds_in_month * 3);
							policy.vesting_cliff_seconds = 0;
							policy.vesting_duration_seconds = 0;
							policy.begin_balance = op.stak_amount.amount;
							b.policy = policy;
						});
				}

				if (op.staking_type == 6) {
					stak_amount.amount = staking_reward(op.stak_amount.amount,dgpo.poc6_percent);
					current_supply_increase += stak_amount.amount;
					stak_amount.amount += op.stak_amount.amount;
					
					// STAKING 6 Month Body+Percent
					d.create<vesting_balance_object>([&](vesting_balance_object& b) {
							b.owner = op.account;
							b.balance = stak_amount;
							linear_vesting_policy policy;
							policy.begin_timestamp = now + fc::seconds(seconds_in_month * 6);
							policy.vesting_cliff_seconds = 0;
							policy.vesting_duration_seconds = 0;
							policy.begin_balance = stak_amount.amount;
							b.policy = policy;
						});
				}

				if (op.staking_type == 12) {
					stak_amount.amount = staking_reward(op.stak_amount.amount,dgpo.poc12_percent);
					current_supply_increase += stak_amount.amount;
					stak_amount.amount += op.stak_amount.amount;
					
					// STAKING 12 Month Body+Percent
					d.create<vesting_balance_object>([&](vesting_balance_object& b) {
							b.owner = op.account;
							b.balance = stak_amount;
							linear_vesting_policy policy;
							policy.begin_timestamp = now;
							policy.vesting_cliff_seconds = 0;
							policy.vesting_duration_seconds = seconds_in_month * 12;
							policy.begin_balance = stak_amount.amount;
							b.policy = policy;
						});
				}

				// PAY NETWORK REWARD
				const account_object& acc_ref1 = account.referrer(d);
				const global_property_object& gpo = d.get_global_properties();
				const auto& poc_params = gpo.staking_parameters;

				if (acc_ref1.id != account_id_type())
					current_supply_increase+=pay_staking_reward(account, acc_ref1, op.stak_amount, 1, poc_params.poc_ref_01, d);

				const account_object& acc_ref2 = acc_ref1.referrer(d);
				if (acc_ref2.id != account_id_type())
					current_supply_increase+=pay_staking_reward(account, acc_ref2, op.stak_amount, 2, poc_params.poc_ref_02, d);

				const account_object& acc_ref3 = acc_ref2.referrer(d);
				if (acc_ref3.id != account_id_type())
					current_supply_increase+=pay_staking_reward(account, acc_ref3, op.stak_amount, 3, poc_params.poc_ref_03, d);

				const account_object& acc_ref4 = acc_ref3.referrer(d);
				if (acc_ref4.id != account_id_type())
					current_supply_increase+=pay_staking_reward(account, acc_ref4, op.stak_amount, 4, poc_params.poc_ref_04, d);

				const account_object& acc_ref5 = acc_ref4.referrer(d);
				if (acc_ref5.id != account_id_type())
					current_supply_increase+=pay_staking_reward(account, acc_ref5, op.stak_amount, 5, poc_params.poc_ref_05, d);

				const account_object& acc_ref6 = acc_ref5.referrer(d);
				if (acc_ref6.id != account_id_type())
					current_supply_increase+=pay_staking_reward(account, acc_ref6, op.stak_amount, 6, poc_params.poc_ref_06, d);

				const account_object& acc_ref7 = acc_ref6.referrer(d);
				if (acc_ref7.id != account_id_type())
					current_supply_increase+=pay_staking_reward(account, acc_ref7, op.stak_amount, 7, poc_params.poc_ref_07, d);

				const account_object& acc_ref8 = acc_ref7.referrer(d);
				if (acc_ref8.id != account_id_type())
					current_supply_increase+=pay_staking_reward(account, acc_ref8, op.stak_amount, 8, poc_params.poc_ref_08, d);

				// GCWD REWARD
				share_type gcwd_amount;
				gcwd_amount=staking_reward(op.stak_amount.amount,poc_params.poc_gcwd);
				current_supply_increase+=gcwd_amount;

				d.modify(asset_dynamic_data_id_type()(d), [gcwd_amount](asset_dynamic_data_object &addo) {
					addo.accumulated_fees += gcwd_amount;
				});
				// APOSTOLOS REWARD
				if (d.head_block_time() >= HARDFORK_CWD7_TIME)
				{
					const auto& gr_params = gpo.greatrace_parameters;
					share_type apostolos_amount;
					apostolos_amount=staking_reward(op.stak_amount.amount,gr_params.apostolos_reward);
					current_supply_increase+=apostolos_amount;

					const account_object& apostolos_account = d.get(GRAPHENE_APOSTOLOS_ACCOUNT);
					d.deposit_cashback(apostolos_account,apostolos_amount,false,false);
				}
				// INCREASE CWD CURRENT SUPPLY 
				d.modify( asset_dynamic_data_id_type()(d), [current_supply_increase]( asset_dynamic_data_object& dd ) {
					dd.current_supply += current_supply_increase;
				});	

				return void_result();
			}
			FC_CAPTURE_AND_RETHROW((op))
		}

		void_result exchange_silver_evaluator::do_evaluate( const exchange_silver_operation &op)
		{
			try
			{
				database& d = db();
				FC_ASSERT(d.head_block_time() >= HARDFORK_CWD6_TIME, "HF6 not yet activated");

				const account_object& from_account = op.account(d);

				const asset_object& asset_type = op.amount.asset_id(d);
				const asset_object& core_asset = d.get_core_asset();
				const asset_dynamic_data_object* core_dyn_data = &core_asset.dynamic_asset_data_id(d);

				bool insufficient_balance = d.get_balance( from_account, asset_type ).amount >= op.amount.amount;

				FC_ASSERT( insufficient_balance,
                 "Insufficient Balance: ${balance}, unable to exchange '${total_transfer}' from account '${a}'", 
                 ("a",from_account.name)("total_transfer",d.to_pretty_string(op.amount))("balance",d.to_pretty_string(d.get_balance(from_account, asset_type))) );

				FC_ASSERT( (core_dyn_data->current_supply + op.amount.amount) <= core_asset.options.max_supply, "System has reached max_supply CWD limit" );

				return void_result();
			}
			FC_CAPTURE_AND_RETHROW((op))
		}
		void_result exchange_silver_evaluator::do_apply( const exchange_silver_operation &op)
		{
			try
			{
				database& d = db();

				asset core_amount;
				core_amount.asset_id = asset_id_type();
				core_amount.amount = op.amount.amount;

				d.adjust_balance( op.account, -op.amount );
				d.adjust_balance( op.account, core_amount );

				d.modify( asset_dynamic_data_id_type()(d), [op]( asset_dynamic_data_object& dd ) {
					dd.current_supply += op.amount.amount;
				});

				d.modify( asset_dynamic_data_id_type(4)(d), [op]( asset_dynamic_data_object& dd ) {
					if (dd.current_supply >= op.amount.amount) {
						dd.current_supply -= op.amount.amount;
					}
					else {
						dd.current_supply = 0;
					}
				});

				return void_result();
			}
			FC_CAPTURE_AND_RETHROW((op))
		}

	}
}