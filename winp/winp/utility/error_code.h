#pragma once

namespace winp::utility{
	enum class error_code{
		nil,
		multiple_main_apps,
		outside_thread_context,
		thread_get_message_failure,
	};
}
