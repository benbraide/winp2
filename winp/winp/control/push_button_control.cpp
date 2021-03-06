#include "../app/app_object.h"

#include "push_button_control.h"

winp::control::push_button::push_button() = default;

winp::control::push_button::push_button(tree &parent)
	: push_button(parent, static_cast<std::size_t>(-1)){}

winp::control::push_button::push_button(tree &parent, std::size_t index){
	set_parent(&parent, index);
}

winp::control::push_button::~push_button() = default;

winp::utility::error_code winp::control::push_button::click() const{
	if (handle_ == nullptr){
		MSG msg{ nullptr, WM_COMMAND, 0, reinterpret_cast<LPARAM>(handle_) };
		trigger_event_<events::click>(msg, thread_.get_class_entry(get_class_name()));
	}
	else
		SendMessageW(handle_, BM_CLICK, 0, 0);

	return utility::error_code::nil;
}

DWORD winp::control::push_button::get_persistent_styles_(bool is_extended) const{
	return (button::get_persistent_styles_(is_extended) | (is_extended ? 0u : BS_PUSHBUTTON));
}

LRESULT winp::control::push_button::dispatch_command_(MSG &msg) const{
	if (msg.wParam == BN_CLICKED)
		return trigger_event_<events::click>(msg, thread_.get_class_entry(get_class_name())).second;
	return button::dispatch_command_(msg);
}

winp::control::default_push_button::default_push_button() = default;

winp::control::default_push_button::default_push_button(tree &parent)
	: default_push_button(parent, static_cast<std::size_t>(-1)){}

winp::control::default_push_button::default_push_button(tree &parent, std::size_t index){
	set_parent(&parent, index);
}

winp::control::default_push_button::~default_push_button() = default;

DWORD winp::control::default_push_button::get_persistent_styles_(bool is_extended) const{
	return (button::get_persistent_styles_(is_extended) | (is_extended ? 0u : BS_DEFPUSHBUTTON));
}
