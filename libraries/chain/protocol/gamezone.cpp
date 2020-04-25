/*
 * Copyright (c) 2019 CrowdWiz., and contributors.
 *
 * The MIT License
 */

#include <graphene/chain/protocol/gamezone.hpp>

namespace graphene { namespace chain {

    share_type cut_fee_gamezone(share_type a, uint16_t p)
    {
    if( a == 0 || p == 0 )
        return 0;
    if( p == GRAPHENE_100_PERCENT )
        return a;

    fc::uint128 r(a.value);
    r *= p;
    r /= GRAPHENE_100_PERCENT;
    return r.to_uint64();
    }

    void lottery_goods_create_lot_operation::validate()const
    {
    //const auto& ch_parameters = d.get_global_properties().parameters;

    FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
    FC_ASSERT( ticket_price.amount>0 );
    FC_ASSERT( fc::raw::pack_size(img_url)<257 );
    FC_ASSERT( fc::raw::pack_size(description)<2001 );
    }

    void lottery_goods_buy_ticket_operation::validate()const
    {
    FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
    FC_ASSERT( ticket_price.amount>0 );
    }

    share_type lottery_goods_buy_ticket_operation::calculate_fee(const fee_parameters_type& k) const
    {
    share_type core_fee_required;
    core_fee_required = cut_fee_gamezone(ticket_price.amount, k.lottery_goods_fee_percent);
    return core_fee_required; 
    }

    void lottery_goods_send_contacts_operation::validate()const
    {
    FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
    }

    void lottery_goods_confirm_delivery_operation::validate()const
    {
    FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
    }

    void lottery_goods_win_operation::validate()const
    {
    FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
    }

    void lottery_goods_loose_operation::validate()const
    {
    FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
    }

    void lottery_goods_refund_operation::validate()const
    {
    FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
    FC_ASSERT( ticket_price.amount>0 );
    }

    void flipcoin_bet_operation::validate()const
    {
    FC_ASSERT( fee.amount >= 0 );
    FC_ASSERT( bet.amount > 0 );
    }

    void flipcoin_call_operation::validate()const
    {
    FC_ASSERT( fee.amount >= 0 );
    FC_ASSERT( bet.amount >= 0 );
    }
    void flipcoin_win_operation::validate()const
    {
    FC_ASSERT( fee.amount >= 0 );
    FC_ASSERT( payout.amount >= 0 );
    FC_ASSERT( referral_payout.amount >= 0 );
    }
    void flipcoin_loose_operation::validate()const
    {
    FC_ASSERT( fee.amount >= 0 );
    FC_ASSERT( bet.amount >= 0 );
    }
    void flipcoin_cancel_operation::validate()const
    {
    FC_ASSERT( fee.amount >= 0 );
    FC_ASSERT( bet.amount >= 0 );
    }


    void matrix_open_room_operation::validate()const
    {
    FC_ASSERT( fee.amount >= 0, "Fee amount should not be negative" );
    FC_ASSERT( level_price.amount>=0 );
    FC_ASSERT( matrix_level >= 1, "Matrix level must be greater than 0" );
    FC_ASSERT( matrix_level <= 8, "Matrix level must be lower than 9" );

    }

    share_type matrix_open_room_operation::calculate_fee(const fee_parameters_type& k) const
    {
    share_type core_fee_required;
    core_fee_required = cut_fee_gamezone(level_price.amount, k.matrix_fee_percent);
    return core_fee_required; 
    }

    void matrix_room_closed_operation::validate()const
    {
    FC_ASSERT( fee.amount >= 0 );
    FC_ASSERT( level_reward.amount >= 0 );
    }

    void matrix_cell_filled_operation::validate()const
    {
    FC_ASSERT( fee.amount >= 0 );
    }

} } // graphene::chain