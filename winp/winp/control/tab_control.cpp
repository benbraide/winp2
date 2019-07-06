#include "../app/app_object.h"

#include "tab_control.h"

winp::control::tab::tab()
	: object(WC_TABCONTROLW, ICC_TAB_CLASSES){
	insert_hook<ui::parent_fill_hook>();
	insert_hook<ui::placement_hook>(ui::placement_hook::alignment_type::top_left);
}

winp::control::tab::tab(tree &parent)
	: tab(parent, static_cast<std::size_t>(-1)){}

winp::control::tab::tab(tree &parent, std::size_t index)
	: tab(){
	set_parent(&parent, index);
}

winp::control::tab::~tab() = default;

winp::utility::error_code winp::control::tab::set_active_page(tab_page &value, const std::function<void(tab &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([pvalue = &value, callback, this]{
		return pass_return_value_to_callback(callback, *this, set_active_page_(*pvalue, -1));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::control::tab_page *winp::control::tab::get_active_page(const std::function<void(tab_page *)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, get_active_page_());
	}, (callback != nullptr), nullptr);
}

winp::utility::error_code winp::control::tab::do_insert_child_(ui::object &child, std::size_t index){
	if (dynamic_cast<tab_page *>(&child) == nullptr)
		return utility::error_code::invalid_child;
	return tree::do_insert_child_(child, index);
}

SIZE winp::control::tab::get_client_size_() const{
	RECT dimension{};

	GetWindowRect(handle_, &dimension);
	TabCtrl_AdjustRect(handle_, FALSE, &dimension);

	return SIZE{ (dimension.right - dimension.left), (dimension.bottom - dimension.top) };
}

SIZE winp::control::tab::get_current_client_size_() const{
	return get_client_size_();
}

POINT winp::control::tab::get_client_offset_() const{
	if (handle_ == nullptr)
		return window_surface::get_client_offset_();

	RECT dimension{};
	GetWindowRect(handle_, &dimension);

	auto computed_dimension = dimension;
	TabCtrl_AdjustRect(handle_, FALSE, &computed_dimension);

	return POINT{ (computed_dimension.left - dimension.left), (computed_dimension.top - dimension.top) };
}

POINT winp::control::tab::get_client_start_offset_() const{
	return get_client_offset_();
}

LRESULT winp::control::tab::dispatch_notification_(MSG &msg) const{
	switch (reinterpret_cast<NMHDR *>(msg.lParam)->code){
	case TCN_SELCHANGING:
	{
		if (auto current_page = get_active_page_(); current_page != nullptr)
			return current_page->deactivate_();
		break;
	}
	case TCN_SELCHANGE:
	{
		if (auto current_page = get_active_page_(); current_page != nullptr)
			return current_page->activate_();
		break;
	}
	default:
		break;
	}

	return 0;
}

winp::utility::error_code winp::control::tab::set_active_page_(tab_page &value, int index){
	if (handle_ == nullptr)
		return utility::error_code::not_supported;

	if (auto active_page = get_active_page_(); active_page != nullptr){
		if (active_page == &value)//No changes
			return utility::error_code::nil;
		
		if (active_page->deactivate_() != FALSE)
			return utility::error_code::action_prevented;
	}

	TabCtrl_SetCurSel(handle_, ((index < 0) ? value.get_insertion_index_(handle_) : index));
	value.activate_();

	return utility::error_code::nil;
}

winp::control::tab_page *winp::control::tab::get_active_page_() const{
	auto current_index = ((handle_ == nullptr) ? -1 : TabCtrl_GetCurSel(handle_));
	if (current_index < 0)
		return nullptr;

	TCITEMW info{ TCIF_PARAM };
	TabCtrl_GetItem(handle_, current_index, &info);

	return reinterpret_cast<tab_page *>(info.lParam);
}
