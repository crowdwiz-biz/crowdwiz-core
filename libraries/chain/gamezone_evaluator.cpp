/*
 * Copyright (c) 2019 CrowdWiz., and contributors.
 *
 * The MIT License
 */

#include <graphene/chain/gamezone_evaluator.hpp>
#include <graphene/chain/gamezone_object.hpp>
#include <graphene/chain/account_object.hpp>
#include <graphene/chain/database.hpp>
#include <graphene/chain/hardfork.hpp>

namespace graphene
{
namespace chain
{

share_type cut_fee_game(share_type a, uint16_t p)
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

void_result flipcoin_bet_evaluator::do_evaluate(const flipcoin_bet_operation &op)
{
    try
    {
      database& d = db();
		const account_object& bettor    	= op.bettor(d);
		const asset_object&   asset_type    = op.bet.asset_id(d);

      FC_ASSERT( op.bet.asset_id == asset_id_type(), "Price must be in core asset");

     	bool insufficient_balance = d.get_balance( bettor, asset_type ).amount >= op.bet.amount;
     	FC_ASSERT( insufficient_balance,
                 "Insufficient Balance: ${balance}, unable to bet '${total_bet}' from account '${a}'", 
                 ("a",bettor.name)("total_bet",d.to_pretty_string(op.bet))("balance",d.to_pretty_string(d.get_balance(bettor, asset_type))) );
        return void_result();
    }
    FC_CAPTURE_AND_RETHROW((op))
}

object_id_type flipcoin_bet_evaluator::do_apply(const flipcoin_bet_operation &op)
{
    try
    {
        database& d = db();
		db().adjust_balance( op.bettor, -op.bet );

        const auto& new_flipcoin_object = db().create<flipcoin_object>([&](flipcoin_object &obj) {
            obj.bettor = op.bettor;
            obj.bet = op.bet;
            obj.status = 0;
			obj.nonce = op.nonce;
            obj.expiration = d.head_block_time() + fc::hours(1);
        });
        return new_flipcoin_object.id;
    }
    FC_CAPTURE_AND_RETHROW((op))
}

void_result flipcoin_call_evaluator::do_evaluate(const flipcoin_call_operation &op)
{
    try
    {
        database& d = db();
		flipcoin = &op.flipcoin(d);

		const account_object& caller    	= op.caller(d);
		const asset_object&   asset_type    = op.bet.asset_id(d);

     	bool insufficient_balance = d.get_balance( caller, asset_type ).amount >= op.bet.amount;
     	FC_ASSERT( insufficient_balance,
                 "Insufficient Balance: ${balance}, unable to bet '${total_bet}' from account '${a}'", 
                 ("a",caller.name)("total_bet",d.to_pretty_string(op.bet))("balance",d.to_pretty_string(d.get_balance(caller, asset_type))) );
		FC_ASSERT(flipcoin->status == 0, "flipcoin already called");
		FC_ASSERT(flipcoin->expiration >= d.head_block_time(), "flipcoin already called");
		FC_ASSERT(flipcoin->bet.amount == op.bet.amount, "flipcoin bet amount must match");

        return void_result();
    }
    FC_CAPTURE_AND_RETHROW((op))
}

void_result flipcoin_call_evaluator::do_apply(const flipcoin_call_operation &op)
{
    try
    {
        database& d = db();
		db().adjust_balance( op.caller, -op.bet );
		d.modify(
			d.get(op.flipcoin),
			[&]( flipcoin_object& f )
			{
				f.status = 1;
				f.caller = op.caller;
                f.expiration = d.head_block_time() + fc::seconds(10);
			}
		);
        return void_result();
    }
    FC_CAPTURE_AND_RETHROW((op))
}

void_result lottery_goods_create_lot_evaluator::do_evaluate( const lottery_goods_create_lot_operation& op )
{ try {
   const database& d = db();

   FC_ASSERT( (op.total_participants>0)&&(op.total_participants<=5000));
   FC_ASSERT( (op.latency_sec>0)&&(op.latency_sec<=100));
   FC_ASSERT( op.ticket_price.asset_id == asset_id_type(), "Price must be in core asset");
   auto head_time = db().head_block_time();
   if ( head_time >= HARDFORK_CWD4_TIME ) {
      auto& owner = d.get<account_object>(op.owner);
      FC_ASSERT( owner.referral_status_type == 4, "You must have Infinity contract");
   }
   else { //for all Infinity
      account_id_type admin_whitelist = account_id_type(27);
      auto& admin = d.get<account_object>(admin_whitelist);
      auto& owner = d.get<account_object>(op.owner);
      FC_ASSERT( admin.whitelisted_accounts.find(owner.id) != admin.whitelisted_accounts.end(), "Not admin account!" );
   }
   return void_result();

}  FC_CAPTURE_AND_RETHROW( (op) ) }

object_id_type lottery_goods_create_lot_evaluator::do_apply( const lottery_goods_create_lot_operation& op )
{ try {
   auto head_time = db().head_block_time();
   time_point_sec expiration = HARDFORK_CWD4_TIME + fc::seconds(2592000);
   if ( head_time >= HARDFORK_CWD4_TIME ) {
      expiration = head_time + fc::seconds(2592000);
   }
   const auto& new_lottery_goods_object = db().create<lottery_goods_object>([&](lottery_goods_object& obj){
       obj.owner = op.owner;
       obj.total_participants = op.total_participants;
       obj.ticket_price = op.ticket_price;
       obj.latency_sec = op.latency_sec;
       obj.img_url = op.img_url;
       obj.description = op.description;
       obj.status = 0;
       obj.expiration = expiration;
   });
   
   return new_lottery_goods_object.id;
} FC_CAPTURE_AND_RETHROW( (op) ) }



void_result lottery_goods_buy_ticket_evaluator::do_evaluate( const lottery_goods_buy_ticket_operation& op )
{ try {
   const database& d = db();
   const asset_object& asset_type = op.ticket_price.asset_id(d);
   const account_object& from_account = op.participant(d);
   lot_obj = &op.lot_id(d); 
   FC_ASSERT( op.ticket_price.asset_id == asset_id_type(), "Price must be in core asset");
   
   bool insufficient_balance = d.get_balance( from_account, asset_type ).amount >= op.ticket_price.amount;
   FC_ASSERT( insufficient_balance,
              "Insufficient Balance: ${balance}, unable to buy_ticket '${total_ticket}' from account '${a}'", 
              ("a",from_account.name)("total_transfer",d.to_pretty_string(op.ticket_price))("balance",d.to_pretty_string(d.get_balance(from_account, asset_type))) );
   
   FC_ASSERT( lot_obj->status==0 );
   FC_ASSERT( lot_obj->participants.size()<lot_obj->total_participants );
   FC_ASSERT( lot_obj->ticket_price.amount == op.ticket_price.amount, "lottery_goods ticket_price amount must match");
      
   return void_result();
}  FC_CAPTURE_AND_RETHROW( (op) ) }


void_result lottery_goods_buy_ticket_evaluator::do_apply( const lottery_goods_buy_ticket_operation& op )
{ try {
   database& d = db();
   lot_obj = &op.lot_id(d);

   if ( db().head_block_time() >= HARDFORK_CWD2_TIME ) {
      const account_object& from_account = op.participant(d);
      from_account.statistics(d).update_pv(lot_obj->ticket_price.amount, from_account, d);
   }

   db().adjust_balance( op.participant, -op.ticket_price );

   d.modify(d.get(op.lot_id), [&]( lottery_goods_object& lg_obj )
   {
      lg_obj.participants.emplace_back(op.participant);
      //lg_obj.participants.insert({lg_obj.participants.size(), op.participant});
      //lg_obj.push_back(op.participant);
      if(lot_obj->participants.size()>=lot_obj->total_participants)
      {
         lg_obj.status = 1;
         lg_obj.expiration = d.head_block_time() + fc::seconds(lg_obj.latency_sec);
      }
   });
   
   return void_result();
} FC_CAPTURE_AND_RETHROW( (op) ) }

 
 
void_result lottery_goods_send_contacts_evaluator::do_evaluate( const lottery_goods_send_contacts_operation& op )
{ try {
   database& d = db();
   lot_obj = &op.lot_id(d);
   
   FC_ASSERT( lot_obj->winner==op.winner );
   FC_ASSERT( lot_obj->owner==op.owner );
   FC_ASSERT( !lot_obj->winner_contacts );
   
   //FC_ASSERT( op.winner.winner_contacts );
   
   return void_result();
}  FC_CAPTURE_AND_RETHROW( (op) ) }
 
 
void_result lottery_goods_send_contacts_evaluator::do_apply( const lottery_goods_send_contacts_operation& op )
{ try {
   database& d = db();

   d.modify(
      d.get(op.lot_id),
      [&]( lottery_goods_object& lg_obj )
      {
         lg_obj.winner_contacts = op.winner_contacts;
      }
   );    
   
   return void_result();
} FC_CAPTURE_AND_RETHROW( (op) ) }



void_result lottery_goods_confirm_delivery_evaluator::do_evaluate( const lottery_goods_confirm_delivery_operation& op )
{ try {
   database& d = db();
   lot_obj = &op.lot_id(d);
   
   FC_ASSERT( lot_obj->status==2 );
   FC_ASSERT( lot_obj->winner==op.winner );
   FC_ASSERT( lot_obj->owner==op.owner );

   
   return void_result();
}  FC_CAPTURE_AND_RETHROW( (op) ) }


void_result lottery_goods_confirm_delivery_evaluator::do_apply( const lottery_goods_confirm_delivery_operation& op )
{ try {
   database& d = db();
   lot_obj = &op.lot_id(d);

   asset profit = lot_obj->ticket_price;
   profit.amount = profit.amount*lot_obj->total_participants;
   
   db().adjust_balance( lot_obj->owner, profit );
      
   d.modify(
      d.get(op.lot_id),
      [&]( lottery_goods_object& lg_obj )
      {
         lg_obj.status = 3;
      }
   );   
   
   return void_result();
} FC_CAPTURE_AND_RETHROW( (op) ) }


void_result matrix_open_room_evaluator::do_evaluate( const matrix_open_room_operation& op )
{ try {
   database& d = db();
   matrix = &op.matrix_id(d);
   const account_object& player = op.player(d);
   const auto& player_stats = player.statistics(d);
   uint8_t current_level = player_stats.matrix_active_levels;
   if (player.referral_status_type*2 > current_level) {
      current_level = player.referral_status_type*2;
   }
	const asset_object&   asset_type    = d.get_core_asset();

   // IS MATRIX ACTIVE
   FC_ASSERT(matrix, "This Matrix does not exist");
   FC_ASSERT(matrix->start_block_number <= d.head_block_num(), "Matrix not active (too early)");
   FC_ASSERT(matrix->finish_block_number >= d.head_block_num(), "Matrix not active (too late)");
   // Check player
   if (player_stats.matrix == op.matrix_id && player_stats.matrix_rooms_opened > 0) {
      FC_ASSERT(op.matrix_level <= current_level, "You are not allowed for this level");
   }
   else {
      FC_ASSERT(current_level >= op.matrix_level || op.matrix_level == 1 , "You are not allowed for this level");
   }
   // Check opened Rooms
   uint8_t player_room_status = 0;
   if (player_stats.matrix == op.matrix_id && player_stats.matrix_cells_opened >= 2) {
      player_room_status = 1;
   }
   auto& rooms_idx = d.get_index_type<matrix_rooms_index>().indices().get<by_matrix_level_player_status>();
   auto itr = rooms_idx.find( boost::make_tuple(op.matrix_id, op.player, op.matrix_level, player_room_status) );
   FC_ASSERT( itr == rooms_idx.end(), "You already have open room at this level");

   if (player.referral_status_type == 0) {
      auto& player_rooms_idx = d.get_index_type<matrix_rooms_index>().indices().get<by_matrix_player_status>();
      auto client_itr = player_rooms_idx.find( boost::make_tuple(op.matrix_id, op.player, player_room_status) );
      FC_ASSERT( client_itr == player_rooms_idx.end(), "You can have no more than one room! Please upgrade your Status!");
   }

   // Check prices
   FC_ASSERT( op.level_price.asset_id == asset_id_type(), "Price must be in core asset");

   if (op.matrix_level == 1) { 
      FC_ASSERT( matrix->matrix_level_1_price == op.level_price.amount, "level_price 1 amount must match");
   }
   if (op.matrix_level == 2) { 
      FC_ASSERT( matrix->matrix_level_2_price == op.level_price.amount, "level_price 2 amount must match");
   }
   if (op.matrix_level == 3) { 
      FC_ASSERT( matrix->matrix_level_3_price == op.level_price.amount, "level_price 3 amount must match");
   }
   if (op.matrix_level == 4) { 
      FC_ASSERT( matrix->matrix_level_4_price == op.level_price.amount, "level_price 4 amount must match");
   }
   if (op.matrix_level == 5) { 
      FC_ASSERT( matrix->matrix_level_5_price == op.level_price.amount, "level_price 5 amount must match");
   }
   if (op.matrix_level == 6) { 
      FC_ASSERT( matrix->matrix_level_6_price == op.level_price.amount, "level_price 6 amount must match");
   }
   if (op.matrix_level == 7) { 
      FC_ASSERT( matrix->matrix_level_7_price == op.level_price.amount, "level_price 7 amount must match");
   }
   if (op.matrix_level == 8) { 
      FC_ASSERT( matrix->matrix_level_8_price == op.level_price.amount, "level_price 8 amount must match");
   }
   bool insufficient_balance = d.get_balance( player, asset_type ).amount >= op.level_price.amount;
   FC_ASSERT( insufficient_balance, "Infufficent Balance!");

   return void_result();
}  FC_CAPTURE_AND_RETHROW( (op) ) }


object_id_type matrix_open_room_evaluator::do_apply( const matrix_open_room_operation& op )
{ try {
   database& d = db();
   matrix = &op.matrix_id(d);
   auto head_time = db().head_block_time();

   db().adjust_balance( op.player, -op.level_price.amount );

   if ( head_time >= HARDFORK_CWD2_TIME ) {
      const account_object& from_account = op.player(d);
      from_account.statistics(d).update_pv(op.level_price.amount, from_account, d);
   }

   d.modify(*matrix, [&](matrix_object &m) {
      m.amount+=op.level_price.amount;
      m.total_amount+=op.level_price.amount;
      if(d.head_block_num() > matrix->finish_block_number - 120000) {
         m.last_120k_amount+=op.level_price.amount;
      }
   });   
   const account_object& player = op.player(d);
   const account_object& ref_01 = d.get(player.referrer);
   const auto& player_stats = player.statistics(d);
   const auto& ref_01_stats = ref_01.statistics(d);

   uint8_t max_level = 1;
   if (player.referral_status_type*2 > max_level) {
      max_level=player.referral_status_type*2;
   }
   if ( head_time >= HARDFORK_CWD4_TIME  && op.matrix_id > matrix_id_type(2)) { //Open rooms
      if (player_stats.matrix != op.matrix_id || player_stats.matrix_rooms_opened == 0) {
         d.modify(player_stats, [&](account_statistics_object &s) {
            s.matrix = op.matrix_id;
            s.matrix_active_levels = max_level;
            s.matrix_cells_opened = 2;
            s.matrix_rooms_opened = 0;
         });
      }
   }
   else {
      if (player_stats.matrix != op.matrix_id || player_stats.matrix_rooms_opened == 0) {
         d.modify(player_stats, [&](account_statistics_object &s) {
            s.matrix = op.matrix_id;
            s.matrix_active_levels = max_level;
            s.matrix_cells_opened = 0;
            s.matrix_rooms_opened = 0;
         });
      }      
   }

   if (ref_01_stats.matrix == op.matrix_id && ref_01_stats.matrix_cells_opened == 1) {
      d.modify(ref_01_stats, [&](account_statistics_object &s) {
         s.matrix_cells_opened++;
      });

      auto& own_rooms_idx = d.get_index_type<matrix_rooms_index>().indices().get<by_matrix_player_status>();
      auto ref_01_own_itr = own_rooms_idx.equal_range( boost::make_tuple(op.matrix_id, ref_01.id, 0) );
      vector<matrix_rooms_id_type> rooms_to_update;
      std::for_each(ref_01_own_itr.first, ref_01_own_itr.second,
                    [&] (const matrix_rooms_object& mro) {
                        rooms_to_update.emplace_back(mro.id);

                    });
      for( auto& r : rooms_to_update ) {
         const matrix_rooms_object& mro =  d.get(r);
            d.modify(mro, [&]( matrix_rooms_object& mr )
            {
               mr.status = 1;
            });
      }
   }


   if (ref_01_stats.matrix == op.matrix_id && ref_01_stats.matrix_rooms_opened > 0 && ref_01_stats.matrix_cells_opened == 0) {
      d.modify(ref_01_stats, [&](account_statistics_object &s) {
         s.matrix_cells_opened++;
      });
   }


   bool need_to_fill = false;
   matrix_rooms_id_type room_id_to_fill;

   if ( head_time >= HARDFORK_CWD4_TIME && op.matrix_id > matrix_id_type(2) ) { //Do not link to firstliner
         auto& other_rooms_idx = d.get_index_type<matrix_rooms_index>().indices().get<by_matrix_level_status>();
         auto other_itr = other_rooms_idx.find( boost::make_tuple(op.matrix_id, op.matrix_level, 1) );    
         if ( other_itr != other_rooms_idx.end()) {
            room_id_to_fill = other_itr->id;
            need_to_fill = true;
         }
   }
   else {
      uint8_t room_status = 0;
      if (ref_01_stats.matrix_cells_opened >= 2) {
         room_status = 1;
      }
      auto& rooms_idx = d.get_index_type<matrix_rooms_index>().indices().get<by_matrix_level_player_status>();
      auto ref_01_itr = rooms_idx.find( boost::make_tuple(op.matrix_id, ref_01.id, op.matrix_level, room_status) );
      
      if ( ref_01_itr != rooms_idx.end()) {
            room_id_to_fill = ref_01_itr->id;
            need_to_fill = true;
         }
      else {
         auto& other_rooms_idx = d.get_index_type<matrix_rooms_index>().indices().get<by_matrix_level_status>();
         auto other_itr = other_rooms_idx.find( boost::make_tuple(op.matrix_id, op.matrix_level, 1) );    
         if ( other_itr != other_rooms_idx.end()) {
            room_id_to_fill = other_itr->id;
            need_to_fill = true;
         }
      }
   }
 
   if(need_to_fill) {
      const matrix_rooms_object& room_to_fill = d.get(room_id_to_fill);
      d.modify(room_to_fill, [&]( matrix_rooms_object& mr )
      {
         mr.cells.emplace_back(op.player);
         matrix_cell_filled_operation cell_filled;
         cell_filled.player = mr.matrix_player;
         cell_filled.matrix_room = mr.id;
         cell_filled.matrix_level = op.matrix_level;
         cell_filled.cell_player = op.player;
         d.push_applied_operation( cell_filled );
         if(mr.cells.size()>=mr.total_cells)
         {
            mr.status = 2;
            share_type level_prize;

            if (op.matrix_level == 1) {
               level_prize = matrix->matrix_level_1_prize;
            }
            if (op.matrix_level == 2) {
               level_prize = matrix->matrix_level_2_prize;
            }
            if (op.matrix_level == 3) {
               level_prize = matrix->matrix_level_3_prize;
            }
            if (op.matrix_level == 4) {
               level_prize = matrix->matrix_level_4_prize;
            }
            if (op.matrix_level == 5) {
               level_prize = matrix->matrix_level_5_prize;
            }
            if (op.matrix_level == 6) {
               level_prize = matrix->matrix_level_6_prize;
            }
            if (op.matrix_level == 7) {
               level_prize = matrix->matrix_level_7_prize;
            }
            if (op.matrix_level == 8) {
               level_prize = matrix->matrix_level_8_prize;
            }

            d.adjust_balance( mr.matrix_player, level_prize);

            matrix_room_closed_operation room_closed;
            room_closed.player = mr.matrix_player;
            room_closed.matrix_room = mr.id;
            room_closed.matrix_level = op.matrix_level;
            room_closed.level_reward = level_prize;
            d.push_applied_operation( room_closed );

            d.modify(*matrix, [&](matrix_object &m) {
               m.amount-=level_prize;
            }); 

            const account_object& matrix_winner = d.get(mr.matrix_player);
            const auto& matrix_winner_stats = matrix_winner.statistics(d);

            uint8_t winner_active_levels = op.matrix_level+1;
            if (winner_active_levels > 8) {
               winner_active_levels = 8;
            }
            matrix_log( "matrix_open_room_evaluator: matrix_winner matrix_active_levels: ${b}", ("b", winner_active_levels) );

            if (matrix_winner_stats.matrix_active_levels < winner_active_levels) {
               d.modify(matrix_winner_stats, [&](account_statistics_object &s) {
                  s.matrix_active_levels = winner_active_levels;
               });
            }
         }
      });
   }

   // Open new Room
   uint8_t new_room_status = 0;
   if (player_stats.matrix_cells_opened >= 2) {
      new_room_status = 1;
   }
  
   uint8_t new_room_cells;

   if (op.matrix_level == 1) {
      new_room_cells = matrix->matrix_level_1_cells;
   }
   if (op.matrix_level == 2) {
      new_room_cells = matrix->matrix_level_2_cells;
   }
   if (op.matrix_level == 3) {
      new_room_cells = matrix->matrix_level_3_cells;
   }
   if (op.matrix_level == 4) {
      new_room_cells = matrix->matrix_level_4_cells;
   }
   if (op.matrix_level == 5) {
      new_room_cells = matrix->matrix_level_5_cells;
   }
   if (op.matrix_level == 6) {
      new_room_cells = matrix->matrix_level_6_cells;
   }
   if (op.matrix_level == 7) {
      new_room_cells = matrix->matrix_level_7_cells;
   }
   if (op.matrix_level == 8) {
      new_room_cells = matrix->matrix_level_8_cells;
   }
    
   const auto& new_matrix_room_object = db().create<matrix_rooms_object>([&](matrix_rooms_object& obj){
      obj.matrix = op.matrix_id;
      obj.matrix_player = op.player;
      obj.matrix_level = op.matrix_level;
      obj.total_cells = new_room_cells;
      obj.status = new_room_status;
   });

   d.modify(player_stats, [&](account_statistics_object &s) {
      s.matrix_rooms_opened += 1;
   });

   return new_matrix_room_object.id;

} FC_CAPTURE_AND_RETHROW( (op) ) }

}} // namespace chain