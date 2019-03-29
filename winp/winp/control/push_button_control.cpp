#include "../app/app_collection.h"

#include "push_button_control.h"

winp::control::push_button::push_button()
	: push_button(app::collection::get_main()->get_thread()){}

winp::control::push_button::push_button(thread::object &thread)
	: button(thread){}

winp::control::push_button::push_button(tree &parent)
	: push_button(parent, static_cast<std::size_t>(-1)){}

winp::control::push_button::push_button(tree &parent, std::size_t index)
	: push_button(parent.get_thread()){
	set_parent(&parent, index);
}

winp::control::push_button::~push_button() = default;

LRESULT winp::control::push_button::dispatch_command(MSG &msg) const{
	if (msg.wParam == BN_CLICKED)
		return trigger_event_<events::click>(msg, thread_.get_class_entry(get_class_name())).second;
	return button::dispatch_command(msg);
}

winp::utility::error_code winp::control::push_button::click() const{
	if (handle_ == nullptr)
		return utility::error_code::action_could_not_be_completed;

	if (auto window_ancestor = get_ancestor<ui::window_surface>(); window_ancestor == nullptr){
		MSG msg{ nullptr, WM_COMMAND, 0, reinterpret_cast<LPARAM>(handle_) };
		trigger_event_<events::click>(msg, thread_.get_class_entry(get_class_name()));
	}
	else
		SendMessageW(window_ancestor->get_handle(), WM_COMMAND, 0, reinterpret_cast<LPARAM>(handle_));

	return utility::error_code::nil;
}

DWORD winp::control::push_button::get_persistent_styles_(bool is_extended) const{
	return (button::get_persistent_styles_(is_extended) | (is_extended ? 0u : BS_PUSHBUTTON));
}

winp::control::default_push_button::default_push_button()
	: default_push_button(app::collection::get_main()->get_thread()){}

winp::control::default_push_button::default_push_button(thread::object &thread)
	: push_button(thread){}

winp::control::default_push_button::default_push_button(tree &parent)
	: default_push_button(parent, static_cast<std::size_t>(-1)){}

winp::control::default_push_button::default_push_button(tree &parent, std::size_t index)
	: default_push_button(parent.get_thread()){
	set_parent(&parent, index);
}

winp::control::default_push_button::~default_push_button() = default;

DWORD winp::control::default_push_button::get_persistent_styles_(bool is_extended) const{
	return (button::get_persistent_styles_(is_extended) | (is_extended ? 0u : BS_DEFPUSHBUTTON));
}
