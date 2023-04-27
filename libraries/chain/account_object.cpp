/*
 * Copyright (c) 2015 Cryptonomex, Inc., and contributors.
 *
 * The MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <graphene/chain/account_object.hpp>
#include <graphene/chain/asset_object.hpp>
#include <graphene/chain/greatrace_object.hpp>

#include <graphene/chain/database.hpp>
#include <graphene/chain/hardfork.hpp>
#include <fc/uint128.hpp>

namespace graphene
{
namespace chain
{

share_type cut_fee(share_type a, uint16_t p)
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

account_id_type next_rewardable(const account_object &a, database &d) 
{
   reward_log( "Called next_rewardable with account ${acc}", ("acc",a.name));
   const auto &params = d.get_global_properties().parameters;
   if (a.get_id() < params.root_account || a.get_id() == params.root_account) 
   {
      reward_log( "next_rewardable Return Root");
      return params.root_account;
   }
   else {
      if (params.compression)
      {
         account_object ref = d.get(a.get_id());
         uint8_t current_compression = 0;
         do
         {
            ref = d.get(ref.referrer);
            current_compression++;
         reward_log( "next_rewardable:do_while account ${acc}, compression level ${level}, PV ${pv}, PVCL ${pvcl}", ("acc",ref.name)("level",current_compression)("pv",ref.statistics(d).get_pv(d))("pvcl",params.compression_limit));
         } while (
            ((ref.active_referral_status(d.head_block_time()) == 0) && ( current_compression <= params.compression_levels ) && ( ref.get_id() > params.root_account ) )||
            ((( current_compression <= params.compression_levels ) && ( ref.get_id() > params.root_account )) &&
            (( ref.statistics(d).get_pv(d) <= params.compression_limit && ref.active_referral_status(d.head_block_time())) &&
              (ref.active_referral_status(d.head_block_time()) < params.min_not_compressed )))
         );
         reward_log( "next_rewardable Return ${acc}", ("acc",ref.name));
         if (ref.get_id() < params.root_account) 
         {
            return params.root_account;
         }
         else {
            return ref.get_id();
         }
      }
      else
      {
         reward_log( "next_rewardable Return WO Compression ${acc}", ("acc",a.referrer));
         return a.referrer;
      }
   }
}

share_type account_statistics_object::get_pv(database &d) const
   {
      const auto &params = d.get_global_properties().parameters;
      const uint64_t &referral_statistic_seconds = params.referral_statistic_seconds;
      const uint64_t &seconds = (d.head_block_time() - last_pv_update_date).count() / fc::seconds(1).count();
      const share_type decrease_vol = personal_volume_in_period.value * seconds / referral_statistic_seconds;
      if (personal_volume_in_period > decrease_vol && last_pv_update_date != time_point_sec::min())
      {
         return (personal_volume_in_period - decrease_vol);
      }
      else
      {
         return 0;
      }
   }

void account_statistics_object::update_pv(share_type volume, const account_object &a, database &d) const
{
   reward_log( "Called update_pv for account =${acc}= and volume ${vol}!", ("acc",a.name)("vol",volume));
   const share_type current_pv = get_pv(d);
   d.modify(*this, [&](account_statistics_object &s) {
      if (a.active_referral_status(d.head_block_time()) > 0)
      {
         s.personal_volume_in_period = current_pv + volume;
      }
      else
      {
         s.personal_volume_in_period = current_pv;
      }
      s.last_pv_update_date = d.head_block_time();
   });
}

share_type account_statistics_object::get_nv(database &d) const
   {
      const auto &params = d.get_global_properties().parameters;
      const uint64_t &referral_statistic_seconds = params.referral_statistic_seconds;
      const uint64_t &seconds = (d.head_block_time() - last_nv_update_date).count() / fc::seconds(1).count();
      const share_type decrease_vol = network_volume_in_period.value * seconds / referral_statistic_seconds;
      if (network_volume_in_period > decrease_vol && last_nv_update_date != time_point_sec::min())
      {
         return network_volume_in_period - decrease_vol;
      }
      else
      {
         return 0;
      }
   }

void account_statistics_object::update_nv(share_type volume, uint8_t level, uint16_t max_reward_level, const account_object &a, database &d, std::set<account_id_type> accounts_set) const
{
   reward_log( "Called update_nv for account =${acc}= and volume ${vol}, current level ${level}!", ("acc",a.name)("vol",volume)("level",level));
   const auto &params = d.get_global_properties().parameters;
   const share_type current_nv = get_nv(d);
   d.modify(*this, [&](account_statistics_object &s) {
      if (a.active_referral_status(d.head_block_time()) >= params.min_nv_status)
      {
         s.network_volume_in_period = current_nv + volume;
      }
      else
      {
         s.network_volume_in_period = current_nv;
      }
      s.last_nv_update_date = d.head_block_time();
   });
   // workaround with nv_level_thresholds
   share_type reward_cut = 0;
   if (a.active_referral_status(d.head_block_time()) >= params.min_nv_status && network_volume_in_period >= params.nv_level_threshold_01)
   {
      if (d.head_block_num() < HARDFORK_CORE_1480_BLOCK_NUM || !accounts_set.count(a.id))
      {
         reward_log( "update_nv, account =${acc}= GOT LEADERS REWARD", ("acc",a.name));
         if (network_volume_in_period >= params.nv_level_threshold_08 && params.nv_level_reward_08 > max_reward_level)
         {
            reward_cut = cut_fee(volume, uint16_t(params.nv_level_reward_08 - max_reward_level));
            max_reward_level = params.nv_level_reward_08;
         }
         else
         {
            if (network_volume_in_period >= params.nv_level_threshold_07 && params.nv_level_reward_07 > max_reward_level)
            {
               reward_cut = cut_fee(volume, uint16_t(params.nv_level_reward_07 - max_reward_level));
               max_reward_level = params.nv_level_reward_07;
            }
            else
            {
               if (network_volume_in_period >= params.nv_level_threshold_06 && params.nv_level_reward_06 > max_reward_level)
               {
                  reward_cut = cut_fee(volume, uint16_t(params.nv_level_reward_06 - max_reward_level));
                  max_reward_level = params.nv_level_reward_06;
               }
               else
               {
                  if (network_volume_in_period >= params.nv_level_threshold_05 && params.nv_level_reward_05 > max_reward_level)
                  {
                     reward_cut = cut_fee(volume, uint16_t(params.nv_level_reward_05 - max_reward_level));
                     max_reward_level = params.nv_level_reward_05;
                  }
                  else
                  {
                     if (network_volume_in_period >= params.nv_level_threshold_04 && params.nv_level_reward_04 > max_reward_level)
                     {
                        reward_cut = cut_fee(volume, uint16_t(params.nv_level_reward_04 - max_reward_level));
                        max_reward_level = params.nv_level_reward_04;
                     }
                     else
                     {
                        if (network_volume_in_period >= params.nv_level_threshold_03 && params.nv_level_reward_03 > max_reward_level)
                        {
                           reward_cut = cut_fee(volume, uint16_t(params.nv_level_reward_03 - max_reward_level));
                           max_reward_level = params.nv_level_reward_03;
                        }
                        else
                        {
                           if (network_volume_in_period >= params.nv_level_threshold_02 && params.nv_level_reward_02 > max_reward_level)
                           {
                              reward_cut = cut_fee(volume, uint16_t(params.nv_level_reward_02 - max_reward_level));
                              max_reward_level = params.nv_level_reward_02;
                           }
                           else
                           {
                              if (network_volume_in_period >= params.nv_level_threshold_01 && params.nv_level_reward_01 > max_reward_level)
                              {
                                 reward_cut = cut_fee(volume, uint16_t(params.nv_level_reward_01 - max_reward_level));
                                 max_reward_level = params.nv_level_reward_01;
                              }
                           }
                        }
                     }
                  }
               }
            }
         }
      }
   }
   if (reward_cut > 0)
   {
        accounts_set.insert(a.id);
        if(d.head_block_num() < HARDFORK_CORE_1480_BLOCK_NUM)
        {
            d.deposit_cashback(a, reward_cut, false);
            reward_log( "update_nv, after deposit_cashback account =${acc}= GOT LEADERS REWARD at Level ${level}, amount ${reward_cut}", ("acc",a.name)("level",max_reward_level)("reward_cut",reward_cut));
        }
        else
        {
            d.adjust_balance(account_id_type(GRAPHENE_NULL_ACCOUNT), asset(reward_cut, asset_id_type(0)));
            reward_log( "Withdrawal CWD from market: account =${acc}= GOT LEADERS REWARD at Level ${level}, amount ${reward_cut}", ("acc",a.name)("level",max_reward_level)("reward_cut",reward_cut));
        }
   }
   level = level + 1;
   if (level <= params.nv_levels && a.get_id() > params.root_account)
   {
      const account_object &a_ref = d.get(next_rewardable(a, d));
      a_ref.statistics(d).update_nv(volume, level, max_reward_level, a_ref, d, accounts_set);
   }
}

void account_balance_object::adjust_balance(const asset &delta)
{
   assert(delta.asset_id == asset_type);
   balance += delta.amount;
   if (asset_type == asset_id_type()) // CORE asset
      maintenance_flag = true;
}

void account_statistics_object::process_fees(const account_object &a, database &d) const
{
   if (pending_fees > 0 || pending_vested_fees > 0)
   {
      //       auto pay_out_fees = [&](const account_object& account, share_type core_fee_total, bool require_vesting)
      //       {
      //          // Check the referrer -- if he's no longer a member, pay to the lifetime referrer instead.
      //          // No need to check the registrar; registrars are required to be lifetime members.
      //          if( account.referrer(d).is_basic_account(d.head_block_time()) )
      //             d.modify( account, [](account_object& acc) {
      //                acc.referrer = acc.lifetime_referrer;
      //             });

      //          share_type network_cut = cut_fee(core_fee_total, account.network_fee_percentage);
      //          assert( network_cut <= core_fee_total );

      // #ifndef NDEBUG
      //          const auto& props = d.get_global_properties();

      //          share_type reserveed = cut_fee(network_cut, props.parameters.reserve_percent_of_fee);
      //          share_type accumulated = network_cut - reserveed;
      //          assert( accumulated + reserveed == network_cut );
      // #endif
      //          share_type lifetime_cut = cut_fee(core_fee_total, account.lifetime_referrer_fee_percentage);
      //          share_type referral = core_fee_total - network_cut - lifetime_cut;

      //          d.modify( d.get_core_dynamic_data(), [network_cut](asset_dynamic_data_object& addo) {
      //             addo.accumulated_fees += network_cut;
      //          });

      //          // Potential optimization: Skip some of this math and object lookups by special casing on the account type.
      //          // For example, if the account is a lifetime member, we can skip all this and just deposit the referral to
      //          // it directly.
      //          share_type referrer_cut = cut_fee(referral, account.referrer_rewards_percentage);
      //          share_type registrar_cut = referral - referrer_cut;

      //          d.deposit_cashback(d.get(account.lifetime_referrer), lifetime_cut, require_vesting);
      //          d.deposit_cashback(d.get(account.referrer), referrer_cut, require_vesting);
      //          d.deposit_cashback(d.get(account.registrar), registrar_cut, require_vesting);

      //          assert( referrer_cut + registrar_cut + accumulated + reserveed + lifetime_cut == core_fee_total );
      //       };

      auto pay_out_fees = [&](const account_object &account, share_type core_fee_total, bool require_vesting) {
         const auto &params = d.get_global_properties().parameters;
         share_type network_cut = core_fee_total;
         share_type total_denominator_volume = 0;
         account_object cbk_tmp;
         reward_log( "Called pay_out_fees with account =${acc}=", ("acc",account.name));
         if (params.cashback && account.active_referral_status(d.head_block_time()) > 0)
         {
            cbk_tmp = account;
         }
         else
         {
            cbk_tmp = d.get(next_rewardable(account, d));
         }
         const account_object& ref_01 = cbk_tmp;
         reward_log( "pay_out_fees ref_01 =${acc}=", ("acc",ref_01.name));

         std::set<account_id_type> accounts_set = {};

         share_type ref_01_fee_cut_reward = 0;
         share_type ref_02_fee_cut_reward = 0;
         share_type ref_03_fee_cut_reward = 0;
         share_type ref_04_fee_cut_reward = 0;
         share_type ref_05_fee_cut_reward = 0;
         share_type ref_06_fee_cut_reward = 0;
         share_type ref_07_fee_cut_reward = 0;
         share_type ref_08_fee_cut_reward = 0;
         share_type ref_01_fee_cut_withdraw = 0;
         share_type ref_02_fee_cut_withdraw = 0;
         share_type ref_03_fee_cut_withdraw = 0;
         share_type ref_04_fee_cut_withdraw = 0;
         share_type ref_05_fee_cut_withdraw = 0;
         share_type ref_06_fee_cut_withdraw = 0;
         share_type ref_07_fee_cut_withdraw = 0;
         share_type ref_08_fee_cut_withdraw = 0;

         if ( (account.id != ref_01.id || params.cashback) && (d.head_block_num() < HARDFORK_CORE_1480_BLOCK_NUM || !accounts_set.count(ref_01.id)) )
         {
            if (params.ref_01_percent_of_fee > 0 && network_cut > 0 && ref_01.referral_levels >= 1)
            {
               reward_log( "pay_out_fees ref_01 =${acc}= GOT reward!", ("acc",ref_01.name));
               share_type ref_01_fee_cut = cut_fee(core_fee_total, params.ref_01_percent_of_fee);
               if (params.denominator)
               {
                  if (ref_01.status_denominator < GRAPHENE_100_PERCENT)
                  {
                     total_denominator_volume = total_denominator_volume + cut_fee(ref_01_fee_cut, GRAPHENE_100_PERCENT - ref_01.status_denominator);
                     ref_01_fee_cut = cut_fee(ref_01_fee_cut, ref_01.status_denominator);
                  }
                  if (ref_01.active_referral_status(d.head_block_time()) == params.denominator_bonus_level && total_denominator_volume > 0)
                  {
                     ref_01_fee_cut = ref_01_fee_cut + total_denominator_volume;
                     total_denominator_volume = 0;
                  }
               }

               if(d.head_block_num() < HARDFORK_CORE_1480_BLOCK_NUM)
               {
                    d.deposit_cashback(d.get(ref_01.get_id()), ref_01_fee_cut, require_vesting);
               } 
               else
               {
                    ref_01_fee_cut_withdraw = cut_fee(ref_01_fee_cut, GRAPHENE_DEFAULT_BURN_PERCENT_OF_FEE);
                    ref_01_fee_cut_reward = cut_fee(ref_01_fee_cut, GRAPHENE_100_PERCENT - GRAPHENE_DEFAULT_BURN_PERCENT_OF_FEE);
                    d.adjust_balance(account_id_type(GRAPHENE_NULL_ACCOUNT), asset(ref_01_fee_cut_withdraw, asset_id_type(0)));
                    d.deposit_cashback(d.get(ref_01.get_id()), ref_01_fee_cut_reward, require_vesting);
               }
               // GR_REWARD
               const auto &dgpo = d.get_dynamic_global_properties();
               if (  dgpo.current_gr_interval == 2  ||
                     dgpo.current_gr_interval == 4  ||
                     dgpo.current_gr_interval == 6  ||
                     dgpo.current_gr_interval == 9  ||
                     dgpo.current_gr_interval == 11 ||
                     dgpo.current_gr_interval == 13
                  )
                  {
                     d.modify(d.get(ref_01.statistics), [&](account_statistics_object &s) {
                        s.current_period_gr += ref_01_fee_cut_reward;
                     });

                     if (ref_01.gr_team.valid()) {
                        d.modify(d.get(*ref_01.gr_team), [&](gr_team_object &t) {
                           if (dgpo.current_gr_interval == 2) {
                              t.gr_interval_2_volume += ref_01_fee_cut_reward;
                              t.first_half_volume += ref_01_fee_cut_reward;
                              t.total_volume += ref_01_fee_cut_reward;
                           }
                           if (dgpo.current_gr_interval == 4) {
                              t.gr_interval_4_volume += ref_01_fee_cut_reward;
                              t.first_half_volume += ref_01_fee_cut_reward;
                              t.total_volume += ref_01_fee_cut_reward;
                           }
                           if (dgpo.current_gr_interval == 6) {
                              t.gr_interval_6_volume += ref_01_fee_cut_reward;
                              t.first_half_volume += ref_01_fee_cut_reward;
                              t.total_volume += ref_01_fee_cut_reward;
                           }
                           if (dgpo.current_gr_interval == 9) {
                              t.gr_interval_9_volume += ref_01_fee_cut_reward;
                              t.second_half_volume += ref_01_fee_cut_reward;
                              t.total_volume += ref_01_fee_cut_reward;
                           }
                           if (dgpo.current_gr_interval == 11) {
                              t.gr_interval_11_volume += ref_01_fee_cut_reward;
                              t.second_half_volume += ref_01_fee_cut_reward;
                              t.total_volume += ref_01_fee_cut_reward;
                           }
                           if (dgpo.current_gr_interval == 13) {
                              t.gr_interval_13_volume += ref_01_fee_cut_reward;
                              t.second_half_volume += ref_01_fee_cut_reward;
                              t.total_volume += ref_01_fee_cut_reward;
                           }
                        });       
                     }
                  }
               reward_log( "pay_out_fees ref_01 =${acc}= after deposit_cashback ${cbk}!", ("acc",ref_01.name)("cbk",ref_01_fee_cut_reward));
               network_cut = network_cut - ref_01_fee_cut;
               accounts_set.insert(ref_01.id);
            }
            else {
               reward_log( "pay_out_fees ref_01 =${acc}= MISS reward!", ("acc",ref_01.name));
            }
            const account_object &ref_02 = d.get(next_rewardable(ref_01,d));
            reward_log( "pay_out_fees ref_02 =${acc}=", ("acc",ref_02.name));

            if ( (ref_01.id != ref_02.id) && (d.head_block_num() < HARDFORK_CORE_1480_BLOCK_NUM || !accounts_set.count(ref_02.id)) )
            {
               if (params.ref_02_percent_of_fee > 0 && network_cut > 0 && ref_02.referral_levels >= 2)
               {
                  reward_log( "pay_out_fees ref_02 =${acc}= GOT reward!", ("acc",ref_02.name));
                  share_type ref_02_fee_cut = cut_fee(core_fee_total, params.ref_02_percent_of_fee);
                  if (params.denominator)
                  {
                     if (ref_02.status_denominator < GRAPHENE_100_PERCENT)
                     {
                        total_denominator_volume = total_denominator_volume + cut_fee(ref_02_fee_cut, GRAPHENE_100_PERCENT - ref_02.status_denominator);
                        ref_02_fee_cut = cut_fee(ref_02_fee_cut, ref_02.status_denominator);
                     }
                     if (ref_02.active_referral_status(d.head_block_time()) == params.denominator_bonus_level && total_denominator_volume > 0)
                     {
                        ref_02_fee_cut = ref_02_fee_cut + total_denominator_volume;
                        total_denominator_volume = 0;
                     }
                  }
                  if(d.head_block_num() < HARDFORK_CORE_1480_BLOCK_NUM)
                  {
                     d.deposit_cashback(ref_02, ref_02_fee_cut, require_vesting);
                  } 
                  else
                  {
                     ref_02_fee_cut_withdraw = cut_fee(ref_02_fee_cut, GRAPHENE_DEFAULT_BURN_PERCENT_OF_FEE);
                     ref_02_fee_cut_reward = cut_fee(ref_02_fee_cut, GRAPHENE_100_PERCENT - GRAPHENE_DEFAULT_BURN_PERCENT_OF_FEE);
                     d.adjust_balance(account_id_type(GRAPHENE_NULL_ACCOUNT), asset(ref_02_fee_cut_withdraw, asset_id_type(0)));
                     d.deposit_cashback(d.get(ref_02.get_id()), ref_02_fee_cut_reward, require_vesting);
                  }
                  reward_log( "pay_out_fees ref_02 =${acc}= after deposit_cashback ${cbk}!", ("acc",ref_02.name)("cbk",ref_02_fee_cut_reward));
                  network_cut = network_cut - ref_02_fee_cut;
                  accounts_set.insert(ref_02.id);
               }
               else {
                  reward_log( "pay_out_fees ref_02 =${acc}= MISS reward!", ("acc",ref_02.name));
               }
               const account_object &ref_03 = d.get(next_rewardable(ref_02, d));
               reward_log( "pay_out_fees ref_03 =${acc}=", ("acc",ref_03.name));

               if ( (ref_02.id != ref_03.id) && (d.head_block_num() < HARDFORK_CORE_1480_BLOCK_NUM || !accounts_set.count(ref_03.id)) )
               {
                  if (params.ref_03_percent_of_fee > 0 && network_cut > 0 && ref_03.referral_levels >= 3)
                  {
                     reward_log( "pay_out_fees ref_03 =${acc}= GOT reward!", ("acc",ref_03.name));
                     share_type ref_03_fee_cut = cut_fee(core_fee_total, params.ref_03_percent_of_fee);
                     if (params.denominator)
                     {
                        if (ref_03.status_denominator < GRAPHENE_100_PERCENT)
                        {
                           total_denominator_volume = total_denominator_volume + cut_fee(ref_03_fee_cut, GRAPHENE_100_PERCENT - ref_03.status_denominator);
                           ref_03_fee_cut = cut_fee(ref_03_fee_cut, ref_03.status_denominator);
                        }
                        if (ref_03.active_referral_status(d.head_block_time()) == params.denominator_bonus_level && total_denominator_volume > 0)
                        {
                           ref_03_fee_cut = ref_03_fee_cut + total_denominator_volume;
                           total_denominator_volume = 0;
                        }
                     }
                     if(d.head_block_num() < HARDFORK_CORE_1480_BLOCK_NUM)
                     {
                        d.deposit_cashback(ref_03, ref_03_fee_cut, require_vesting);
                     } 
                     else
                     {
                        ref_03_fee_cut_withdraw = cut_fee(ref_03_fee_cut, GRAPHENE_DEFAULT_BURN_PERCENT_OF_FEE);
                        ref_03_fee_cut_reward = cut_fee(ref_03_fee_cut, GRAPHENE_100_PERCENT - GRAPHENE_DEFAULT_BURN_PERCENT_OF_FEE);
                        d.adjust_balance(account_id_type(GRAPHENE_NULL_ACCOUNT), asset(ref_03_fee_cut_withdraw, asset_id_type(0)));
                        d.deposit_cashback(d.get(ref_03.get_id()), ref_03_fee_cut_reward, require_vesting);
                     }
                     reward_log( "pay_out_fees ref_03 =${acc}= after deposit_cashback ${cbk}!", ("acc",ref_03.name)("cbk",ref_03_fee_cut_reward));
                     network_cut = network_cut - ref_03_fee_cut;
                     accounts_set.insert(ref_03.id);
                  }
                  else {
                     reward_log( "pay_out_fees ref_03 =${acc}= MISS reward!", ("acc",ref_03.name));
                  }
                  const account_object &ref_04 = d.get(next_rewardable(ref_03, d));
                  reward_log( "pay_out_fees ref_04 =${acc}=", ("acc",ref_04.name));

                  if ( (ref_03.id != ref_04.id) && (d.head_block_num() < HARDFORK_CORE_1480_BLOCK_NUM || !accounts_set.count(ref_04.id)) )
                  {
                     if (params.ref_04_percent_of_fee > 0 && network_cut > 0 && ref_04.referral_levels >= 4)
                     {
                        reward_log( "pay_out_fees ref_04 =${acc}= GOT reward!", ("acc",ref_04.name));
                        share_type ref_04_fee_cut = cut_fee(core_fee_total, params.ref_04_percent_of_fee);
                        if (params.denominator)
                        {
                           if (ref_04.status_denominator < GRAPHENE_100_PERCENT)
                           {
                              total_denominator_volume = total_denominator_volume + cut_fee(ref_04_fee_cut, GRAPHENE_100_PERCENT - ref_04.status_denominator);
                              ref_04_fee_cut = cut_fee(ref_04_fee_cut, ref_04.status_denominator);
                           }
                           if (ref_04.active_referral_status(d.head_block_time()) == params.denominator_bonus_level && total_denominator_volume > 0)
                           {
                              ref_04_fee_cut = ref_04_fee_cut + total_denominator_volume;
                              total_denominator_volume = 0;
                           }
                        }
                        if(d.head_block_num() < HARDFORK_CORE_1480_BLOCK_NUM)
                        {
                           d.deposit_cashback(ref_04, ref_04_fee_cut, require_vesting);
                        } 
                        else
                        {
                           ref_04_fee_cut_withdraw = cut_fee(ref_04_fee_cut, GRAPHENE_DEFAULT_BURN_PERCENT_OF_FEE);
                           ref_04_fee_cut_reward = cut_fee(ref_04_fee_cut, GRAPHENE_100_PERCENT - GRAPHENE_DEFAULT_BURN_PERCENT_OF_FEE);
                           d.adjust_balance(account_id_type(GRAPHENE_NULL_ACCOUNT), asset(ref_04_fee_cut_withdraw, asset_id_type(0)));
                           d.deposit_cashback(d.get(ref_04.get_id()), ref_04_fee_cut_reward, require_vesting);
                        }
                        reward_log( "pay_out_fees ref_04 =${acc}= after deposit_cashback ${cbk}!", ("acc",ref_04.name)("cbk",ref_04_fee_cut_reward));
                        network_cut = network_cut - ref_04_fee_cut;
                        accounts_set.insert(ref_04.id);
                     }
                     else {
                        reward_log( "pay_out_fees ref_04 =${acc}= MISS reward!", ("acc",ref_04.name));
                     }
                     const account_object &ref_05 = d.get(next_rewardable(ref_04, d));
                     reward_log( "pay_out_fees ref_05 =${acc}=", ("acc",ref_05.name));

                     if ( (ref_04.id != ref_05.id) && (d.head_block_num() < HARDFORK_CORE_1480_BLOCK_NUM || !accounts_set.count(ref_05.id)) )
                     {
                        if (params.ref_05_percent_of_fee > 0 && network_cut > 0 && ref_05.referral_levels >= 5)
                        {
                           reward_log( "pay_out_fees ref_05 =${acc}= GOT reward!", ("acc",ref_05.name));
                           share_type ref_05_fee_cut = cut_fee(core_fee_total, params.ref_05_percent_of_fee);
                           if (params.denominator)
                           {
                              if (ref_05.status_denominator < GRAPHENE_100_PERCENT)
                              {
                                 total_denominator_volume = total_denominator_volume + cut_fee(ref_05_fee_cut, GRAPHENE_100_PERCENT - ref_05.status_denominator);
                                 ref_05_fee_cut = cut_fee(ref_05_fee_cut, ref_05.status_denominator);
                              }
                              if (ref_05.active_referral_status(d.head_block_time()) == params.denominator_bonus_level && total_denominator_volume > 0)
                              {
                                 ref_05_fee_cut = ref_05_fee_cut + total_denominator_volume;
                                 total_denominator_volume = 0;
                              }
                           }
                           if(d.head_block_num() < HARDFORK_CORE_1480_BLOCK_NUM)
                           {
                              d.deposit_cashback(ref_05, ref_05_fee_cut, require_vesting);
                           } 
                           else
                           {
                              ref_05_fee_cut_withdraw = cut_fee(ref_05_fee_cut, GRAPHENE_DEFAULT_BURN_PERCENT_OF_FEE);
                              ref_05_fee_cut_reward = cut_fee(ref_05_fee_cut, GRAPHENE_100_PERCENT - GRAPHENE_DEFAULT_BURN_PERCENT_OF_FEE);
                              d.adjust_balance(account_id_type(GRAPHENE_NULL_ACCOUNT), asset(ref_05_fee_cut_withdraw, asset_id_type(0)));
                              d.deposit_cashback(d.get(ref_05.get_id()), ref_05_fee_cut_reward, require_vesting);
                           }
                           reward_log( "pay_out_fees ref_05 =${acc}= after deposit_cashback ${cbk}!", ("acc",ref_05.name)("cbk",ref_05_fee_cut_reward));
                           network_cut = network_cut - ref_05_fee_cut;
                           accounts_set.insert(ref_05.id);
                        }
                        else {
                           reward_log( "pay_out_fees ref_05 =${acc}= MISS reward!", ("acc",ref_05.name));
                        }
                        const account_object &ref_06 = d.get(next_rewardable(ref_05, d));
                        reward_log( "pay_out_fees ref_06 =${acc}=", ("acc",ref_06.name));

                        if ( (ref_05.id != ref_06.id) && (d.head_block_num() < HARDFORK_CORE_1480_BLOCK_NUM || !accounts_set.count(ref_06.id)) )
                        {
                           if (params.ref_06_percent_of_fee > 0 && network_cut > 0 && ref_06.referral_levels >= 6)
                           {
                              reward_log( "pay_out_fees ref_06 =${acc}= GOT reward!", ("acc",ref_06.name));
                              share_type ref_06_fee_cut = cut_fee(core_fee_total, params.ref_06_percent_of_fee);
                              if (params.denominator)
                              {
                                 if (ref_06.status_denominator < GRAPHENE_100_PERCENT)
                                 {
                                    total_denominator_volume = total_denominator_volume + cut_fee(ref_06_fee_cut, GRAPHENE_100_PERCENT - ref_06.status_denominator);
                                    ref_06_fee_cut = cut_fee(ref_06_fee_cut, ref_06.status_denominator);
                                 }
                                 if (ref_06.active_referral_status(d.head_block_time()) == params.denominator_bonus_level && total_denominator_volume > 0)
                                 {
                                    ref_06_fee_cut = ref_06_fee_cut + total_denominator_volume;
                                    total_denominator_volume = 0;
                                 }
                              }
                              if(d.head_block_num() < HARDFORK_CORE_1480_BLOCK_NUM)
                              {
                                 d.deposit_cashback(ref_06, ref_06_fee_cut, require_vesting);
                              } 
                              else
                              {
                                 ref_06_fee_cut_withdraw = cut_fee(ref_06_fee_cut, GRAPHENE_DEFAULT_BURN_PERCENT_OF_FEE);
                                 ref_06_fee_cut_reward = cut_fee(ref_06_fee_cut, GRAPHENE_100_PERCENT - GRAPHENE_DEFAULT_BURN_PERCENT_OF_FEE);
                                 d.adjust_balance(account_id_type(GRAPHENE_NULL_ACCOUNT), asset(ref_06_fee_cut_withdraw, asset_id_type(0)));
                                 d.deposit_cashback(d.get(ref_06.get_id()), ref_06_fee_cut_reward, require_vesting);
                              }
                              reward_log( "pay_out_fees ref_06 =${acc}= after deposit_cashback ${cbk}!", ("acc",ref_06.name)("cbk",ref_06_fee_cut_reward));
                              network_cut = network_cut - ref_06_fee_cut;
                              accounts_set.insert(ref_06.id);
                           }
                           else {
                              reward_log( "pay_out_fees ref_06 =${acc}= MISS reward!", ("acc",ref_06.name));
                           }
                           const account_object &ref_07 = d.get(next_rewardable(ref_06, d));
                           reward_log( "pay_out_fees ref_07 =${acc}=", ("acc",ref_07.name));

                           if ( (ref_06.id != ref_07.id) && (d.head_block_num() < HARDFORK_CORE_1480_BLOCK_NUM || !accounts_set.count(ref_07.id)) )
                           {
                              if (params.ref_07_percent_of_fee > 0 && network_cut > 0 && ref_07.referral_levels >= 7)
                              {
                                 reward_log( "pay_out_fees ref_07 =${acc}= GOT reward!", ("acc",ref_07.name));
                                 share_type ref_07_fee_cut = cut_fee(core_fee_total, params.ref_07_percent_of_fee);
                                 if (params.denominator)
                                 {
                                    if (ref_07.status_denominator < GRAPHENE_100_PERCENT)
                                    {
                                       total_denominator_volume = total_denominator_volume + cut_fee(ref_07_fee_cut, GRAPHENE_100_PERCENT - ref_07.status_denominator);
                                       ref_07_fee_cut = cut_fee(ref_07_fee_cut, ref_07.status_denominator);
                                    }
                                    if (ref_07.active_referral_status(d.head_block_time()) == params.denominator_bonus_level && total_denominator_volume > 0)
                                    {
                                       ref_07_fee_cut = ref_07_fee_cut + total_denominator_volume;
                                       total_denominator_volume = 0;
                                    }
                                 }
                                 if(d.head_block_num() < HARDFORK_CORE_1480_BLOCK_NUM)
                                 {
                                    d.deposit_cashback(ref_07, ref_07_fee_cut, require_vesting);
                                 } 
                                 else
                                 {
                                    ref_07_fee_cut_withdraw = cut_fee(ref_07_fee_cut, GRAPHENE_DEFAULT_BURN_PERCENT_OF_FEE);
                                    ref_07_fee_cut_reward = cut_fee(ref_07_fee_cut, GRAPHENE_100_PERCENT - GRAPHENE_DEFAULT_BURN_PERCENT_OF_FEE);
                                    d.adjust_balance(account_id_type(GRAPHENE_NULL_ACCOUNT), asset(ref_07_fee_cut_withdraw, asset_id_type(0)));
                                    d.deposit_cashback(d.get(ref_07.get_id()), ref_07_fee_cut_reward, require_vesting);
                                 }
                                 reward_log( "pay_out_fees ref_07 =${acc}= after deposit_cashback ${cbk}!", ("acc",ref_07.name)("cbk",ref_07_fee_cut_reward));
                                 network_cut = network_cut - ref_07_fee_cut;
                                 accounts_set.insert(ref_07.id);
                              }
                              else {
                                 reward_log( "pay_out_fees ref_07 =${acc}= MISS reward!", ("acc",ref_07.name));
                              }                              
                              const account_object &ref_08 = d.get(next_rewardable(ref_07, d));
                              reward_log( "pay_out_fees ref_08 =${acc}=", ("acc",ref_08.name));
                              if ( (ref_07.id != ref_08.id) && (d.head_block_num() < HARDFORK_CORE_1480_BLOCK_NUM || !accounts_set.count(ref_08.id)) )
                              {
                                 if (params.ref_08_percent_of_fee > 0 && network_cut > 0 && ref_08.referral_levels >= 8)
                                 {
                                    reward_log( "pay_out_fees ref_08 =${acc}= GOT reward!", ("acc",ref_08.name));
                                    share_type ref_08_fee_cut = cut_fee(core_fee_total, params.ref_08_percent_of_fee);
                                    if (params.denominator)
                                    {
                                       if (ref_08.status_denominator < GRAPHENE_100_PERCENT)
                                       {
                                          total_denominator_volume = total_denominator_volume + cut_fee(ref_08_fee_cut, GRAPHENE_100_PERCENT - ref_08.status_denominator);
                                          ref_08_fee_cut = cut_fee(ref_08_fee_cut, ref_08.status_denominator);
                                       }
                                       if (ref_08.active_referral_status(d.head_block_time()) == params.denominator_bonus_level && total_denominator_volume > 0)
                                       {
                                          ref_08_fee_cut = ref_08_fee_cut + total_denominator_volume;
                                          total_denominator_volume = 0;
                                       }
                                    }
                                    if(d.head_block_num() < HARDFORK_CORE_1480_BLOCK_NUM)
                                    {
                                       d.deposit_cashback(ref_08, ref_08_fee_cut, require_vesting);
                                    } 
                                    else
                                    {
                                       ref_08_fee_cut_withdraw = cut_fee(ref_08_fee_cut, GRAPHENE_DEFAULT_BURN_PERCENT_OF_FEE);
                                       ref_08_fee_cut_reward = cut_fee(ref_08_fee_cut, GRAPHENE_100_PERCENT - GRAPHENE_DEFAULT_BURN_PERCENT_OF_FEE);
                                       d.adjust_balance(account_id_type(GRAPHENE_NULL_ACCOUNT), asset(ref_08_fee_cut_withdraw, asset_id_type(0)));
                                       d.deposit_cashback(d.get(ref_08.get_id()), ref_08_fee_cut_reward, require_vesting);
                                    }
                                    reward_log( "pay_out_fees ref_08 =${acc}= after deposit_cashback ${cbk}!", ("acc",ref_08.name)("cbk",ref_08_fee_cut_reward));
                                    network_cut = network_cut - ref_08_fee_cut;
                                    accounts_set.insert(ref_08.id);
                                 }
                                 else {
                                    reward_log( "pay_out_fees ref_08 =${acc}= MISS reward!", ("acc",ref_08.name));
                                 }                                  
                              } // ref_08
                           }    // ref_07
                        }       // ref_06
                     }          // ref_05
                  }             // ref_04
               }                // ref_03
            }                   // ref_02
         }                      // ref_01
         d.modify(asset_dynamic_data_id_type()(d), [network_cut](asset_dynamic_data_object &addo) {
            addo.accumulated_fees += network_cut;
         });
      };
      if (pending_fees > 0) {
         reward_log( "Call pay_out_fees pending_fees ${cbk}!", ("cbk",pending_fees));     
         pay_out_fees(a, pending_fees, true);
      }
      if (pending_vested_fees > 0) {
         reward_log( "Call pay_out_fees pending_vested_fees ${cbk}!", ("cbk",pending_vested_fees));     
         pay_out_fees(a, pending_vested_fees, false);
      }

      d.modify(*this, [&](account_statistics_object &s) {
         s.lifetime_fees_paid += pending_fees + pending_vested_fees;
         s.pending_fees = 0;
         s.pending_vested_fees = 0;
      });
   }
}

void account_statistics_object::pay_fee(share_type core_fee, share_type cashback_vesting_threshold)
{
   // if( core_fee > cashback_vesting_threshold )
   //    pending_fees += core_fee;
   // else
   //    pending_vested_fees += core_fee;
   pending_vested_fees += core_fee;
}

set<account_id_type> account_member_index::get_account_members(const account_object &a) const
{
   set<account_id_type> result;
   for (auto auth : a.owner.account_auths)
      result.insert(auth.first);
   for (auto auth : a.active.account_auths)
      result.insert(auth.first);
   return result;
}
set<public_key_type, pubkey_comparator> account_member_index::get_key_members(const account_object &a) const
{
   set<public_key_type, pubkey_comparator> result;
   for (auto auth : a.owner.key_auths)
      result.insert(auth.first);
   for (auto auth : a.active.key_auths)
      result.insert(auth.first);
   result.insert(a.options.memo_key);
   return result;
}
set<address> account_member_index::get_address_members(const account_object &a) const
{
   set<address> result;
   for (auto auth : a.owner.address_auths)
      result.insert(auth.first);
   for (auto auth : a.active.address_auths)
      result.insert(auth.first);
   result.insert(a.options.memo_key);
   return result;
}

void account_member_index::object_inserted(const object &obj)
{
   assert(dynamic_cast<const account_object *>(&obj)); // for debug only
   const account_object &a = static_cast<const account_object &>(obj);

   auto account_members = get_account_members(a);
   for (auto item : account_members)
      account_to_account_memberships[item].insert(obj.id);

   auto key_members = get_key_members(a);
   for (auto item : key_members)
      account_to_key_memberships[item].insert(obj.id);

   auto address_members = get_address_members(a);
   for (auto item : address_members)
      account_to_address_memberships[item].insert(obj.id);
}

void account_member_index::object_removed(const object &obj)
{
   assert(dynamic_cast<const account_object *>(&obj)); // for debug only
   const account_object &a = static_cast<const account_object &>(obj);

   auto key_members = get_key_members(a);
   for (auto item : key_members)
      account_to_key_memberships[item].erase(obj.id);

   auto address_members = get_address_members(a);
   for (auto item : address_members)
      account_to_address_memberships[item].erase(obj.id);

   auto account_members = get_account_members(a);
   for (auto item : account_members)
      account_to_account_memberships[item].erase(obj.id);
}

void account_member_index::about_to_modify(const object &before)
{
   before_key_members.clear();
   before_account_members.clear();
   assert(dynamic_cast<const account_object *>(&before)); // for debug only
   const account_object &a = static_cast<const account_object &>(before);
   before_key_members = get_key_members(a);
   before_address_members = get_address_members(a);
   before_account_members = get_account_members(a);
}

void account_member_index::object_modified(const object &after)
{
   assert(dynamic_cast<const account_object *>(&after)); // for debug only
   const account_object &a = static_cast<const account_object &>(after);

   {
      set<account_id_type> after_account_members = get_account_members(a);
      vector<account_id_type> removed;
      removed.reserve(before_account_members.size());
      std::set_difference(before_account_members.begin(), before_account_members.end(),
                          after_account_members.begin(), after_account_members.end(),
                          std::inserter(removed, removed.end()));

      for (auto itr = removed.begin(); itr != removed.end(); ++itr)
         account_to_account_memberships[*itr].erase(after.id);

      vector<object_id_type> added;
      added.reserve(after_account_members.size());
      std::set_difference(after_account_members.begin(), after_account_members.end(),
                          before_account_members.begin(), before_account_members.end(),
                          std::inserter(added, added.end()));

      for (auto itr = added.begin(); itr != added.end(); ++itr)
         account_to_account_memberships[*itr].insert(after.id);
   }

   {
      set<public_key_type, pubkey_comparator> after_key_members = get_key_members(a);

      vector<public_key_type> removed;
      removed.reserve(before_key_members.size());
      std::set_difference(before_key_members.begin(), before_key_members.end(),
                          after_key_members.begin(), after_key_members.end(),
                          std::inserter(removed, removed.end()));

      for (auto itr = removed.begin(); itr != removed.end(); ++itr)
         account_to_key_memberships[*itr].erase(after.id);

      vector<public_key_type> added;
      added.reserve(after_key_members.size());
      std::set_difference(after_key_members.begin(), after_key_members.end(),
                          before_key_members.begin(), before_key_members.end(),
                          std::inserter(added, added.end()));

      for (auto itr = added.begin(); itr != added.end(); ++itr)
         account_to_key_memberships[*itr].insert(after.id);
   }

   {
      set<address> after_address_members = get_address_members(a);

      vector<address> removed;
      removed.reserve(before_address_members.size());
      std::set_difference(before_address_members.begin(), before_address_members.end(),
                          after_address_members.begin(), after_address_members.end(),
                          std::inserter(removed, removed.end()));

      for (auto itr = removed.begin(); itr != removed.end(); ++itr)
         account_to_address_memberships[*itr].erase(after.id);

      vector<address> added;
      added.reserve(after_address_members.size());
      std::set_difference(after_address_members.begin(), after_address_members.end(),
                          before_address_members.begin(), before_address_members.end(),
                          std::inserter(added, added.end()));

      for (auto itr = added.begin(); itr != added.end(); ++itr)
         account_to_address_memberships[*itr].insert(after.id);
   }
}

void account_referrer_index::object_inserted(const object &obj)
{
}
void account_referrer_index::object_removed(const object &obj)
{
}
void account_referrer_index::about_to_modify(const object &before)
{
}
void account_referrer_index::object_modified(const object &after)
{
}

const uint8_t balances_by_account_index::bits = 20;
const uint64_t balances_by_account_index::mask = (1ULL << balances_by_account_index::bits) - 1;

void balances_by_account_index::object_inserted(const object &obj)
{
   const auto &abo = dynamic_cast<const account_balance_object &>(obj);
   while (balances.size() < (abo.owner.instance.value >> bits) + 1)
   {
      balances.reserve((abo.owner.instance.value >> bits) + 1);
      balances.resize(balances.size() + 1);
      balances.back().resize(1ULL << bits);
   }
   balances[abo.owner.instance.value >> bits][abo.owner.instance.value & mask][abo.asset_type] = &abo;
}

void balances_by_account_index::object_removed(const object &obj)
{
   const auto &abo = dynamic_cast<const account_balance_object &>(obj);
   if (balances.size() < (abo.owner.instance.value >> bits) + 1)
      return;
   balances[abo.owner.instance.value >> bits][abo.owner.instance.value & mask].erase(abo.asset_type);
}

void balances_by_account_index::about_to_modify(const object &before)
{
   ids_being_modified.emplace(before.id);
}

void balances_by_account_index::object_modified(const object &after)
{
   FC_ASSERT(ids_being_modified.top() == after.id, "Modification of ID is not supported!");
   ids_being_modified.pop();
}

const map<asset_id_type, const account_balance_object *> &balances_by_account_index::get_account_balances(const account_id_type &acct) const
{
   static const map<asset_id_type, const account_balance_object *> _empty;

   if (balances.size() < (acct.instance.value >> bits) + 1)
      return _empty;
   return balances[acct.instance.value >> bits][acct.instance.value & mask];
}

const account_balance_object *balances_by_account_index::get_account_balance(const account_id_type &acct, const asset_id_type &asset) const
{
   if (balances.size() < (acct.instance.value >> bits) + 1)
      return nullptr;
   const auto &mine = balances[acct.instance.value >> bits][acct.instance.value & mask];
   const auto itr = mine.find(asset);
   if (mine.end() == itr)
      return nullptr;
   return itr->second;
}

} // namespace chain
} // namespace graphene
