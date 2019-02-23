#pragma once

namespace winp::utility{
	enum class error_code{
		nil,
		not_supported,
		action_prevented,
		multiple_main_apps,
		multiple_app_threads,
		duplicate_entry,
		outside_thread_context,
		thread_context_mismatch,
		thread_get_message_failure,
		index_out_of_range,
		event_cannot_be_bound,
	};
}
