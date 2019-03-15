#include "../app/app_collection.h"

#include "menu_object.h"
#include "menu_link_item.h"

winp::menu::link_item::link_item(thread::object &thread)
	: item(thread), target_(nullptr){}

winp::menu::link_item::link_item(popup &target)
	: item(target.get_thread()), target_(&target){}

winp::menu::link_item::link_item(popup &target, tree &parent)
	: link_item(target, parent, static_cast<std::size_t>(-1)){}

winp::menu::link_item::link_item(popup &target, tree &parent, std::size_t index)
	: link_item(target){
	set_parent(&parent, index);
}

winp::menu::link_item::~link_item() = default;

const winp::menu::popup &winp::menu::link_item::get_target(const std::function<void(const popup &)> &callback) const{
	return *compute_or_post_task_inside_thread_context([=]{
		return &pass_return_ref_value_to_callback(callback, target_);
	}, (callback != nullptr), target_);
}

winp::menu::popup &winp::menu::link_item::get_target(const std::function<void(const popup &)> &callback){
	return *compute_or_post_task_inside_thread_context([=]{
		return &pass_return_ref_value_to_callback(callback, target_);
	}, (callback != nullptr), target_);
}

winp::utility::error_code winp::menu::link_item::set_text(const std::wstring &value, const std::function<void(object &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_text_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

const std::wstring &winp::menu::link_item::get_text(const std::function<void(const std::wstring &)> &callback) const{
	return *compute_or_post_task_inside_thread_context([=]{
		return &pass_return_ref_value_to_callback(callback, &text_);
	}, (callback != nullptr), &text_);
}

winp::utility::error_code winp::menu::link_item::create_(){
	if (text_.empty())
		return utility::error_code::menu_item_empty_text;
	return (target_->is_created() ? item::create_() : utility::error_code::menu_link_target_not_created);
}

HMENU winp::menu::link_item::create_handle_(menu::object &parent){
	MENUITEMINFOW info{};
	auto index = fill_basic_info_(parent, info);

	info.fMask |= (MIIM_SUBMENU | MIIM_STRING);
	info.hSubMenu = target_->get_handle();
	info.dwTypeData = text_.data();
	info.cch = static_cast<UINT>(text_.size());

	return ((InsertMenuItemW(parent.get_handle(), index, TRUE, &info) == FALSE) ? nullptr : parent.get_handle());
}

winp::utility::error_code winp::menu::link_item::set_text_(const std::wstring &value){
	text_ = value;
	if (handle_ == nullptr)
		return utility::error_code::nil;

	MENUITEMINFOW info{
		sizeof(MENUITEMINFOW)
	};

	info.fMask = MIIM_STRING;
	info.dwTypeData = text_.data();
	info.cch = static_cast<UINT>(text_.size());

	return ((SetMenuItemInfoW(handle_, id_, FALSE, &info) == FALSE) ? utility::error_code::action_could_not_be_completed : utility::error_code::nil);
}
