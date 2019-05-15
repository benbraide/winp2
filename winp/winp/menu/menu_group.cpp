#include "../app/app_object.h"

#include "menu_group.h"
#include "menu_check_item.h"

winp::menu::radio_group::radio_group()
	: radio_group(app::object::get_thread()){}

winp::menu::radio_group::radio_group(thread::object &thread)
	: tree(thread){}

winp::menu::radio_group::radio_group(tree &parent)
	: radio_group(parent, static_cast<std::size_t>(-1)){}

winp::menu::radio_group::radio_group(tree &parent, std::size_t index)
	: radio_group(parent.get_thread()){
	set_parent(&parent, index);
}

winp::menu::radio_group::~radio_group() = default;

winp::utility::error_code winp::menu::radio_group::insert_child_(object &child, std::size_t index){
	if (dynamic_cast<check_item *>(&child) == nullptr)
		return utility::error_code::check_item_required;
	return tree::insert_child_(child, index);
}

UINT winp::menu::radio_group::get_types_(std::size_t index) const{
	return (tree::get_types_(index) | MFT_RADIOCHECK);
}
