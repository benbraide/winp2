#include "../app/app_collection.h"

#include "menu_object_wrapper.h"
#include "menu_item_wrapper.h"

winp::menu::action_item_wrapper::action_item_wrapper(menu::object &parent, std::size_t index, const MENUITEMINFOW &info)
	: action_item(parent.get_thread()){
	set_parent(&parent, index);
	resolve_info_(parent, info);
}

winp::menu::action_item_wrapper::~action_item_wrapper(){
	destruct();
}

winp::utility::error_code winp::menu::action_item_wrapper::create_(){
	if (handle_ != nullptr)
		return utility::error_code::nil;

	auto object_parent = dynamic_cast<menu::object *>(parent_);
	if (object_parent == nullptr || !object_parent->is_created())
		return utility::error_code::parent_not_created;

	MENUITEMINFOW info{
		sizeof(MENUITEMINFOW),
		(MIIM_ID | MIIM_STRING | MIIM_FTYPE | MIIM_STATE | MIIM_BITMAP)
	};

	if (GetMenuItemInfoW(object_parent->get_handle(), static_cast<UINT>(get_index_()), TRUE, &info) == FALSE)
		return utility::error_code::action_could_not_be_completed;

	resolve_info_(*object_parent, info);
	return utility::error_code::nil;
}

winp::utility::error_code winp::menu::action_item_wrapper::destroy_(){
	handle_ = nullptr;
	return utility::error_code::nil;
}

void winp::menu::action_item_wrapper::resolve_info_(menu::object &parent, const MENUITEMINFOW &info){
	if (0u < info.cch){//Retrieve text
		text_.resize(info.cch);

		MENUITEMINFOW text_info{
			sizeof(MENUITEMINFOW),
			MIIM_STRING
		};

		text_info.dwTypeData = text_.data();
		text_info.cch = (info.cch + 1u);

		GetMenuItemInfoW(parent.get_handle(), static_cast<UINT>(get_index_()), TRUE, &text_info);
	}

	id_ = info.wID;
	handle_ = parent.get_handle();
	bitmap_ = info.hbmpItem;

	states_ = info.fState;
	types_ = info.fType;
}

winp::menu::link_item_wrapper::link_item_wrapper(menu::object &parent, std::size_t index, const MENUITEMINFOW &info)
	: link_item(parent.get_thread()){
	set_parent(&parent, index);
	resolve_info_(parent, info);
}

winp::menu::link_item_wrapper::~link_item_wrapper(){
	destruct();
}

winp::utility::error_code winp::menu::link_item_wrapper::create_(){
	if (handle_ != nullptr)
		return utility::error_code::nil;

	auto object_parent = dynamic_cast<menu::object *>(parent_);
	if (object_parent == nullptr || !object_parent->is_created())
		return utility::error_code::parent_not_created;

	MENUITEMINFOW info{
		sizeof(MENUITEMINFOW),
		(MIIM_ID | MIIM_STRING | MIIM_FTYPE | MIIM_STATE | MIIM_BITMAP | MIIM_SUBMENU)
	};

	if (GetMenuItemInfoW(object_parent->get_handle(), static_cast<UINT>(get_index_()), TRUE, &info) == FALSE)
		return utility::error_code::action_could_not_be_completed;

	resolve_info_(*object_parent, info);
	return utility::error_code::nil;
}

winp::utility::error_code winp::menu::link_item_wrapper::destroy_(){
	handle_ = nullptr;
	popup_ = nullptr;
	return utility::error_code::nil;
}

void winp::menu::link_item_wrapper::resolve_info_(menu::object &parent, const MENUITEMINFOW &info){
	if (0u < info.cch){//Retrieve text
		text_.resize(info.cch);

		MENUITEMINFOW text_info{
			sizeof(MENUITEMINFOW),
			MIIM_STRING
		};

		text_info.dwTypeData = text_.data();
		text_info.cch = (info.cch + 1u);

		GetMenuItemInfoW(parent.get_handle(), static_cast<UINT>(get_index_()), TRUE, &text_info);
	}

	id_ = info.wID;
	handle_ = parent.get_handle();
	bitmap_ = info.hbmpItem;

	states_ = info.fState;
	types_ = info.fType;

	if (info.hSubMenu != nullptr)//Wrap sub-menu
		target_ = (popup_ = std::make_shared<menu::popup_wrapper>(thread_, info.hSubMenu)).get();
}

winp::menu::check_item_wrapper::check_item_wrapper(menu::object &parent, std::size_t index, const MENUITEMINFOW &info)
	: check_item(parent.get_thread()){
	set_parent(&parent, index);
	resolve_info_(parent, info);
}

winp::menu::check_item_wrapper::~check_item_wrapper(){
	destruct();
}

winp::utility::error_code winp::menu::check_item_wrapper::create_(){
	if (handle_ != nullptr)
		return utility::error_code::nil;

	auto object_parent = dynamic_cast<menu::object *>(parent_);
	if (object_parent == nullptr || !object_parent->is_created())
		return utility::error_code::parent_not_created;

	MENUITEMINFOW info{
		sizeof(MENUITEMINFOW),
		(MIIM_ID | MIIM_STRING | MIIM_FTYPE | MIIM_STATE | MIIM_BITMAP | MIIM_CHECKMARKS)
	};

	if (GetMenuItemInfoW(object_parent->get_handle(), static_cast<UINT>(get_index_()), TRUE, &info) == FALSE)
		return utility::error_code::action_could_not_be_completed;

	resolve_info_(*object_parent, info);
	return utility::error_code::nil;
}

winp::utility::error_code winp::menu::check_item_wrapper::destroy_(){
	handle_ = nullptr;
	return utility::error_code::nil;
}

void winp::menu::check_item_wrapper::resolve_info_(menu::object &parent, const MENUITEMINFOW &info){
	if (0u < info.cch){//Retrieve text
		text_.resize(info.cch);

		MENUITEMINFOW text_info{
			sizeof(MENUITEMINFOW),
			MIIM_STRING
		};

		text_info.dwTypeData = text_.data();
		text_info.cch = (info.cch + 1u);

		GetMenuItemInfoW(parent.get_handle(), static_cast<UINT>(get_index_()), TRUE, &text_info);
	}

	id_ = info.wID;
	handle_ = parent.get_handle();
	bitmap_ = info.hbmpItem;

	states_ = info.fState;
	types_ = info.fType;

	checked_bitmap_ = info.hbmpChecked;
	unchecked_bitmap_ = info.hbmpUnchecked;
}

winp::menu::separator_wrapper::separator_wrapper(menu::object &parent, std::size_t index, const MENUITEMINFOW &info)
	: separator(parent.get_thread()){
	set_parent(&parent, index);
	resolve_info_(parent, info);
}

winp::menu::separator_wrapper::~separator_wrapper(){
	destruct();
}

winp::utility::error_code winp::menu::separator_wrapper::create_(){
	if (handle_ != nullptr)
		return utility::error_code::nil;

	auto object_parent = dynamic_cast<menu::object *>(parent_);
	if (object_parent == nullptr || !object_parent->is_created())
		return utility::error_code::parent_not_created;

	MENUITEMINFOW info{
		sizeof(MENUITEMINFOW),
		(MIIM_ID | MIIM_FTYPE | MIIM_STATE | MIIM_BITMAP)
	};

	if (GetMenuItemInfoW(object_parent->get_handle(), static_cast<UINT>(get_index_()), TRUE, &info) == FALSE)
		return utility::error_code::action_could_not_be_completed;

	resolve_info_(*object_parent, info);
	return utility::error_code::nil;
}

winp::utility::error_code winp::menu::separator_wrapper::destroy_(){
	handle_ = nullptr;
	return utility::error_code::nil;
}

void winp::menu::separator_wrapper::resolve_info_(menu::object &parent, const MENUITEMINFOW &info){
	id_ = info.wID;
	handle_ = parent.get_handle();
	bitmap_ = info.hbmpItem;

	states_ = info.fState;
	types_ = info.fType;
}
