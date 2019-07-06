#include "../app/app_object.h"

#include "custom_non_window.h"

winp::non_window::custom::custom() = default;

winp::non_window::custom::custom(tree &parent)
	: custom(parent, static_cast<std::size_t>(-1)){}

winp::non_window::custom::custom(tree &parent, std::size_t index){
	set_parent(&parent, index);
}

winp::non_window::custom::~custom(){
	destruct();
}
