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
