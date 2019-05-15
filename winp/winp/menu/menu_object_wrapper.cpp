#include "../ui/ui_window_surface.h"
#include "../app/app_object.h"

#include "menu_item_wrapper.h"

winp::menu::popup_wrapper::popup_wrapper()
	: popup_wrapper(app::object::get_thread()){}

winp::menu::popup_wrapper::popup_wrapper(thread::object &thread)
	: popup(thread){}

winp::menu::popup_wrapper::popup_wrapper(thread::object &thread, HMENU handle)
	: popup_wrapper(thread){
	resolve_handle_(handle);
}

winp::menu::popup_wrapper::~popup_wrapper(){
	destruct();
}

winp::utility::error_code winp::menu::popup_wrapper::set_handle(HMENU value, const std::function<void(popup_wrapper &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_handle_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::menu::popup_wrapper::create_(){
	return utility::error_code::not_supported;
}

winp::utility::error_code winp::menu::popup_wrapper::destroy_(){
	if (handle_ == nullptr)
		return utility::error_code::nil;
	
	auto items_copy = items_;//Extend items lifetime
	items_.clear();

	handle_ = nullptr;
	thread_.get_item_manager().remove_menu(*this);

	return utility::error_code::nil;
}

winp::utility::error_code winp::menu::popup_wrapper::do_erase_child_(ui::object &child, std::size_t index){
	if (auto error_code = popup::do_erase_child_(child, index); error_code != utility::error_code::nil)
		return error_code;

	if (!items_.empty())
		items_.erase(&child);

	return utility::error_code::nil;
}

winp::utility::error_code winp::menu::popup_wrapper::set_handle_(HMENU value){
	destroy_();
	resolve_handle_(value);
	return utility::error_code::nil;
}

void winp::menu::popup_wrapper::resolve_handle_(HMENU handle){
	if ((handle_ = handle) == nullptr)
		return;

	thread_.get_item_manager().add_menu(*this);
	MENUITEMINFOW info{
		sizeof(MENUITEMINFOW),
		(MIIM_ID | MIIM_STRING | MIIM_FTYPE | MIIM_STATE | MIIM_BITMAP | MIIM_CHECKMARKS | MIIM_SUBMENU)
	};

	std::shared_ptr<menu::item> item;
	auto count = GetMenuItemCount(handle);

	for (auto index = 0; index < count; ++index){
		if (GetMenuItemInfoW(handle, index, TRUE, &info) == FALSE)
			continue;

		if ((item = create_item_(info, index)) != nullptr)
			items_[item.get()] = item;
	}
}

std::shared_ptr<winp::menu::item> winp::menu::popup_wrapper::create_item_(const MENUITEMINFOW &info, UINT index){
	if (info.hSubMenu != nullptr)
		return std::make_shared<menu::link_item_wrapper>(*this, index, info);

	if ((info.fType & MFT_SEPARATOR) == 0u)
		return std::make_shared<menu::action_item_wrapper>(*this, index, info);
	
	return std::make_shared<menu::separator_wrapper>(*this, index, info);
}

winp::menu::system_popup_wrapper::system_popup_wrapper()
	: system_popup_wrapper(app::object::get_thread()){}

winp::menu::system_popup_wrapper::system_popup_wrapper(thread::object &thread)
	: popup_wrapper(thread){}

winp::menu::system_popup_wrapper::system_popup_wrapper(thread::object &thread, HMENU handle)
	: system_popup_wrapper(thread){
	resolve_handle_(handle);
}

winp::menu::system_popup_wrapper::system_popup_wrapper(ui::window_surface &target_window)
	: system_popup_wrapper(target_window.get_thread()){
	resolve_handle_(GetSystemMenu((target_window_ = &target_window)->get_handle(), FALSE));
}

winp::menu::system_popup_wrapper::~system_popup_wrapper() = default;

std::shared_ptr<winp::menu::item> winp::menu::system_popup_wrapper::create_item_(const MENUITEMINFOW &info, UINT index){
	if (info.hSubMenu != nullptr)
		return std::make_shared<menu::system_link_item_wrapper>(*this, index, info);

	if ((info.fType & MFT_SEPARATOR) == 0u)
		return std::make_shared<menu::system_action_item_wrapper>(*this, index, info);

	return std::make_shared<menu::system_separator_wrapper>(*this, index, info);
}
