#pragma once

namespace winp::utility{
	enum class error_code{
		nil,
		multiple_main_apps,
		outside_thread_context,
		thread_context_mismatch,
		thread_get_message_failure,
		index_out_of_range,
		event_cannot_be_bound,
	};
}
