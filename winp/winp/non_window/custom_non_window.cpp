#include "../app/app_object.h"

#include "custom_non_window.h"

winp::non_window::custom::custom()
	: custom(app::object::get_thread()){}

winp::non_window::custom::custom(thread::object &thread)
	: non_window_surface(thread){}

winp::non_window::custom::custom(tree &parent)
	: custom(parent, static_cast<std::size_t>(-1)){}

winp::non_window::custom::custom(tree &parent, std::size_t index)
	: custom(parent.get_thread()){
	set_parent(&parent, index);
}

winp::non_window::custom::~custom(){
	destruct();
}
