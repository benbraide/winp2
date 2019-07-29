#include "../app/app_object.h"
#include "../ui/ui_window_surface.h"

WORD winp::events::cursor::get_hit_target() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return LOWORD(message_info_.lParam);
}

WORD winp::events::cursor::get_mouse_button() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return HIWORD(message_info_.lParam);
}

void winp::events::cursor::call_default_(){}

const POINT &winp::events::hit_test::get_position() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return position_;
}

bool winp::events::mouse::is_non_client() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;

	if (auto window_context = dynamic_cast<ui::window_surface *>(&context_); window_context != nullptr && window_context->get_handle() == message_info_.hwnd)
		return is_non_client_;

	return false;
}

const POINT &winp::events::mouse::get_position() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return position_;
}

unsigned int winp::events::mouse::get_button() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return button_;
}

bool winp::events::mouse_wheel::is_vertical() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return (message_info_.message == WM_MOUSEWHEEL);
}

SIZE winp::events::mouse_wheel::get_delta() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;

	if (message_info_.message == WM_MOUSEWHEEL)
		return SIZE{ 0, static_cast<int>(static_cast<short>(HIWORD(message_info_.wParam)) / WHEEL_DELTA) };

	return SIZE{ static_cast<int>(static_cast<short>(HIWORD(message_info_.wParam)) / WHEEL_DELTA), 0 };
}

const POINT &winp::events::mouse_drag_begin::get_down_position() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return down_position_;
}

const POINT &winp::events::mouse_drag::get_last_position() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return last_position_;
}

POINT winp::events::mouse_drag::get_offset() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return POINT{ (position_.x - last_position_.x), (position_.y - last_position_.y) };
}

winp::events::mouse_edge_drag::edge_type winp::events::mouse_edge_drag::get_edge() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return edge_;
}

unsigned short winp::events::key::get_virtual_code() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return static_cast<unsigned short>(message_info_.wParam);
}

wchar_t winp::events::key::get_scan_code() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return (reinterpret_cast<const wchar_t *>(&message_info_.lParam))[2];
}

bool winp::events::key::is_extended() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return std::bitset<sizeof(LPARAM) * 8>(message_info_.lParam).test(24);
}

bool winp::events::key::check_key_state(unsigned short key) const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;

	if (key_states_size <= key)
		return false;

	if (key_states_ == nullptr){//Retrieve states
		key_states_ = std::make_unique<BYTE[]>(key_states_size);
		GetKeyboardState(key_states_.get());
	}

	if (key == VK_CAPITAL || key == VK_NUMLOCK || key == VK_SCROLL || key == VK_INSERT)
		return ((key_states_[key] & 1u) != 0u);

	return (key_states_[key] < 0);
}

bool winp::events::key_down::is_first_down() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return !std::bitset<sizeof(LPARAM) * 8>(message_info_.lParam).test(30);
}

WORD winp::events::key_down::get_repeat_count() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return static_cast<WORD>(message_info_.lParam);
}

bool winp::events::key_press::is_first_down() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return !std::bitset<sizeof(LPARAM) * 8>(message_info_.lParam).test(30);
}

bool winp::events::key_press::is_being_released() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return std::bitset<sizeof(LPARAM) * 8>(message_info_.lParam).test(31);
}

bool winp::events::item_check::is_checked() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return (message_info_.wParam != FALSE);
}

bool winp::events::item_check::is_indeterminate() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return (message_info_.wParam == 2);
}

bool winp::events::mouse_hover::is_hovered() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return hovered_;
}
