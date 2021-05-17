/*
 * Copyright (c) 2019 CrowdWiz., and contributors.
 *
 * The MIT License
 */

#include <graphene/chain/greatrace_evaluator.hpp>
#include <graphene/chain/greatrace_object.hpp>
#include <graphene/chain/account_object.hpp>
#include <graphene/chain/database.hpp>
#include <graphene/chain/hardfork.hpp>

namespace graphene
{
namespace chain
{
/* -----------------------------
 * gr_team_create_evaluator
 */
void_result gr_team_create_evaluator::do_evaluate(const gr_team_create_operation &op)
{
    try
    {
    	database& d = db();
		const dynamic_global_property_object& dgpo = d.get_dynamic_global_properties();
		FC_ASSERT( dgpo.current_gr_interval == 1, "Not in GR Start interval");

        const account_object& captain = op.captain(d);
        const auto& captain_stats = captain.statistics(d);

        const global_property_object& gpo = d.get_global_properties();
        const auto& gr_params = gpo.greatrace_parameters;

        FC_ASSERT( captain.referral_status_type >=  gr_params.min_team_status,  "Please upgrade your status");
        FC_ASSERT( captain_stats.first_month_gr >=  gr_params.min_team_volume,  "Insufficient last month volume");

        auto& team_indx = d.get_index_type<gr_team_index>();
        auto current_team_captain_itr = team_indx.indices().get<by_captain>().find( op.captain );
        FC_ASSERT( current_team_captain_itr == team_indx.indices().get<by_captain>().end(), "You already captain of the team!" );

        if( op.name.size() )
        {
            auto current_team_itr = team_indx.indices().get<by_name>().find( op.name );
            FC_ASSERT( current_team_itr == team_indx.indices().get<by_name>().end(),
                        "Team '${a}' already exists.", ("a",op.name) );
        }


    	return void_result();
    }
    FC_CAPTURE_AND_RETHROW((op))
}
object_id_type gr_team_create_evaluator::do_apply(const gr_team_create_operation &op)
{
    try
    {
        database& d = db();
        const account_object& captain = op.captain(d);

        const auto& new_gr_team_object = d.create<gr_team_object>([&](gr_team_object &obj) {
            obj.captain = op.captain;
            obj.name = op.name;
            obj.description = op.description;
            obj.logo = op.logo;
        });

        d.modify(*captain, [&](account_object& a) {
            a.gr_team = new_gr_team_object.id;
        });


        return new_gr_team_object.id;
    }
    FC_CAPTURE_AND_RETHROW((op))
}

/* -----------------------------
 * gr_team_delete_evaluator
 */
void_result gr_team_delete_evaluator::do_evaluate(const  gr_team_delete_operation &op)
{
    try
    {
    	database& d = db();
        const dynamic_global_property_object& dgpo = d.get_dynamic_global_properties();

		FC_ASSERT(  dgpo.current_gr_interval == 1 ||
                    dgpo.current_gr_interval == 3 ||
                    dgpo.current_gr_interval == 5 ||
                    dgpo.current_gr_interval == 7 ||
                    dgpo.current_gr_interval == 8 ||
                    dgpo.current_gr_interval == 10 ||
                    dgpo.current_gr_interval == 12 ||
                    dgpo.current_gr_interval == 14,
                    "Not in GR Start, Rest or Finish intervals"
                );
        const gr_team_object& team = op.team(d);
        FC_ASSERT( team.captain == op.captain, "You are not captain of this team!" );

    	return void_result();
    }
    FC_CAPTURE_AND_RETHROW((op))
}
void_result gr_team_delete_evaluator::do_apply(const  gr_team_delete_operation &op)
{
    try
    {
        database& d = db();
        const gr_team_object& team = op.team(d);
        const account_object& captain = op.captain(d);
        d.modify(*captain, [&](account_object& a) {
            a.gr_team.reset();
        });
        if ( team.players.size() > 0 ) {
            for ( auto player_id : team.players ) {
                auto& player = d.get<account_object>(player_id);
                d.modify(*player, [&](account_object& a) {
                    a.gr_team.reset();
                });
            }
        }
        d.remove(team);

    	return void_result();
    }
    FC_CAPTURE_AND_RETHROW((op))
}

/* -----------------------------
 * gr_invite_send_evaluator
 */
void_result gr_invite_send_evaluator::do_evaluate(const gr_invite_send_operation &op)
{
    try
    {
    	database& d = db();
		const dynamic_global_property_object& dgpo = d.get_dynamic_global_properties();

		FC_ASSERT(  dgpo.current_gr_interval == 1 ||
                    dgpo.current_gr_interval == 3 ||
                    dgpo.current_gr_interval == 5 ||
                    dgpo.current_gr_interval == 7 ||
                    dgpo.current_gr_interval == 8 ||
                    dgpo.current_gr_interval == 10 ||
                    dgpo.current_gr_interval == 12 ||
                    dgpo.current_gr_interval == 14,
                    "Not in GR Start, Rest or Finish intervals"
                );
        const gr_team_object& team = op.team(d);
        FC_ASSERT( team.captain == op.captain, "You are not captain of this team!" );
        FC_ASSERT( team.players.size() < 11, "You team is full!" );
        FC_ASSERT( team.players.find( op.player ) == team.players.end(), "Player already in team!");
        FC_ASSERT( !player.gr_team.valid(), "Player in other team!" );
    	return void_result();
    }
    FC_CAPTURE_AND_RETHROW((op))
}
object_id_type gr_invite_send_evaluator::do_apply(const gr_invite_send_operation &op)
{
    try
    {
        database& d = db();
        const auto& new_gr_invite_object = d.create<gr_invite_object>([&](gr_invite_object &obj) {
            obj.captain = op.captain;
            obj.player = op.player;
            obj.team = op.team;
        });
        
        return new_gr_invite_object.id;
    }
    FC_CAPTURE_AND_RETHROW((op))
}

/* -----------------------------
 * gr_invite_accept_evaluator
 */
void_result gr_invite_accept_evaluator::do_evaluate(const  gr_invite_accept_operation &op)
{
    try
    {
// captain
// player
// team
// invite
    	database& d = db();
        const gr_team_object& team = op.team(d);
        const gr_invite_object& invite = op.invite(d);
        const account_object& player = op.player(d);

        FC_ASSERT( invite.player == op.player, "Invite for different player!" );
        FC_ASSERT( invite.team == op.team, "Invite for different team!" );
        FC_ASSERT( team.captain == op.captain, "Wrong captain of this team!" );
        FC_ASSERT( team.players.find( op.player ) == team.players.end(), "Player already in team!");
        FC_ASSERT( team.players.size() < 11, "Team is full!" );
        FC_ASSERT( !player.gr_team.valid(), "Player in other team!" );

    	return void_result();
    }
    FC_CAPTURE_AND_RETHROW((op))
}
void_result gr_invite_accept_evaluator::do_apply(const  gr_invite_accept_operation &op)
{
    try
    {
        database& d = db();
        const gr_team_object& team = op.team(d);
        const gr_invite_object& invite = op.invite(d);
        const account_object& player = op.player(d);
        d.modify(*team, [&](gr_team_object& t) {
            t.players.insert(op.player);
        });

        d.modify(*player, [&](account_object& a) {
            a.gr_team=op.team;
        });

        d.remove(invite);

    	return void_result();
    }
    FC_CAPTURE_AND_RETHROW((op))
}

/* -----------------------------
 * gr_player_remove_evaluator
 */
void_result gr_player_remove_evaluator::do_evaluate(const  gr_player_remove_operation &op)
{
    try
    {
    	database& d = db();
		const dynamic_global_property_object& dgpo = d.get_dynamic_global_properties();

		FC_ASSERT(  dgpo.current_gr_interval == 1 ||
                    dgpo.current_gr_interval == 3 ||
                    dgpo.current_gr_interval == 5 ||
                    dgpo.current_gr_interval == 7 ||
                    dgpo.current_gr_interval == 8 ||
                    dgpo.current_gr_interval == 10 ||
                    dgpo.current_gr_interval == 12 ||
                    dgpo.current_gr_interval == 14,
                    "Not in GR Start, Rest or Finish intervals"
                );

        const gr_team_object& team = op.team(d);
        const account_object& player = op.player(d);

        FC_ASSERT( team.players.find( op.player ) != team.players.end(), "Player not in team!");
        if (player.gr_team.valid()) {
            FC_ASSERT( player.gr_team == op.team, "Player in other team!" );
        }
        FC_ASSERT( team.captain == op.captain, "You are not captain of this team!" );

    	return void_result();
    }
    FC_CAPTURE_AND_RETHROW((op))
}
void_result gr_player_remove_evaluator::do_apply(const  gr_player_remove_operation &op)
{
    try
    {
        database& d = db();
        const account_object& player = op.player(d);
        const gr_team_object& team = op.team(d);

        d.modify(*player, [&](account_object& a) {
            a.gr_team.reset();
        });

        d.modify(*team, [&](gr_team_object& t) {
            t.players.erase(op.player);
        });

    	return void_result();
    }
    FC_CAPTURE_AND_RETHROW((op))
}

/* -----------------------------
 * gr_team_leave_evaluator
 */
void_result gr_team_leave_evaluator::do_evaluate(const  gr_team_leave_operation &op)
{
    try
    {
    	database& d = db();
		const dynamic_global_property_object& dgpo = d.get_dynamic_global_properties();

		FC_ASSERT(  dgpo.current_gr_interval == 1 ||
                    dgpo.current_gr_interval == 3 ||
                    dgpo.current_gr_interval == 5 ||
                    dgpo.current_gr_interval == 7 ||
                    dgpo.current_gr_interval == 8 ||
                    dgpo.current_gr_interval == 10 ||
                    dgpo.current_gr_interval == 12 ||
                    dgpo.current_gr_interval == 14,
                    "Not in GR Start, Rest or Finish intervals"
                );

        const gr_team_object& team = op.team(d);
        const account_object& player = op.player(d);

        FC_ASSERT( team.players.find( op.player ) != team.players.end(), "Player not in team!");
        if (player.gr_team.valid()) {
            FC_ASSERT( player.gr_team == op.team, "Player in other team!" );
        }
        FC_ASSERT( team.captain == op.captain, "Wrong captain of this team!" );

    	return void_result();
    }
    FC_CAPTURE_AND_RETHROW((op))
}
void_result gr_team_leave_evaluator::do_apply(const  gr_team_leave_operation &op)
{
    try
    {
        database& d = db();
        const account_object& player = op.player(d);
        const gr_team_object& team = op.team(d);

        d.modify(*player, [&](account_object& a) {
            a.gr_team.reset();
        });

        d.modify(*team, [&](gr_team_object& t) {
            t.players.erase(op.player);
        });

    	return void_result();
    }
    FC_CAPTURE_AND_RETHROW((op))
}

/* -----------------------------
 * gr_vote_evaluator
 */
void_result gr_vote_evaluator::do_evaluate(const gr_vote_operation &op)
{
    try
    {
    	database& d = db();
        const dynamic_global_property_object& dgpo = d.get_dynamic_global_properties();
        FC_ASSERT(dgpo.gr_vote_is_active, "GR Vore is not running");
        const global_property_object& gpo = db().get_global_properties();
        const auto& gr_params = gpo.greatrace_parameters;
        const account_object& player = op.player(d);
        FC_ASSERT(player.last_gr_rank >= gr_params.min_vote_last_rank, "Your Rank is not enough");
    	return void_result();
    }
    FC_CAPTURE_AND_RETHROW((op))
}
object_id_type gr_vote_evaluator::do_apply(const gr_vote_operation &op)
{
    try
    {
        database& d = db();

        const auto& new_gr_votes_object = d.create<gr_votes_object>([&](gr_votes_object &obj) {
            obj.player = op.player;
            obj.gr_iron_volume = op.gr_iron_volume;
            obj.gr_bronze_volume = op.gr_bronze_volume;
            obj.gr_silver_volume = op.gr_silver_volume;
            obj.gr_gold_volume = op.gr_gold_volume;
            obj.gr_platinum_volume = op.gr_platinum_volume;
            obj.gr_diamond_volume = op.gr_diamond_volume;
            obj.gr_elite_volume = op.gr_elite_volume;
            obj.gr_iron_reward = op.gr_iron_reward;
            obj.gr_bronze_reward = op.gr_bronze_reward;
            obj.gr_silver_reward = op.gr_silver_reward;
            obj.gr_gold_reward = op.gr_gold_reward;
            obj.gr_platinum_reward = op.gr_platinum_reward;
            obj.gr_diamond_reward = op.gr_diamond_reward;
            obj.gr_elite_reward = op.gr_elite_reward;
            obj.gr_master_reward = op.gr_master_reward;
        });


        return new_gr_votes_object.id;
    }
    FC_CAPTURE_AND_RETHROW((op))
}


/* -----------------------------
 *  GR BETTING
 */

void_result gr_range_bet_evaluator::do_evaluate(const gr_range_bet_operation &op)
{
    try
    {
    	database& d = db();
        const auto& gr_team_idx = d.get_index_type<gr_team_index>().indices().get<by_id>();
        const dynamic_global_property_object& dgpo = d.get_dynamic_global_properties();
        FC_ASSERT(d.head_block_time() <= dgpo.gr_bet_interval_time, "All bets are off now");
        const account_object& bettor = op.bettor(d);
        const asset_object& asset_type  = op.bet.asset_id(d);
        const gr_team_object& gr_team  = op.team(d);
        FC_ASSERT(op.lower_rank<=gr_team_idx.size(), "Lower rank must by lower than total GR teams");
        FC_ASSERT(asset_type.symbol == "CWD", "Bet must be in CWD");
		bool insufficient_balance = d.get_balance( bettor, asset_type ).amount >= op.bet.amount;
		FC_ASSERT(insufficient_balance, "Insufficient balance");
    	return void_result();
    }
    FC_CAPTURE_AND_RETHROW((op))
}
object_id_type gr_range_bet_evaluator::do_apply(const gr_range_bet_operation &op)
{
    try
    {
        const database& d = db();
        const auto& by_bet_idx = d.get_index_type<gr_range_bet_index>().indices().get<by_bet>();
        auto itr = by_bet_idx.equal_range(boost::make_tuple( op.team, op.lower_rank, op.upper_rank ));
        if (itr.first != by_bet_idx.end) {
            d.modify( *itr, [&](gr_range_bet_object& gr_bet) {
                if (op.result == true) {
                    if (gr_bet.true_bets.count(op.bettor) > 0) {
                        gr_bet.true_bets[op.bettor]+=op.bet.amount;
                    }
                    else{
                        gr_bet.true_bets.emplace(op.bettor,op.bet.amount);
                    }  
                    gr_bet.total_true_bets+=op.bet.amount;
                }
                else{
                    if (gr_bet.false_bets.count(op.bettor) > 0) {
                        gr_bet.false_bets[op.bettor]+=op.bet.amount;
                    }
                    else{
                        gr_bet.false_bets.emplace(op.bettor,op.bet.amount);
                    }  
                    gr_bet.total_false_bets+=op.bet.amount;
                }
                obj.total_prize += op.bet.amount;

            });
            
            d.adjust_balance( op.bettor, -op.bet );
            return itr->id;
        }
        else {
            const auto& new_gr_range_bet_object = d.create<gr_range_bet_object>([&](gr_range_bet_object &obj) {
                obj.team = op.team;
                obj.lower_rank = op.lower_rank;
                obj.upper_rank = op.upper_rank;
                obj.total_prize = op.bet.amount;
                if (op.result == true) {
                    obj.true_bets.emplace(op.bettor,op.bet.amount);
                    obj.total_true_bets=op.bet.amount;
                }
                else{
                    obj.false_bets.emplace(op.bettor,op.bet.amount);
                    obj.total_false_bets=op.bet.amount;
                }
            });
            d.adjust_balance( op.bettor, -op.bet );
            return new_gr_range_bet_object.id;
        }
    }
    FC_CAPTURE_AND_RETHROW((op))
}

void_result gr_team_bet_evaluator::do_evaluate(const gr_team_bet_operation &op)
{
    try
    {
    	database& d = db();
        const auto& gr_team_idx = d.get_index_type<gr_team_index>().indices().get<by_id>();
        const dynamic_global_property_object& dgpo = d.get_dynamic_global_properties();
        FC_ASSERT(d.head_block_time() <= dgpo.gr_bet_interval_time, "All bets are off now");
        const account_object& bettor = op.bettor(d);
        const asset_object& asset_type  = op.bet.asset_id(d);
        const gr_team_object& gr_team1  = op.team1(d);
        const gr_team_object& gr_team2  = op.team2(d);
        FC_ASSERT(op.winner == op.team1 || op.winner == op.team2, "Winner must be one of the bets teams");
        FC_ASSERT(asset_type.symbol == "CWD", "Bet must be in CWD");
		bool insufficient_balance = d.get_balance( bettor, asset_type ).amount >= op.bet.amount;
		FC_ASSERT(insufficient_balance, "Insufficient balance");
    	return void_result();
    }
    FC_CAPTURE_AND_RETHROW((op))
}
object_id_type gr_team_bet_evaluator::do_apply(const gr_team_bet_operation &op)
{
    try
    {
        const database& d = db();
        const auto& by_bet_idx = d.get_index_type<gr_team_bet_index>().indices().get<by_bet>();
        auto itr = by_bet_idx.equal_range(boost::make_tuple( op.team1, op.team2 ));
        if (itr.first != by_bet_idx.end) {
            d.modify( *itr, [&](gr_team_bet_object& gr_bet) {
                if (op.winner == op.team1) {
                    if (gr_bet.team1_bets.count(op.bettor) > 0) {

                        gr_bet.team1_bets[op.bettor]+=op.bet.amount;
                    }
                    else {
                        gr_bet.team1_bets.emplace(op.bettor,op.bet.amount);
                    }
                    gr_bet.total_team1_bets+=op.bet.amount;

                }
                else {
                    if (gr_bet.team2_bets.count(op.bettor) > 0) {
                        gr_bet.team2_bets[op.bettor]+=op.bet.amount;
                    }
                    else {
                        gr_bet.team2_bets.emplace(op.bettor,op.bet.amount);
                    }
                    gr_bet.total_team2_bets+=op.bet.amount;         
                }
            });

            gr_bet.total_prize += op.bet.amount;
            d.adjust_balance( op.bettor, -op.bet );
            return itr->id;
        }
        else {
            auto itr2 = by_bet_idx.equal_range(boost::make_tuple( op.team2, op.team1 ));
            if (itr2.first != by_bet_idx.end) {
                d.modify( *itr2, [&](gr_team_bet_object& gr_bet) {
                    if (op.winner == op.team1) {
                        if (gr_bet.team1_bets.count(op.bettor) > 0) {

                            gr_bet.team1_bets[op.bettor]+=op.bet.amount;
                        }
                        else {
                            gr_bet.team1_bets.emplace(op.bettor,op.bet.amount);
                        }
                        gr_bet.total_team1_bets+=op.bet.amount;
                    }
                    else {
                        if (gr_bet.team2_bets.count(op.bettor) > 0) {
                            gr_bet.team2_bets[op.bettor]+=op.bet.amount;
                        }
                        else {
                            gr_bet.team2_bets.emplace(op.bettor,op.bet.amount);
                        }  
                        gr_bet.total_team2_bets+=op.bet.amount;
           
                    }
                });
                gr_bet.total_prize += op.bet.amount;                
                d.adjust_balance( op.bettor, -op.bet );
                return itr2->id;
            }
            else {
                const auto& new_gr_team_bet_object = d.create<gr_team_bet_object>([&](gr_team_bet_object &obj) {
                    obj.team1 = op.team1;
                    obj.team2 = op.team2;
                    obj.total_prize = op.bet.amount;
                    if (op.winner == op.team1) {
                        obj.team1_bets.emplace(op.bettor,op.bet.amount);
                    }
                    else{
                        obj.team2_bets.emplace(op.bettor,op.bet.amount);
                    }
                });
                d.adjust_balance( op.bettor, -op.bet );
                return new_gr_team_bet_object.id;
        }
    }
    FC_CAPTURE_AND_RETHROW((op))
}


}} // namespace chain