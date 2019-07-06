#include "../app/app_object.h"

#include "menu_object.h"
#include "menu_link_item.h"

winp::menu::link_item::link_item(){
	target_ = std::make_shared<popup>(*this);
}

winp::menu::link_item::link_item(ui::tree &parent)
	: link_item(parent, static_cast<std::size_t>(-1)){}

winp::menu::link_item::link_item(ui::tree &parent, std::size_t index)
	: link_item(){
	set_parent(&parent, index);
}

winp::menu::link_item::~link_item() = default;

winp::menu::popup *winp::menu::link_item::get_target(const std::function<void(popup *)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, dynamic_cast<popup *>(target_.get()));
	}, (callback != nullptr), nullptr);
}

winp::utility::error_code winp::menu::link_item::set_text(const std::wstring &value, const std::function<void(link_item &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_text_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

const std::wstring &winp::menu::link_item::get_text(const std::function<void(const std::wstring &)> &callback) const{
	return *compute_or_post_task_inside_thread_context([=]{
		return &pass_return_ref_value_to_callback(callback, &text_);
	}, (callback != nullptr), &text_);
}

winp::utility::error_code winp::menu::link_item::fill_info_(MENUITEMINFOW &info){
	auto popup_target = dynamic_cast<popup *>(target_.get());
	if (popup_target == nullptr || popup_target->create() != utility::error_code::nil)
		return utility::error_code::action_could_not_be_completed;

	info.fMask |= (MIIM_SUBMENU | MIIM_STRING);
	info.hSubMenu = popup_target->get_handle();
	info.dwTypeData = text_.data();
	info.cch = static_cast<UINT>(text_.size());

	return utility::error_code::nil;
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

	return ((SetMenuItemInfoW(handle_, get_insertion_index_(), TRUE, &info) == FALSE) ? utility::error_code::action_could_not_be_completed : utility::error_code::nil);
}

winp::menu::wrapped_link_item::wrapped_link_item(menu::object &parent, std::size_t index)
	: link_item(parent, index){
	MENUITEMINFOW info{
		sizeof(MENUITEMINFOW),
		(MIIM_STRING | MIIM_FTYPE | MIIM_STATE | MIIM_BITMAP | MIIM_SUBMENU)
	};

	GetMenuItemInfoW((handle_ = parent.get_handle()), static_cast<UINT>(index), TRUE, &info);
	if (0u < info.cch){//Retrieve text
		text_.resize(info.cch);

		MENUITEMINFOW text_info{
			sizeof(MENUITEMINFOW),
			MIIM_STRING
		};

		text_info.dwTypeData = text_.data();
		text_info.cch = (info.cch + 1u);

		GetMenuItemInfoW(handle_, static_cast<UINT>(index), TRUE, &text_info);
	}

	bitmap_ = info.hbmpItem;
	states_ = info.fState;
	types_ = info.fType;

	target_ = std::make_shared<wrapped_popup>(*this, info.hSubMenu);
}

winp::menu::wrapped_link_item::~wrapped_link_item(){
	destruct_();
}

winp::utility::error_code winp::menu::wrapped_link_item::create_(){
	return utility::error_code::nil;
}

winp::utility::error_code winp::menu::wrapped_link_item::destroy_(){
	handle_ = nullptr;
	return utility::error_code::nil;
}
