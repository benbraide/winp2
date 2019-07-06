#include "../app/app_object.h"

#include "control_group.h"
#include "check_control.h"

winp::control::radio_group::radio_group() = default;

winp::control::radio_group::radio_group(tree &parent)
	: radio_group(parent, static_cast<std::size_t>(-1)){}

winp::control::radio_group::radio_group(tree &parent, std::size_t index){
	set_parent(&parent, index);
}

winp::control::radio_group::~radio_group() = default;

winp::utility::error_code winp::control::radio_group::insert_child_(object &child, std::size_t index){
	if (dynamic_cast<check *>(&child) == nullptr)
		return utility::error_code::check_item_required;
	return tree::insert_child_(child, index);
}
