#include "../ui/ui_window_surface.h"
#include "../app/app_collection.h"

#include "menu_object_wrapper.h"

winp::menu::popup_wrapper::popup_wrapper(thread::object &thread, HMENU handle)
	: popup(thread){
	resolve_handle_(handle);
}

winp::menu::popup_wrapper::~popup_wrapper(){
	destruct();
}

winp::utility::error_code winp::menu::popup_wrapper::create_(){
	return utility::error_code::not_supported;
}

winp::utility::error_code winp::menu::popup_wrapper::destroy_(){
	auto items_copy = items_;//Extend items lifetime

	items_.clear();
	handle_ = nullptr;

	return utility::error_code::nil;
}

winp::utility::error_code winp::menu::popup_wrapper::do_erase_child_(ui::object &child, std::size_t index){
	if (auto error_code = popup::do_erase_child_(child, index); error_code != utility::error_code::nil)
		return error_code;

	if (!items_.empty())
		items_.erase(&child);

	return utility::error_code::nil;
}

void winp::menu::popup_wrapper::resolve_handle_(HMENU handle){
	if (handle == nullptr)
		return;

	MENUITEMINFOW info{
		sizeof(MENUITEMINFOW),
		(MIIM_ID | MIIM_STRING | MIIM_FTYPE | MIIM_STATE | MIIM_BITMAP | MIIM_CHECKMARKS | MIIM_SUBMENU)
	};

	std::shared_ptr<menu::item> item;
	auto count = GetMenuItemCount(handle);

	for (auto index = 0; index < count; ++index){
		if (GetMenuItemInfoW(handle, index, TRUE, &info) == FALSE)
			continue;

		if (info.hSubMenu != nullptr)
			item = std::make_shared<menu::link_item_wrapper>(*this, index, info);
		else if ((info.fType & MFT_SEPARATOR) == 0u)
			item = std::make_shared<menu::action_item_wrapper>(*this, index, info);
		else//Separator
			item = std::make_shared<menu::separator_wrapper>(*this, index, info);

		if (item != nullptr)
			items_[item.get()] = item;
	}
}
