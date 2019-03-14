#include "../app/app_collection.h"

#include "menu_object_wrapper.h"
#include "menu_item_wrapper.h"

winp::menu::action_item_wrapper::action_item_wrapper(menu::object &parent, std::size_t index, const MENUITEMINFOW &info)
	: action_item(parent.get_thread()){
	set_parent(&parent, index);
	resolve_info_(parent, static_cast<UINT>(index), info);
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

	auto index = static_cast<UINT>(get_index_());
	if (GetMenuItemInfoW(object_parent->get_handle(), index, TRUE, &info) == FALSE)
		return utility::error_code::action_could_not_be_completed;

	resolve_info_(*object_parent, index, info);
	return utility::error_code::nil;
}

winp::utility::error_code winp::menu::action_item_wrapper::destroy_(){
	if (handle_ != nullptr){
		handle_ = nullptr;
		thread_.get_item_manager().remove_generated_item_id(*this);
	}

	return utility::error_code::nil;
}

void winp::menu::action_item_wrapper::resolve_info_(menu::object &parent, UINT index, const MENUITEMINFOW &info){
	if (0u < info.cch){//Retrieve text
		text_.resize(info.cch);

		MENUITEMINFOW text_info{
			sizeof(MENUITEMINFOW),
			MIIM_STRING
		};

		text_info.dwTypeData = text_.data();
		text_info.cch = (info.cch + 1u);

		GetMenuItemInfoW(parent.get_handle(), index, TRUE, &text_info);
	}

	id_ = info.wID;
	handle_ = parent.get_handle();
	bitmap_ = info.hbmpItem;

	states_ = info.fState;
	types_ = info.fType;

	thread_.get_item_manager().add_generated_item_id(*this);
}

winp::menu::link_item_wrapper::link_item_wrapper(menu::object &parent, std::size_t index, const MENUITEMINFOW &info)
	: link_item(parent.get_thread()){
	target_ = (popup_ = std::make_shared<ui::object_collection<menu::popup_wrapper>>(thread_)).get();
	set_parent(&parent, index);
	resolve_info_(parent, static_cast<UINT>(index), info);
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

	auto index = static_cast<UINT>(get_index_());
	if (GetMenuItemInfoW(object_parent->get_handle(), index, TRUE, &info) == FALSE)
		return utility::error_code::action_could_not_be_completed;

	resolve_info_(*object_parent, index, info);
	return utility::error_code::nil;
}

winp::utility::error_code winp::menu::link_item_wrapper::destroy_(){
	if (handle_ != nullptr){
		handle_ = nullptr;
		popup_->destroy();
		thread_.get_item_manager().remove_generated_item_id(*this);
	}

	return utility::error_code::nil;
}

void winp::menu::link_item_wrapper::resolve_info_(menu::object &parent, UINT index, const MENUITEMINFOW &info){
	if (0u < info.cch){//Retrieve text
		text_.resize(info.cch);

		MENUITEMINFOW text_info{
			sizeof(MENUITEMINFOW),
			MIIM_STRING
		};

		text_info.dwTypeData = text_.data();
		text_info.cch = (info.cch + 1u);

		GetMenuItemInfoW(parent.get_handle(), index, TRUE, &text_info);
	}

	id_ = info.wID;
	handle_ = parent.get_handle();
	bitmap_ = info.hbmpItem;

	states_ = info.fState;
	types_ = info.fType;

	if (info.hSubMenu != nullptr)//Wrap sub-menu
		popup_->set_handle(info.hSubMenu, parent.is_system());

	thread_.get_item_manager().add_generated_item_id(*this);
}

winp::menu::check_item_wrapper::check_item_wrapper(menu::object &parent, std::size_t index, const MENUITEMINFOW &info)
	: check_item(parent.get_thread()){
	set_parent(&parent, index);
	resolve_info_(parent, static_cast<UINT>(index), info);
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

	auto index = static_cast<UINT>(get_index_());
	if (GetMenuItemInfoW(object_parent->get_handle(), index, TRUE, &info) == FALSE)
		return utility::error_code::action_could_not_be_completed;

	resolve_info_(*object_parent, index, info);
	return utility::error_code::nil;
}

winp::utility::error_code winp::menu::check_item_wrapper::destroy_(){
	if (handle_ != nullptr){
		handle_ = nullptr;
		thread_.get_item_manager().remove_generated_item_id(*this);
	}

	return utility::error_code::nil;
}

void winp::menu::check_item_wrapper::resolve_info_(menu::object &parent, UINT index, const MENUITEMINFOW &info){
	if (0u < info.cch){//Retrieve text
		text_.resize(info.cch);

		MENUITEMINFOW text_info{
			sizeof(MENUITEMINFOW),
			MIIM_STRING
		};

		text_info.dwTypeData = text_.data();
		text_info.cch = (info.cch + 1u);

		GetMenuItemInfoW(parent.get_handle(), index, TRUE, &text_info);
	}

	id_ = info.wID;
	handle_ = parent.get_handle();
	bitmap_ = info.hbmpItem;

	states_ = info.fState;
	types_ = info.fType;

	checked_bitmap_ = info.hbmpChecked;
	unchecked_bitmap_ = info.hbmpUnchecked;

	thread_.get_item_manager().add_generated_item_id(*this);
}

winp::menu::separator_wrapper::separator_wrapper(menu::object &parent, std::size_t index, const MENUITEMINFOW &info)
	: separator(parent.get_thread()){
	set_parent(&parent, index);
	resolve_info_(parent, static_cast<UINT>(index), info);
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

	auto index = static_cast<UINT>(get_index_());
	if (GetMenuItemInfoW(object_parent->get_handle(), index, TRUE, &info) == FALSE)
		return utility::error_code::action_could_not_be_completed;

	resolve_info_(*object_parent, index, info);
	return utility::error_code::nil;
}

winp::utility::error_code winp::menu::separator_wrapper::destroy_(){
	if (handle_ != nullptr){
		handle_ = nullptr;
		thread_.get_item_manager().remove_generated_item_id(*this);
	}

	return utility::error_code::nil;
}

void winp::menu::separator_wrapper::resolve_info_(menu::object &parent, UINT index, const MENUITEMINFOW &info){
	id_ = info.wID;
	handle_ = parent.get_handle();
	bitmap_ = info.hbmpItem;

	states_ = info.fState;
	types_ = info.fType;

	thread_.get_item_manager().add_generated_item_id(*this);
}
