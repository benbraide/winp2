#include "../app/app_object.h"

#include "non_window_object.h"

winp::non_window::object::object() = default;

winp::non_window::object::object(tree &parent)
	: object(parent, static_cast<std::size_t>(-1)){}

winp::non_window::object::object(tree &parent, std::size_t index){
	set_parent(&parent, index);
}

winp::non_window::object::~object(){
	destruct_();
}
