#include "../app/app_collection.h"

#include "menu_link_item_with_popup.h"

winp::menu::system_link_item_with_popup::system_link_item_with_popup(tree &parent)
	: system_link_item_with_popup(parent, static_cast<std::size_t>(-1)){}

winp::menu::system_link_item_with_popup::system_link_item_with_popup(tree &parent, std::size_t index)
	: m_base_type(parent.get_thread(), false){
	if (auto system_parent = dynamic_cast<menu::system_object *>(&parent); system_parent != nullptr)
		m_base_type::target_ = (m_base_type::popup_ = std::make_shared<ui::object_collection<menu::system_popup>>(*system_parent->get_target_window())).get();
	else
		target_ = (popup_ = std::make_shared<ui::object_collection<menu::system_popup>>(thread_)).get();
	m_base_type::set_parent(&parent, index);
}

winp::menu::system_link_item_with_popup::~system_link_item_with_popup() = default;
