#include "../app/app_collection.h"

#include "menu_object.h"
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

winp::utility::error_code winp::menu::action_item::set_text(const std::wstring &value, const std::function<void(action_item &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_text_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

const std::wstring &winp::menu::action_item::get_text(const std::function<void(const std::wstring &)> &callback) const{
	return *compute_or_post_task_inside_thread_context([=]{
		return &pass_return_ref_value_to_callback(callback, &text_);
	}, (callback != nullptr), &text_);
}

winp::utility::error_code winp::menu::action_item::create_(){
	return (text_.empty() ? utility::error_code::menu_item_empty_text : item::create_());
}

HMENU winp::menu::action_item::create_handle_(menu::object &parent){
	MENUITEMINFOW info{};
	auto index = fill_basic_info_(parent, info);

	info.fMask |= MIIM_STRING;
	info.dwTypeData = text_.data();
	info.cch = static_cast<UINT>(text_.size());

	return ((InsertMenuItemW(parent.get_handle(), index, TRUE, &info) == FALSE) ? nullptr : parent.get_handle());
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
