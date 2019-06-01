#include "../app/app_object.h"

#include "control_object.h"

winp::control::object::object(thread::object &thread, const std::wstring &class_name, DWORD common_id)
	: window_surface(thread), class_name_(class_name){
	styles_ |= WS_TABSTOP;
	thread_.init_control(class_name_, common_id);
	add_event_handler_([this](events::create &e){
		show_();
	});
}

winp::control::object::~object() = default;

bool winp::control::object::is_dialog_message_(MSG &msg) const{
	return (parent_ != nullptr && parent_->is_dialog_message(msg));
}

winp::ui::window_surface::system_menu_type &winp::control::object::get_system_menu_() const{
	throw utility::error_code::not_supported;
}

winp::ui::window_surface::bar_menu_type &winp::control::object::get_menu_bar_() const{
	throw utility::error_code::not_supported;
}

const std::wstring &winp::control::object::get_class_name_() const{
	return class_name_;
}

HINSTANCE winp::control::object::get_instance_() const{
	return nullptr;
}

LRESULT winp::control::object::dispatch_command_(MSG &msg) const{
	return trigger_event_<events::unhandled>(msg, thread_.get_class_entry(get_class_name())).second;
}

LRESULT winp::control::object::dispatch_notification_(MSG &msg) const{
	return trigger_event_<events::unhandled>(msg, thread_.get_class_entry(get_class_name())).second;
}
