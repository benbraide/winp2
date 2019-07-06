#include "../app/app_object.h"

#include "split_button_control.h"

winp::control::split_button::split_button() = default;

winp::control::split_button::split_button(tree &parent)
	: split_button(parent, static_cast<std::size_t>(-1)){}

winp::control::split_button::split_button(tree &parent, std::size_t index){
	set_parent(&parent, index);
}

winp::control::split_button::~split_button() = default;

DWORD winp::control::split_button::get_persistent_styles_(bool is_extended) const{
	return (push_button::get_persistent_styles_(is_extended) | (is_extended ? 0u : BS_SPLITBUTTON));
}

POINT winp::control::split_button::get_context_menu_position_() const{
	auto dimension = get_absolute_dimension_();
	return POINT{ dimension.left, dimension.bottom };
}

LRESULT winp::control::split_button::dispatch_notification_(MSG &msg) const{
	if (reinterpret_cast<NMHDR *>(msg.lParam)->code == BCN_DROPDOWN)
		return SendMessageW(handle_, WINP_WM_SPLIT_BUTTON_DROPDOWN, static_cast<WPARAM>(msg.lParam), static_cast<LPARAM>(MAKELONG(-1, -1)));
	return push_button::dispatch_notification_(msg);
}

SIZE winp::control::split_button::compute_additional_size_() const{
	return SIZE{ 20, 0 };
}

winp::control::default_split_button::default_split_button() = default;

winp::control::default_split_button::default_split_button(tree &parent)
	: default_split_button(parent, static_cast<std::size_t>(-1)){}

winp::control::default_split_button::default_split_button(tree &parent, std::size_t index){
	set_parent(&parent, index);
}

winp::control::default_split_button::~default_split_button() = default;

DWORD winp::control::default_split_button::get_persistent_styles_(bool is_extended) const{
	return (button::get_persistent_styles_(is_extended) | (is_extended ? 0u : BS_DEFSPLITBUTTON));
}
