#pragma once
#include <graphene/chain/protocol/operations.hpp>
#include <graphene/chain/evaluator.hpp>
#include <graphene/chain/database.hpp>

namespace graphene { namespace chain {

	class create_p2p_adv_evaluator : public evaluator<create_p2p_adv_evaluator>
	{
		public:
			typedef create_p2p_adv_operation operation_type;

			void_result do_evaluate( const create_p2p_adv_operation& op );
			object_id_type do_apply( const create_p2p_adv_operation& op );
	};

	class edit_p2p_adv_evaluator : public evaluator<edit_p2p_adv_evaluator>
	{
		public:
			typedef edit_p2p_adv_operation operation_type;

			void_result do_evaluate( const edit_p2p_adv_operation& op );
			void_result do_apply( const edit_p2p_adv_operation& op );
	};

	class clear_p2p_adv_black_list_evaluator : public evaluator<clear_p2p_adv_black_list_evaluator>
	{
		public:
			typedef clear_p2p_adv_black_list_operation operation_type;

			void_result do_evaluate( const clear_p2p_adv_black_list_operation& op );
			void_result do_apply( const clear_p2p_adv_black_list_operation& op );
	};

	class remove_from_p2p_adv_black_list_evaluator : public evaluator<remove_from_p2p_adv_black_list_evaluator>
	{
		public:
			typedef remove_from_p2p_adv_black_list_operation operation_type;

			void_result do_evaluate( const remove_from_p2p_adv_black_list_operation& op );
			void_result do_apply( const remove_from_p2p_adv_black_list_operation& op );
	};

	class create_p2p_order_evaluator : public evaluator<create_p2p_order_evaluator>
	{
		public:
			typedef create_p2p_order_operation operation_type;

			void_result do_evaluate( const create_p2p_order_operation& op );
			object_id_type do_apply( const create_p2p_order_operation& op );
	};

	class cancel_p2p_order_evaluator : public evaluator<cancel_p2p_order_evaluator>
	{
		public:
			typedef cancel_p2p_order_operation operation_type;

			void_result do_evaluate( const cancel_p2p_order_operation& op );
			void_result do_apply( const cancel_p2p_order_operation& op );
	};

	class call_p2p_order_evaluator : public evaluator<call_p2p_order_evaluator>
	{
		public:
			typedef call_p2p_order_operation operation_type;

			void_result do_evaluate( const call_p2p_order_operation& op );
			void_result do_apply( const call_p2p_order_operation& op );
	};

	class payment_p2p_order_evaluator : public evaluator<payment_p2p_order_evaluator>
	{
		public:
			typedef payment_p2p_order_operation operation_type;

			void_result do_evaluate( const payment_p2p_order_operation& op );
			void_result do_apply( const payment_p2p_order_operation& op );
	};

	class release_p2p_order_evaluator : public evaluator<release_p2p_order_evaluator>
	{
		public:
			typedef release_p2p_order_operation operation_type;

			void_result do_evaluate( const release_p2p_order_operation& op );
			void_result do_apply( const release_p2p_order_operation& op );
	};

	class open_p2p_dispute_evaluator : public evaluator<open_p2p_dispute_evaluator>
	{
		public:
			typedef open_p2p_dispute_operation operation_type;

			void_result do_evaluate( const open_p2p_dispute_operation& op );
			void_result do_apply( const open_p2p_dispute_operation& op );
	};

	class reply_p2p_dispute_evaluator : public evaluator<reply_p2p_dispute_evaluator>
	{
		public:
			typedef reply_p2p_dispute_operation operation_type;

			void_result do_evaluate( const reply_p2p_dispute_operation& op );
			void_result do_apply( const reply_p2p_dispute_operation& op );
	};

	class resolve_p2p_dispute_evaluator : public evaluator<resolve_p2p_dispute_evaluator>
	{
		public:
			typedef resolve_p2p_dispute_operation operation_type;

			void_result do_evaluate( const resolve_p2p_dispute_operation& op );
			void_result do_apply( const resolve_p2p_dispute_operation& op );
	};
} } // graphene::chain