/*
 * Copyright (c) 2019 CrowdWiz., and contributors.
 *
 * The MIT License
 */

#pragma once
#include <graphene/chain/protocol/asset.hpp>
#include <graphene/chain/protocol/types.hpp>
#include <graphene/chain/protocol/operations.hpp>

#include <graphene/db/object.hpp>
#include <graphene/db/generic_index.hpp>
#include <boost/multi_index/composite_key.hpp>

namespace graphene { namespace chain {
   using namespace graphene::db;

   class flipcoin_object : public abstract_object<flipcoin_object>
   {
      public:
        static const uint8_t space_id = protocol_ids;
        static const uint8_t type_id = flipcoin_object_type;
		account_id_type bettor;
		optional <account_id_type> caller;
		optional <account_id_type> winner;
        asset bet;
        uint8_t nonce = 1;
        time_point_sec expiration;
        uint8_t status = 0; //0 = active, 1 = filled, 2 = flipped
   };

    struct by_id;
    struct by_status;
    struct by_expiration;

    using flipcoin_multi_index_type = multi_index_container<
      flipcoin_object,
      indexed_by<
         ordered_unique< tag<by_id>,
            member<object, object_id_type, &object::id>
         >,
         ordered_non_unique< tag<by_status>,
            composite_key<
               flipcoin_object,
               member<flipcoin_object, uint8_t, &flipcoin_object::status>,
               member< object, object_id_type, &object::id>
            >
         >,
         ordered_unique< tag<by_expiration>,
            composite_key< flipcoin_object,
               member< flipcoin_object, time_point_sec, &flipcoin_object::expiration>,
               member< object, object_id_type, &object::id>
            >
         >
      >
   >;
   using flipcoin_index = generic_index<flipcoin_object, flipcoin_multi_index_type>;

   class lottery_goods_object : public abstract_object<lottery_goods_object>
   {
      public:
         static const uint8_t space_id = protocol_ids;
         static const uint8_t type_id = lottery_goods_object_type;

         account_id_type             owner;
         std::vector<account_id_type>     participants;
         uint32_t                    total_participants;
         asset                       ticket_price;
         time_point_sec              expiration;
         uint16_t                    latency_sec;
         uint8_t                     status = 0; //0 = new, 1 = sold out, 2 = completed, 3 = owner paid, 4 = canceled
         string                      img_url;
         string                      description;
         optional<account_id_type>   winner;
         optional<memo_data>         winner_contacts;
   };

   struct by_id;
   struct by_status;
   struct by_owner_status;
   struct by_winner_status;
   struct by_expiration;

   typedef multi_index_container<
      lottery_goods_object,
      indexed_by<
         ordered_unique< tag<by_id>, member< object, object_id_type, &object::id > >,
         ordered_unique< tag<by_status>,
            composite_key< lottery_goods_object,
               member<lottery_goods_object, uint8_t, &lottery_goods_object::status>,
               member< object, object_id_type, &object::id >
            >
         >,
         ordered_unique< tag<by_owner_status>,
            composite_key< lottery_goods_object,
               member<lottery_goods_object, account_id_type, &lottery_goods_object::owner>,
               member<lottery_goods_object, uint8_t, &lottery_goods_object::status>,
               member< object, object_id_type, &object::id >
            >
         >,
         ordered_unique< tag<by_winner_status>,
            composite_key< lottery_goods_object,
               member<lottery_goods_object, optional<account_id_type>, &lottery_goods_object::winner>,
               member<lottery_goods_object, uint8_t, &lottery_goods_object::status>,
               member< object, object_id_type, &object::id >
            >
         >,
         ordered_non_unique< tag<by_expiration>,
               member<lottery_goods_object, time_point_sec, &lottery_goods_object::expiration>
         >
      >
    > lottery_goods_multi_index_type;
	
   using lottery_goods_index = generic_index<lottery_goods_object, lottery_goods_multi_index_type>;

   /* 
    * MATRIX RELOADING OBJECTS
    */

   class matrix_object : public abstract_object<matrix_object>
   {
      public:
         static const uint8_t space_id = protocol_ids;
         static const uint8_t type_id = matrix_object_type;
         uint32_t             start_block_number = 0;
         uint32_t             finish_block_number = 0;
         uint8_t              status = 0;
         share_type           amount;
         share_type           total_amount;
         share_type           last_120k_amount;
         share_type           matrix_level_1_price;
         share_type           matrix_level_2_price;
         share_type           matrix_level_3_price;
         share_type           matrix_level_4_price;
         share_type           matrix_level_5_price;
         share_type           matrix_level_6_price;
         share_type           matrix_level_7_price;
         share_type           matrix_level_8_price;

         share_type           matrix_level_1_prize;
         share_type           matrix_level_2_prize;
         share_type           matrix_level_3_prize;
         share_type           matrix_level_4_prize;
         share_type           matrix_level_5_prize;
         share_type           matrix_level_6_prize;
         share_type           matrix_level_7_prize;
         share_type           matrix_level_8_prize;

         uint8_t              matrix_level_1_cells;
         uint8_t              matrix_level_2_cells;
         uint8_t              matrix_level_3_cells;
         uint8_t              matrix_level_4_cells;
         uint8_t              matrix_level_5_cells;
         uint8_t              matrix_level_6_cells;
         uint8_t              matrix_level_7_cells;
         uint8_t              matrix_level_8_cells;

   };

    struct by_id;
    struct by_status;
    struct by_finish_block_number;
    struct by_start_block_number;

    using matrix_multi_index_type = multi_index_container<
      matrix_object,
      indexed_by<
         ordered_unique< tag<by_id>,
            member<object, object_id_type, &object::id>
         >,
         ordered_non_unique< tag<by_status>,
            composite_key<
               matrix_object,
               member<matrix_object, uint8_t, &matrix_object::status>,
               member< object, object_id_type, &object::id>
            >
         >,
         ordered_non_unique< tag<by_finish_block_number>,
            composite_key<
               matrix_object,
               member<matrix_object, uint32_t, &matrix_object::finish_block_number>,
               member< object, object_id_type, &object::id>
            >
         >,
         ordered_non_unique< tag<by_start_block_number>,
            composite_key<
               matrix_object,
               member<matrix_object, uint32_t, &matrix_object::start_block_number>,
               member< object, object_id_type, &object::id>
            >
         >
      >
   >;
   using matrix_index = generic_index<matrix_object, matrix_multi_index_type>;

   class matrix_rooms_object : public abstract_object<matrix_rooms_object>
   {
      public:
         static const uint8_t space_id = protocol_ids;
         static const uint8_t type_id = matrix_rooms_object_type;

         matrix_id_type                         matrix;
         account_id_type                        matrix_player;
         uint8_t                                matrix_level = 0;
         uint8_t                                total_cells = 0;
         uint8_t                                status = 0; // 0 - open cells closed, 1 - open cells opened, 2 - closed
         std::vector<account_id_type>           cells;
   };

    struct by_id;
    struct by_matrix_player_status;
    struct by_matrix_level_player_status;
    struct by_matrix_level_status;

    using matrix_rooms_multi_index_type = multi_index_container<
      matrix_rooms_object,
      indexed_by<
         ordered_unique< tag<by_id>,
            member<object, object_id_type, &object::id>
         >,
         ordered_non_unique< tag<by_matrix_player_status>,
            composite_key<
               matrix_rooms_object,
               member<matrix_rooms_object, matrix_id_type, &matrix_rooms_object::matrix>,
               member<matrix_rooms_object, account_id_type, &matrix_rooms_object::matrix_player>,
               member<matrix_rooms_object, uint8_t, &matrix_rooms_object::status>,
               member< object, object_id_type, &object::id>
            >
         >,
         ordered_non_unique< tag<by_matrix_level_player_status>,
            composite_key<
               matrix_rooms_object,
               member<matrix_rooms_object, matrix_id_type, &matrix_rooms_object::matrix>,
               member<matrix_rooms_object, account_id_type, &matrix_rooms_object::matrix_player>,
               member<matrix_rooms_object, uint8_t, &matrix_rooms_object::matrix_level>,
               member<matrix_rooms_object, uint8_t, &matrix_rooms_object::status>,
               member< object, object_id_type, &object::id>
            >
         >,
         ordered_non_unique< tag<by_matrix_level_status>,
            composite_key<
               matrix_rooms_object,
               member<matrix_rooms_object, matrix_id_type, &matrix_rooms_object::matrix>,
               member<matrix_rooms_object, uint8_t, &matrix_rooms_object::matrix_level>,
               member<matrix_rooms_object, uint8_t, &matrix_rooms_object::status>,
               member< object, object_id_type, &object::id>
            >
         >
      >
   >;
   using matrix_rooms_index = generic_index<matrix_rooms_object, matrix_rooms_multi_index_type>;


   /* 
    * END MATRIX RELOADING OBJECTS
    */

} } // graphene::chain

FC_REFLECT_DERIVED( graphene::chain::flipcoin_object, (graphene::db::object),
	(bettor)
	(caller)
	(winner)
	(bet)
	(nonce)
	(expiration)
	(status)
) 
FC_REFLECT_DERIVED(graphene::chain::lottery_goods_object, (graphene::db::object),
   (owner)
   (participants)
   (total_participants)
   (ticket_price)
   (expiration)
   (latency_sec)
   (status)
   (img_url)
   (description)
   (winner)
   (winner_contacts)
)  
FC_REFLECT_DERIVED(graphene::chain::matrix_object, (graphene::db::object),
   (start_block_number)
   (finish_block_number)
   (status)
   (amount)
   (total_amount)
   (last_120k_amount)
   (matrix_level_1_price)
   (matrix_level_2_price)
   (matrix_level_3_price)
   (matrix_level_4_price)
   (matrix_level_5_price)
   (matrix_level_6_price)
   (matrix_level_7_price)
   (matrix_level_8_price)
   (matrix_level_1_prize)
   (matrix_level_2_prize)
   (matrix_level_3_prize)
   (matrix_level_4_prize)
   (matrix_level_5_prize)
   (matrix_level_6_prize)
   (matrix_level_7_prize)
   (matrix_level_8_prize)
   (matrix_level_1_cells)
   (matrix_level_2_cells)
   (matrix_level_3_cells)
   (matrix_level_4_cells)
   (matrix_level_5_cells)
   (matrix_level_6_cells)
   (matrix_level_7_cells)
   (matrix_level_8_cells)
)  

FC_REFLECT_DERIVED(graphene::chain::matrix_rooms_object, (graphene::db::object),
   (matrix)
   (matrix_player)
   (matrix_level)
   (total_cells)
   (status)
   (cells)
) 