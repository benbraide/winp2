#include "../app/app_object.h"

#include "tab_control.h"

winp::control::tab::tab()
	: tab(app::object::get_thread()){}

winp::control::tab::tab(thread::object &thread)
	: object(thread, WC_TABCONTROLW, ICC_TAB_CLASSES){}

winp::control::tab::tab(tree &parent)
	: tab(parent, static_cast<std::size_t>(-1)){}

winp::control::tab::tab(tree &parent, std::size_t index)
	: tab(parent.get_thread()){
	set_parent(&parent, index);
}

winp::control::tab::~tab() = default;

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
			return current_page->deactivate_page_();
		break;
	}
	case TCN_SELCHANGE:
	{
		if (auto current_page = get_active_page_(); current_page != nullptr)
			return current_page->activate_page_();
		break;
	}
	default:
		break;
	}

	return 0;
}

winp::control::tab_page *winp::control::tab::get_active_page_() const{
	auto current_index = ((handle_ == nullptr) ? -1 : TabCtrl_GetCurSel(handle_));
	if (current_index < 0)
		return nullptr;

	tab_page *current_page = nullptr;
	traverse_children_of_<tab_page>([&](tab_page &child){
		if (--current_index < 0)
			current_page = &child;
		return (current_page == nullptr);
	});

	return current_page;
}
