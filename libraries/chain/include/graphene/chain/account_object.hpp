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
#include <graphene/chain/protocol/operations.hpp>
#include <graphene/db/generic_index.hpp>
#include <boost/multi_index/composite_key.hpp>

namespace graphene
{
namespace chain
{
class database;

/**
    * @class account_statistics_object
    * @ingroup object
    * @ingroup implementation
    *
    * This object contains regularly updated statistical data about an account. It is provided for the purpose of
    * separating the account data that changes frequently from the account data that is mostly static, which will
    * minimize the amount of data that must be backed up as part of the undo history everytime a transfer is made.
    */
class account_statistics_object : public graphene::db::abstract_object<account_statistics_object>
{
 public:
   static const uint8_t space_id = implementation_ids;
   static const uint8_t type_id = impl_account_statistics_object_type;

   account_id_type owner;

   string name; ///< redundantly store account name here for better maintenance performance

   /**
          * Keep the most recent operation as a root pointer to a linked list of the transaction history.
          */
   account_transaction_history_id_type most_recent_op;
   /** Total operations related to this account. */
   uint64_t total_ops = 0;
   /** Total operations related to this account that has been removed from the database. */
   uint64_t removed_ops = 0;

   /**
          * When calculating votes it is necessary to know how much is stored in orders (and thus unavailable for
          * transfers). Rather than maintaining an index of [asset,owner,order_id] we will simply maintain the running
          * total here and update it every time an order is created or modified.
          */
   share_type total_core_in_orders;

   share_type core_in_balance = 0; ///< redundantly store core balance here for better maintenance performance

   bool has_cashback_vb = false; ///< redundantly store this for better maintenance performance

   bool is_voting = false; ///< redundately store whether this account is voting for better maintenance performance

   time_point_sec last_vote_time; // add last time voted

   /// Whether this account owns some CORE asset and is voting
   inline bool has_some_core_voting() const
   {
      return is_voting && (total_core_in_orders > 0 || core_in_balance > 0 || has_cashback_vb);
   }

   inline bool has_network_income() const
   {
      return (first_month_income > 0 || second_month_income > 0 || third_month_income > 0 || current_month_income > 0);
   }

   inline bool has_p2p_rating() const
   {
      return (p2p_first_month_rating > 0 || p2p_current_month_rating > 0);
   }

   inline bool has_poc_vote() const
   {
      return (poc3_vote > 0 || poc6_vote > 0 || poc12_vote > 0);
   }
   /**
          * Tracks the total fees paid by this account for the purpose of calculating bulk discounts.
          */
   share_type lifetime_fees_paid;

   /**
          * Tracks the fees paid by this account which have not been disseminated to the various parties that receive
          * them yet (registrar, referrer, lifetime referrer, network, etc). This is used as an optimization to avoid
          * doing massive amounts of uint128 arithmetic on each and every operation.
          *
          * These fees will be paid out as vesting cash-back, and this counter will reset during the maintenance
          * interval.
          */
   share_type pending_fees;
   /**
          * Same as @ref pending_fees, except these fees will be paid out as pre-vested cash-back (immediately
          * available for withdrawal) rather than requiring the normal vesting period.
          */
   share_type pending_vested_fees;

   /**
    * CROWDWIZ referral subsystem parameters
    */

   /// CWD Current Personal volume for 90 days
   share_type personal_volume_in_period = 0;
   /// CWD Current Network volume for 90 days
   share_type network_volume_in_period = 0;
   /// CWD Last time personal volume operation occur
   time_point_sec last_pv_update_date;
   /// CWD Last time network volume operation occur
   time_point_sec last_nv_update_date;
   /**
    * CROWDWIZ GAMEZONE subsystem parameters
    */
    matrix_id_type       matrix;
    uint8_t              matrix_active_levels = 1;
    uint8_t              matrix_cells_opened = 0;
    uint16_t             matrix_rooms_opened = 0;
    uint32_t             lottery_goods_rating = 0;
   /**
    * CROWDWIZ P2P exchange subsystem parameters
    */    
    uint32_t             p2p_complete_deals = 0;
    uint32_t             p2p_canceled_deals = 0;
    uint32_t             p2p_arbitrage_loose = 0;
    share_type           p2p_deals_volume = 0;
    time_point_sec       p2p_banned = fc::time_point_sec( 0 );
    bool                 p2p_gateway_active = true;
    uint32_t             p2p_first_month_rating = 0;
    uint32_t             p2p_current_month_rating = 0;

   /**
    * CROWDWIZ P2P credit subsystem parameters
    */

   // referral system statistics
   share_type first_month_income = 0;
   share_type second_month_income = 0;
   share_type third_month_income = 0;
   share_type current_month_income = 0;

   // great race first line stats
   share_type first_month_gr = 0;
   share_type current_month_gr = 0;

   // credit parameters
   share_type total_credit = 0;
   share_type allowed_to_repay = 0;
   share_type credit_repaid = 0;
   account_id_type creditor = account_id_type(0);

    // PoC Voting
    share_type poc3_vote  = 0;
    share_type poc6_vote  = 0;
    share_type poc12_vote = 0;
    bool had_staking = false;

   /// Whether this account has pending fees, no matter vested or not
   inline bool has_pending_fees() const { return pending_fees > 0 || pending_vested_fees > 0; }

   /// Whether need to process this account during the maintenance interval
   inline bool need_maintenance() const { return has_some_core_voting() || has_pending_fees(); }

   /// @brief Split up and pay out @ref pending_fees and @ref pending_vested_fees
   void process_fees(const account_object &a, database &d) const;

   /**
          * Core fees are paid into the account_statistics_object by this method
          */
   void pay_fee(share_type core_fee, share_type cashback_vesting_threshold);
   /**
          * Update network volume
          */
   void update_nv(share_type volume, uint8_t level, uint16_t max_reward_level, const account_object &a, database &d) const;
   share_type get_nv(database &d) const;
   /**
          * Update personal volume
          */
   void update_pv(share_type volume, const account_object &a, database &d) const;
   share_type get_pv(database &d) const;

};

/**
    * @brief Tracks the balance of a single account/asset pair
    * @ingroup object
    *
    * This object is indexed on owner and asset_type so that black swan
    * events in asset_type can be processed quickly.
    */
class account_balance_object : public abstract_object<account_balance_object>
{
 public:
   static const uint8_t space_id = implementation_ids;
   static const uint8_t type_id = impl_account_balance_object_type;

   account_id_type owner;
   asset_id_type asset_type;
   share_type balance;
   bool maintenance_flag = false; ///< Whether need to process this balance object in maintenance interval

   asset get_balance() const { return asset(balance, asset_type); }
   void adjust_balance(const asset &delta);
};

/**
    * @brief This class represents an account on the object graph
    * @ingroup object
    * @ingroup protocol
    *
    * Accounts are the primary unit of authority on the graphene system. Users must have an account in order to use
    * assets, trade in the markets, vote for committee_members, etc.
    */
class account_object : public graphene::db::abstract_object<account_object>
{
 public:
   static const uint8_t space_id = protocol_ids;
   static const uint8_t type_id = account_object_type;

   /**
          * The time at which this account's membership expires.
          * If set to any time in the past, the account is a basic account.
          * If set to time_point_sec::maximum(), the account is a lifetime member.
          * If set to any time not in the past less than time_point_sec::maximum(), the account is an annual member.
          *
          * See @ref is_lifetime_member, @ref is_basic_account, @ref is_annual_member, and @ref is_member
          */
   time_point_sec membership_expiration_date;

   ///The account that paid the fee to register this account. Receives a percentage of referral rewards.
   account_id_type registrar;
   /// The account credited as referring this account. Receives a percentage of referral rewards.
   account_id_type referrer;
   /// The lifetime member at the top of the referral tree. Receives a percentage of referral rewards.
   account_id_type lifetime_referrer;

   /// Percentage of fee which should go to network.
   uint16_t network_fee_percentage = GRAPHENE_DEFAULT_NETWORK_PERCENT_OF_FEE;
   /// Percentage of fee which should go to lifetime referrer.
   uint16_t lifetime_referrer_fee_percentage = 0;
   /// Percentage of referral rewards (leftover fee after paying network and lifetime referrer) which should go
   /// to referrer. The remainder of referral rewards goes to the registrar.
   uint16_t referrer_rewards_percentage = 0;

   /// The account's name. This name must be unique among all account names on the graph. May not be empty.
   string name;

   /**
          * The owner authority represents absolute control over the account. Usually the keys in this authority will
          * be kept in cold storage, as they should not be needed very often and compromise of these keys constitutes
          * complete and irrevocable loss of the account. Generally the only time the owner authority is required is to
          * update the active authority.
          */
   authority owner;
   /// The owner authority contains the hot keys of the account. This authority has control over nearly all
   /// operations the account may perform.
   authority active;

   typedef account_options options_type;
   account_options options;

   /// The reference implementation records the account's statistics in a separate object. This field contains the
   /// ID of that object.
   account_statistics_id_type statistics;

   /**
          * This is a set of all accounts which have 'whitelisted' this account. Whitelisting is only used in core
          * validation for the purpose of authorizing accounts to hold and transact in whitelisted assets. This
          * account cannot update this set, except by transferring ownership of the account, which will clear it. Other
          * accounts may add or remove their IDs from this set.
          */
   flat_set<account_id_type> whitelisting_accounts;

   /**
          * Optionally track all of the accounts this account has whitelisted or blacklisted, these should
          * be made Immutable so that when the account object is cloned no deep copy is required.  This state is
          * tracked for GUI display purposes.
          *
          * TODO: move white list tracking to its own multi-index container rather than having 4 fields on an
          * account.   This will scale better because under the current design if you whitelist 2000 accounts,
          * then every time someone fetches this account object they will get the full list of 2000 accounts.
          */
   ///@{
   set<account_id_type> whitelisted_accounts;
   set<account_id_type> blacklisted_accounts;
   ///@}

   /**
          * This is a set of all accounts which have 'blacklisted' this account. Blacklisting is only used in core
          * validation for the purpose of forbidding accounts from holding and transacting in whitelisted assets. This
          * account cannot update this set, and it will be preserved even if the account is transferred. Other accounts
          * may add or remove their IDs from this set.
          */
   flat_set<account_id_type> blacklisting_accounts;

   /**
          * Vesting balance which receives cashback_reward deposits.
          */
   optional<vesting_balance_id_type> cashback_vb;

   special_authority owner_special_authority = no_special_authority();
   special_authority active_special_authority = no_special_authority();

   /**
          * This flag is set when the top_n logic sets both authorities,
          * and gets reset when authority or special_authority is set.
          */
   uint8_t top_n_control_flags = 0;
   static const uint8_t top_n_control_owner = 1;
   static const uint8_t top_n_control_active = 2;

   /**
          * This is a set of assets which the account is allowed to have.
          * This is utilized to restrict buyback accounts to the assets that trade in their markets.
          * In the future we may expand this to allow accounts to e.g. voluntarily restrict incoming transfers.
          */
   optional<flat_set<asset_id_type>> allowed_assets;

   uint8_t referral_levels;
   uint8_t referral_status_type = 0;
   share_type referral_status_paid_fee = 0;
   time_point_sec referral_status_expiration_date;
   uint16_t status_denominator;

   optional<gr_team_id_type> gr_team;
   uint8_t last_gr_rank = 0;

   inline bool has_gr_rank() const
   {
      return (last_gr_rank > 0);
   }

   bool has_special_authority() const
   {
      return (owner_special_authority.which() != special_authority::tag<no_special_authority>::value) || (active_special_authority.which() != special_authority::tag<no_special_authority>::value);
   }

   template <typename DB>
   const vesting_balance_object &cashback_balance(const DB &db) const
   {
      FC_ASSERT(cashback_vb);
      return db.get(*cashback_vb);
   }

   /// @return true if this is a lifetime member account; false otherwise.
   bool is_lifetime_member() const
   {
      return membership_expiration_date == time_point_sec::maximum();
   }
   /// @return true if this is a basic account; false otherwise.
   bool is_basic_account(time_point_sec now) const
   {
      return now > membership_expiration_date;
   }
   /// @return true if the account is an unexpired annual member; false otherwise.
   /// @note This method will return false for lifetime members.
   bool is_annual_member(time_point_sec now) const
   {
      return !is_lifetime_member() && !is_basic_account(now);
   }
   /// @return true if the account is an annual or lifetime member; false otherwise.
   bool is_member(time_point_sec now) const
   {
      return !is_basic_account(now);
   }

   uint8_t active_referral_status(time_point_sec now) const
   {
      if (now <= referral_status_expiration_date)
      {
            return referral_status_type;
      }
      else
        {
            return 0;
        }
   }
   account_id_type get_id() const { return id; }
//    account_id_type next_rewardable(const account_object &a, database &d) ;
};

/**
    *  @brief This secondary index will allow a reverse lookup of all accounts that a particular key or account
    *  is an potential signing authority.
    */
class account_member_index : public secondary_index
{
 public:
   virtual void object_inserted(const object &obj) override;
   virtual void object_removed(const object &obj) override;
   virtual void about_to_modify(const object &before) override;
   virtual void object_modified(const object &after) override;

   /** given an account or key, map it to the set of accounts that reference it in an active or owner authority */
   map<account_id_type, set<account_id_type>> account_to_account_memberships;
   map<public_key_type, set<account_id_type>, pubkey_comparator> account_to_key_memberships;
   /** some accounts use address authorities in the genesis block */
   map<address, set<account_id_type>> account_to_address_memberships;

 protected:
   set<account_id_type> get_account_members(const account_object &a) const;
   set<public_key_type, pubkey_comparator> get_key_members(const account_object &a) const;
   set<address> get_address_members(const account_object &a) const;

   set<account_id_type> before_account_members;
   set<public_key_type, pubkey_comparator> before_key_members;
   set<address> before_address_members;
};

/**
    *  @brief This secondary index will allow a reverse lookup of all accounts that have been referred by
    *  a particular account.
    */
class account_referrer_index : public secondary_index
{
 public:
   virtual void object_inserted(const object &obj) override;
   virtual void object_removed(const object &obj) override;
   virtual void about_to_modify(const object &before) override;
   virtual void object_modified(const object &after) override;

   /** maps the referrer to the set of accounts that they have referred */
   map<account_id_type, set<account_id_type>> referred_by;
};

/**
    *  @brief This secondary index will allow fast access to the balance objects
    *         that belonging to an account.
    */
class balances_by_account_index : public secondary_index
{
 public:
   virtual void object_inserted(const object &obj) override;
   virtual void object_removed(const object &obj) override;
   virtual void about_to_modify(const object &before) override;
   virtual void object_modified(const object &after) override;

   const map<asset_id_type, const account_balance_object *> &get_account_balances(const account_id_type &acct) const;
   const account_balance_object *get_account_balance(const account_id_type &acct, const asset_id_type &asset) const;

 private:
   static const uint8_t bits;
   static const uint64_t mask;

   /** Maps each account to its balance objects */
   vector<vector<map<asset_id_type, const account_balance_object *>>> balances;
   std::stack<object_id_type> ids_being_modified;
};

struct by_asset_balance;
struct by_maintenance_flag;
/**
    * @ingroup object_index
    */
typedef multi_index_container<
    account_balance_object,
    indexed_by<
        ordered_unique<tag<by_id>, member<object, object_id_type, &object::id>>,
        ordered_non_unique<tag<by_maintenance_flag>,
                           member<account_balance_object, bool, &account_balance_object::maintenance_flag>>,
        ordered_unique<tag<by_asset_balance>,
                       composite_key<
                           account_balance_object,
                           member<account_balance_object, asset_id_type, &account_balance_object::asset_type>,
                           member<account_balance_object, share_type, &account_balance_object::balance>,
                           member<account_balance_object, account_id_type, &account_balance_object::owner>>,
                       composite_key_compare<
                           std::less<asset_id_type>,
                           std::greater<share_type>,
                           std::less<account_id_type>>>>>
    account_balance_object_multi_index_type;

/**
    * @ingroup object_index
    */
typedef generic_index<account_balance_object, account_balance_object_multi_index_type> account_balance_index;

struct by_name
{
};
struct by_status_expiration;
struct by_gr_rank;

/**
    * @ingroup object_index
    */
typedef multi_index_container<
    account_object,
    indexed_by<
        ordered_unique<tag<by_id>, member<object, object_id_type, &object::id>>,
        ordered_non_unique<tag<by_status_expiration>, member<account_object, time_point_sec, &account_object::referral_status_expiration_date>>,
        ordered_unique<tag<by_gr_rank>,
                composite_key<
                    account_object,
                    const_mem_fun<account_object, bool, &account_object::has_gr_rank>,
                    member<account_object, string, &account_object::name>>>,
        ordered_unique<tag<by_name>, member<account_object, string, &account_object::name>>>>
    account_multi_index_type;
/**
    * @ingroup object_index
    */
typedef generic_index<account_object, account_multi_index_type> account_index;

struct by_owner;
struct by_creditor;
struct by_maintenance_seq;
struct by_network_income;
struct by_poc_vote;
struct by_p2p_rating;
/**
    * @ingroup object_index
    */
typedef multi_index_container<
    account_statistics_object,
    indexed_by<
        ordered_unique<tag<by_id>, member<object, object_id_type, &object::id>>,
        ordered_unique<tag<by_owner>,
                       member<account_statistics_object, account_id_type, &account_statistics_object::owner>>,
        ordered_non_unique<tag<by_creditor>,
                       member<account_statistics_object, account_id_type, &account_statistics_object::creditor>>,
        ordered_unique<tag<by_maintenance_seq>,
                       composite_key<
                           account_statistics_object,
                           const_mem_fun<account_statistics_object, bool, &account_statistics_object::need_maintenance>,
                           member<account_statistics_object, string, &account_statistics_object::name>>>,
        ordered_unique<tag<by_network_income>,
                       composite_key<
                           account_statistics_object,
                           const_mem_fun<account_statistics_object, bool, &account_statistics_object::has_network_income>,
                           member<account_statistics_object, string, &account_statistics_object::name>>>,
        ordered_unique<tag<by_poc_vote>,
                       composite_key<
                           account_statistics_object,
                           const_mem_fun<account_statistics_object, bool, &account_statistics_object::has_poc_vote>,
                           member<account_statistics_object, string, &account_statistics_object::name>>>,
        ordered_unique<tag<by_p2p_rating>,
                       composite_key<
                           account_statistics_object,
                           const_mem_fun<account_statistics_object, bool, &account_statistics_object::has_p2p_rating>,
                           member<account_statistics_object, string, &account_statistics_object::name>>>>>
    account_stats_multi_index_type;

/**
    * @ingroup object_index
    */
typedef generic_index<account_statistics_object, account_stats_multi_index_type> account_stats_index;

} // namespace chain
} // namespace graphene

FC_REFLECT_DERIVED(graphene::chain::account_object,
                   (graphene::db::object),
                   (membership_expiration_date)(registrar)(referrer)(lifetime_referrer)(network_fee_percentage)(lifetime_referrer_fee_percentage)(referrer_rewards_percentage)(name)(owner)(active)(options)(statistics)(whitelisting_accounts)(blacklisting_accounts)(whitelisted_accounts)(blacklisted_accounts)(cashback_vb)(owner_special_authority)(active_special_authority)(top_n_control_flags)(allowed_assets)(referral_levels)(referral_status_type)(referral_status_paid_fee)(referral_status_expiration_date)(status_denominator)(gr_team)(last_gr_rank))

FC_REFLECT_DERIVED(graphene::chain::account_balance_object,
                   (graphene::db::object),
                   (owner)(asset_type)(balance)(maintenance_flag))

FC_REFLECT_DERIVED(graphene::chain::account_statistics_object,
                   (graphene::chain::object),
                   (owner)
                   (name)
                   (most_recent_op)
                   (total_ops)
                   (removed_ops)
                   (total_core_in_orders)
                   (core_in_balance)
                   (has_cashback_vb)
                   (is_voting)
                   (last_vote_time)
                   (lifetime_fees_paid)
                   (pending_fees)
                   (pending_vested_fees)
                   (personal_volume_in_period)
                   (network_volume_in_period)
                   (last_pv_update_date)
                   (last_nv_update_date)
                   (matrix)
                   (matrix_active_levels)
                   (matrix_cells_opened)
                   (matrix_rooms_opened)
                   (lottery_goods_rating)
                   (p2p_complete_deals)
                   (p2p_canceled_deals)
                   (p2p_arbitrage_loose)
                   (p2p_deals_volume)
                   (p2p_banned)
                   (p2p_gateway_active)
                   (p2p_first_month_rating)
                   (p2p_current_month_rating)
                   (first_month_income)
                   (second_month_income)
                   (third_month_income)
                   (current_month_income)
                   (first_month_gr)
                   (current_month_gr)
                   (total_credit)
                   (allowed_to_repay)
                   (credit_repaid)
                   (creditor)
                   (poc3_vote)
                   (poc6_vote)
                   (poc12_vote)
                   (had_staking)
                   )
