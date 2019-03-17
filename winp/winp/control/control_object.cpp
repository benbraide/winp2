#include "../app/app_collection.h"

#include "control_object.h"

winp::control::object::~object() = default;

winp::control::object::object(thread::object &thread, const std::wstring &class_name, DWORD common_id)
	: window_surface(thread), class_name_(class_name){
	thread_.init_control(class_name_, common_id);
}

bool winp::control::object::is_dialog_message_(MSG &msg) const{
	return false;
}

const std::wstring &winp::control::object::get_class_name_() const{
	return class_name_;
}

HINSTANCE winp::control::object::get_instance_() const{
	return nullptr;
}
