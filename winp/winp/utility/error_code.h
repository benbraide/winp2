#pragma once

namespace winp::utility{
	enum class error_code{
		nil,
		not_supported,
		action_prevented,
		action_could_not_be_completed,
		object_not_created,
		parent_not_created,
		invalid_parent,
		multiple_main_apps,
		multiple_app_threads,
		duplicate_entry,
		outside_thread_context,
		thread_context_mismatch,
		thread_get_message_failure,
		index_out_of_range,
		event_cannot_be_bound,
		menu_link_target_not_created,
		check_item_required,
		menu_item_empty_text,
		tool_tip_target_not_created,
		tool_tip_target_not_visible,
	};
}
