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
    	return void_result();
    }
    FC_CAPTURE_AND_RETHROW((op))
}
object_id_type gr_team_create_evaluator::do_apply(const gr_team_create_operation &op)
{
    try
    {
        database& d = db();
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
    	return void_result();
    }
    FC_CAPTURE_AND_RETHROW((op))
}
void_result gr_team_delete_evaluator::do_apply(const  gr_team_delete_operation &op)
{
    try
    {
        database& d = db();
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
    	return void_result();
    }
    FC_CAPTURE_AND_RETHROW((op))
}
object_id_type gr_invite_send_evaluator::do_apply(const gr_invite_send_operation &op)
{
    try
    {
        database& d = db();
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
    	database& d = db();
    	return void_result();
    }
    FC_CAPTURE_AND_RETHROW((op))
}
void_result gr_invite_accept_evaluator::do_apply(const  gr_invite_accept_operation &op)
{
    try
    {
        database& d = db();
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
    	return void_result();
    }
    FC_CAPTURE_AND_RETHROW((op))
}
void_result gr_player_remove_evaluator::do_apply(const  gr_player_remove_operation &op)
{
    try
    {
        database& d = db();
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
    	return void_result();
    }
    FC_CAPTURE_AND_RETHROW((op))
}
void_result gr_team_leave_evaluator::do_apply(const  gr_team_leave_operation &op)
{
    try
    {
        database& d = db();
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
    	return void_result();
    }
    FC_CAPTURE_AND_RETHROW((op))
}
object_id_type gr_vote_evaluator::do_apply(const gr_vote_operation &op)
{
    try
    {
        database& d = db();
        return new_gr_vote_object.id;
    }
    FC_CAPTURE_AND_RETHROW((op))
}


}} // namespace chain