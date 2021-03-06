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
#pragma once
#include <fc/uint128.hpp>

#include <graphene/chain/protocol/chain_parameters.hpp>
#include <graphene/chain/protocol/gamezone_parameters.hpp>
#include <graphene/chain/protocol/staking_parameters.hpp>
#include <graphene/chain/protocol/greatrace_parameters.hpp>
#include <graphene/chain/protocol/types.hpp>
#include <graphene/chain/database.hpp>
#include <graphene/db/object.hpp>

namespace graphene { namespace chain {

   /**
    * @class global_property_object
    * @brief Maintains global state information (committee_member list, current fees)
    * @ingroup object
    * @ingroup implementation
    *
    * This is an implementation detail. The values here are set by committee_members to tune the blockchain parameters.
    */
   class global_property_object : public graphene::db::abstract_object<global_property_object>
   {
      public:
         static const uint8_t space_id = implementation_ids;
         static const uint8_t type_id  = impl_global_property_object_type;

         staking_chain_parameters   staking_parameters;
         gamezone_chain_parameters  gamezone_parameters;
         greatrace_chain_parameters greatrace_parameters;
         chain_parameters           parameters;
         optional<chain_parameters> pending_parameters;

         uint32_t                           next_available_vote_id = 0;
         vector<committee_member_id_type>   active_committee_members; // updated once per maintenance interval
         flat_set<witness_id_type>          active_witnesses; // updated once per maintenance interval
         // n.b. witness scheduling is done by witness_schedule object
   };

   /**
    * @class dynamic_global_property_object
    * @brief Maintains global state information (committee_member list, current fees)
    * @ingroup object
    * @ingroup implementation
    *
    * This is an implementation detail. The values here are calculated during normal chain operations and reflect the
    * current values of global blockchain properties.
    */
   class dynamic_global_property_object : public abstract_object<dynamic_global_property_object>
   {
      public:
         static const uint8_t space_id = implementation_ids;
         static const uint8_t type_id  = impl_dynamic_global_property_object_type;

         uint32_t          head_block_number = 0;
         block_id_type     head_block_id;
         time_point_sec    time;
         witness_id_type   current_witness;
         time_point_sec    next_maintenance_time;
         time_point_sec    next_monthly_maintenance_time;
         time_point_sec    last_budget_time;
         share_type        witness_budget;
         uint32_t          accounts_registered_this_interval = 0;
         // GCWD Price parameters
         share_type        gcwd_price = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(13000));
         // PoC parameters
         time_point_sec    next_poc_vote_time = fc::time_point_sec(1601078400);         
         time_point_sec    end_poc_vote_time = fc::time_point_sec();
         bool		         poc_vote_is_active = false;
         uint64_t          poc3_percent  = (45*GRAPHENE_1_PERCENT);          
         uint64_t          poc6_percent  = (78*GRAPHENE_1_PERCENT); 
         uint64_t          poc12_percent = (97*GRAPHENE_1_PERCENT); 
         // Great Race parameters
         uint8_t           current_gr_interval = 0; // from 0 to 14
         time_point_sec    next_gr_interval_time = fc::time_point_sec(1623088800); // GR start time 
         time_point_sec    gr_bet_interval_time = fc::time_point_sec(1623088800); // GR start time
         time_point_sec    end_gr_vote_time = fc::time_point_sec();
         bool              gr_vote_is_active = false;

         share_type        gr_iron_volume = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(4500));
         share_type        gr_bronze_volume = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(9000));
         share_type        gr_silver_volume = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(27000));
         share_type        gr_gold_volume = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(50000));
         share_type        gr_platinum_volume = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(90000));
         share_type        gr_diamond_volume = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(125000));
         share_type        gr_master_volume = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(250000));

         share_type        gr_iron_reward = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(2000));
         share_type        gr_bronze_reward = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(4000));
         share_type        gr_silver_reward = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(12000));
         share_type        gr_gold_reward = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(30000));
         share_type        gr_platinum_reward = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(60000));
         share_type        gr_diamond_reward = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(100000));
         share_type        gr_elite_reward = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(250000));
         share_type        gr_master_reward = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(250000));

         share_type        gr_top3_reward = (GRAPHENE_BLOCKCHAIN_PRECISION * int64_t(10000));

         /**
          *  Every time a block is missed this increases by
          *  RECENTLY_MISSED_COUNT_INCREMENT,
          *  every time a block is found it decreases by
          *  RECENTLY_MISSED_COUNT_DECREMENT.  It is
          *  never less than 0.
          *
          *  If the recently_missed_count hits 2*UNDO_HISTORY then no new blocks may be pushed.
          */
         uint32_t          recently_missed_count = 0;

         /**
          * The current absolute slot number.  Equal to the total
          * number of slots since genesis.  Also equal to the total
          * number of missed slots plus head_block_number.
          */
         uint64_t                current_aslot = 0;

         /**
          * used to compute witness participation.
          */
         fc::uint128_t recent_slots_filled;

         /**
          * dynamic_flags specifies chain state properties that can be
          * expressed in one bit.
          */
         uint32_t dynamic_flags = 0;

         uint32_t last_irreversible_block_num = 0;

         enum dynamic_flag_bits
         {
            /**
             * If maintenance_flag is set, then the head block is a
             * maintenance block.  This means
             * get_time_slot(1) - head_block_time() will have a gap
             * due to maintenance duration.
             *
             * This flag answers the question, "Was maintenance
             * performed in the last call to apply_block()?"
             */
            maintenance_flag = 0x01
         };
   };
}}

FC_REFLECT_DERIVED( graphene::chain::dynamic_global_property_object, (graphene::db::object),
                    (head_block_number)
                    (head_block_id)
                    (time)
                    (current_witness)
                    (next_maintenance_time)
                    (next_monthly_maintenance_time)
                    (last_budget_time)
                    (witness_budget)
                    (accounts_registered_this_interval)
                    (gcwd_price)
                    (next_poc_vote_time)
                    (end_poc_vote_time)
                    (poc_vote_is_active)
                    (poc3_percent)
                    (poc6_percent)
                    (poc12_percent)
                    (current_gr_interval)
                    (next_gr_interval_time)
                    (gr_bet_interval_time)
                    (end_gr_vote_time)
                    (gr_vote_is_active)
                    (gr_iron_volume)
                    (gr_bronze_volume)
                    (gr_silver_volume)
                    (gr_gold_volume)
                    (gr_platinum_volume)
                    (gr_diamond_volume)
                    (gr_master_volume)
                    (gr_iron_reward)
                    (gr_bronze_reward)
                    (gr_silver_reward)
                    (gr_gold_reward)
                    (gr_platinum_reward)
                    (gr_diamond_reward)
                    (gr_elite_reward)
                    (gr_master_reward)
                    (gr_top3_reward)
                    (recently_missed_count)
                    (current_aslot)
                    (recent_slots_filled)
                    (dynamic_flags)
                    (last_irreversible_block_num)
                  )

FC_REFLECT_DERIVED( graphene::chain::global_property_object, (graphene::db::object),
                    (staking_parameters)
                    (gamezone_parameters)
                    (greatrace_parameters)
                    (parameters)
                    (pending_parameters)
                    (next_available_vote_id)
                    (active_committee_members)
                    (active_witnesses)
                  )
