/*
 * Copyright (c) 2021 CrowdWiz., and contributors.
 *
 * The MIT License
 */

#pragma once
#include <graphene/chain/evaluator.hpp>
#include <graphene/chain/greatrace_object.hpp>

namespace graphene { namespace chain {


class gr_team_create_evaluator : public evaluator<gr_team_create_evaluator>
   {
      public:
         typedef gr_team_create_operation operation_type;

         void_result do_evaluate( const gr_team_create_operation& o );
         object_id_type do_apply( const gr_team_create_operation& o );
   };
class gr_team_delete_evaluator : public evaluator<gr_team_delete_evaluator>
   {
      public:
         typedef gr_team_delete_operation operation_type;

         void_result do_evaluate( const gr_team_delete_operation& o );
         void_result do_apply( const gr_team_delete_operation& o );
   };
class gr_invite_send_evaluator : public evaluator<gr_invite_send_evaluator>
   {
      public:
         typedef gr_invite_send_operation operation_type;

         void_result do_evaluate( const gr_invite_send_operation& o );
         object_id_type do_apply( const gr_invite_send_operation& o );
   };
class gr_invite_accept_evaluator : public evaluator<gr_invite_accept_evaluator>
   {
      public:
         typedef gr_invite_accept_operation operation_type;

         void_result do_evaluate( const gr_invite_accept_operation& o );
         void_result do_apply( const gr_invite_accept_operation& o );
   };
class gr_player_remove_evaluator : public evaluator<gr_player_remove_evaluator>
   {
      public:
         typedef gr_player_remove_operation operation_type;

         void_result do_evaluate( const gr_player_remove_operation& o );
         void_result do_apply( const gr_player_remove_operation& o );
   };
class gr_team_leave_evaluator : public evaluator<gr_team_leave_evaluator>
   {
      public:
         typedef gr_team_leave_operation operation_type;

         void_result do_evaluate( const gr_team_leave_operation& o );
         void_result do_apply( const gr_team_leave_operation& o );
   };
class gr_vote_evaluator : public evaluator<gr_vote_evaluator>
   {
      public:
         typedef gr_vote_operation operation_type;

         void_result do_evaluate( const gr_vote_operation& o );
         object_id_type do_apply( const gr_vote_operation& o );
   };


} } // graphene::chain 