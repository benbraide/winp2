#include "../app/app_collection.h"

WORD winp::events::cursor::get_hit_target() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return LOWORD(message_.lParam);
}

WORD winp::events::cursor::get_mouse_button() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return HIWORD(message_.lParam);
}

bool winp::events::cursor::call_default_(){
	return true;
}

const POINT &winp::events::hit_test::get_position() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return position_;
}

bool winp::events::mouse::is_non_client() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
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

bool winp::events::mouse_leave::is_non_client() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return true;
}

bool winp::events::mouse_move::is_non_client() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return (message_.message == WM_NCMOUSEMOVE);
}

bool winp::events::mouse_down::is_non_client() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return is_non_client_;
}

bool winp::events::mouse_up::is_non_client() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return is_non_client_;
}

bool winp::events::mouse_dbl_clk::is_non_client() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return is_non_client_;
}

bool winp::events::mouse_wheel::is_vertical() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return (message_.message == WM_MOUSEWHEEL);
}

SIZE winp::events::mouse_wheel::get_delta() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;

	if (message_.message == WM_MOUSEWHEEL)
		return SIZE{ 0, static_cast<int>(static_cast<short>(HIWORD(message_.wParam)) / WHEEL_DELTA) };

	return SIZE{ static_cast<int>(static_cast<short>(HIWORD(message_.wParam)) / WHEEL_DELTA), 0 };
}

unsigned short winp::events::key::get_virtual_code() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return static_cast<unsigned short>(message_.wParam);
}

wchar_t winp::events::key::get_scan_code() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return (reinterpret_cast<const wchar_t *>(&message_.lParam))[2];
}

bool winp::events::key::is_extended() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return std::bitset<sizeof(LPARAM) * 8>(message_.lParam).test(24);
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
	return !std::bitset<sizeof(LPARAM) * 8>(message_.lParam).test(30);
}

WORD winp::events::key_down::get_repeat_count() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return static_cast<WORD>(message_.lParam);
}

bool winp::events::key_press::is_first_down() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return !std::bitset<sizeof(LPARAM) * 8>(message_.lParam).test(30);
}

bool winp::events::key_press::is_being_released() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return std::bitset<sizeof(LPARAM) * 8>(message_.lParam).test(31);
}

bool winp::events::item_check::is_checked() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return (original_message_.wParam != FALSE);
}

bool winp::events::item_check::is_indeterminate() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return (original_message_.wParam == 2);
}
