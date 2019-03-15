#include "../app/app_collection.h"

#include "menu_link_item_with_popup.h"

winp::menu::link_item_with_popup::link_item_with_popup()
	: link_item_with_popup(app::collection::get_main()->get_thread()){}

winp::menu::link_item_with_popup::link_item_with_popup(thread::object &thread)
	: link_item(thread){
	target_ = (popup_ = std::make_shared<ui::object_collection<menu::popup>>(thread)).get();
	set_popup_item_link_();
}

winp::menu::link_item_with_popup::link_item_with_popup(tree &parent)
	: link_item_with_popup(parent, static_cast<std::size_t>(-1)){}

winp::menu::link_item_with_popup::link_item_with_popup(tree &parent, std::size_t index)
	: link_item_with_popup(parent.get_thread()){
	set_parent(&parent, index);
}

winp::menu::link_item_with_popup::~link_item_with_popup(){
	destruct();
}

winp::utility::error_code winp::menu::link_item_with_popup::create_(){
	popup_->create();
	if (auto error_code = link_item::create_(); error_code != utility::error_code::nil){
		popup_->destroy();
		return error_code;
	}

	return utility::error_code::nil;
}

winp::utility::error_code winp::menu::link_item_with_popup::destroy_(){
	if (auto error_code = link_item::destroy_(); error_code != utility::error_code::nil)
		return error_code;

	popup_->destroy();
	return utility::error_code::nil;
}

winp::utility::error_code winp::menu::link_item_with_popup::set_parent_value_(ui::tree *value, bool changing){
	auto error_code = link_item::set_parent_value_(value, changing);
	if (!changing){
		auto object_parent = dynamic_cast<menu::object *>(parent_);
		popup_->set_system_state(object_parent != nullptr && object_parent->is_system());
	}

	return error_code;
}
