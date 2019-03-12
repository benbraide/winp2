#include "../app/app_collection.h"

#include "menu_tree.h"
#include "menu_action_item.h"

winp::menu::action_item::action_item()
	: action_item(app::collection::get_main()->get_thread()){}

winp::menu::action_item::action_item(thread::object &thread)
	: item(thread){}

winp::menu::action_item::action_item(tree &parent)
	: action_item(parent, static_cast<std::size_t>(-1)){}

winp::menu::action_item::action_item(tree &parent, std::size_t index)
	: action_item(parent.get_thread()){
	set_parent(&parent, index);
}

winp::menu::action_item::~action_item() = default;

winp::utility::error_code winp::menu::action_item::set_text(const std::wstring &value, const std::function<void(object &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_text_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

const std::wstring &winp::menu::action_item::get_text(const std::function<void(const std::wstring &)> &callback) const{
	return *compute_or_post_task_inside_thread_context([=]{
		return &pass_return_ref_value_to_callback(callback, &text_);
	}, (callback != nullptr), &text_);
}

HMENU winp::menu::action_item::create_handle_(menu::object &parent){
	auto value = item::create_handle_(parent);
	if (value == nullptr || text_.empty())
		return value;//Failed to create handle OR empty text

	MENUITEMINFOW info{
		sizeof(MENUITEMINFOW)
	};

	info.fMask = MIIM_STRING;
	info.dwTypeData = text_.data();
	info.cch = static_cast<UINT>(text_.size());

	SetMenuItemInfoW(handle_, id_, FALSE, &info);//Update menu item
	return value;
}

winp::utility::error_code winp::menu::action_item::set_text_(const std::wstring &value){
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
