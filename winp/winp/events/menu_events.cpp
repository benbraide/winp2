#include "../app/app_object.h"

bool winp::events::menu::should_call_default_() const{
	return (default_callback_ != nullptr && message_info_.hwnd != nullptr && message_info_.message < WM_APP && (states_ & state_default_prevented) == 0u);
}

const POINT &winp::events::context_menu_base::get_position() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return position_;
}

bool winp::events::context_menu_base::should_call_default_() const{
	return false;
}

winp::menu::popup &winp::events::context_menu::get_popup() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return popup_;
}
