#include "../app/app_object.h"
#include "../ui/ui_size_and_position_hooks.h"

#include "tab_control.h"

winp::control::tab_page::tab_page(){
	insert_hook<ui::parent_fill_hook>();
	insert_hook<ui::placement_hook>(ui::placement_hook::alignment_type::top_left);

	add_event_handler_([this](events::create &e){
		if (e.is_creating())
			return;

		auto tab_parent = dynamic_cast<tab *>(parent_);
		auto tab_handle = ((tab_parent == nullptr) ? nullptr : tab_parent->get_handle_());

		if (insert_into_tab_(*tab_parent) == utility::error_code::nil){
			set_dimension_((position_.x - 1), (position_.y - 1), 0, 0, 0u, false);//Force refresh
			if (auto active_page = tab_parent->get_active_page_(); active_page == nullptr)
				tab_parent->set_active_page_(*this, -1);
			else if (active_page == this)
				show_();
		}
		else//Error
			window_surface::destroy_();

		tab_parent->show_();
	});

	add_event_handler_([this](events::destroy &e){
		auto tab_parent = dynamic_cast<tab *>(parent_);
		auto tab_handle = ((tab_parent == nullptr) ? nullptr : tab_parent->get_handle_());

		if (tab_handle != nullptr)
			TabCtrl_DeleteItem(tab_handle, get_insertion_index_(tab_handle));
	});
}

winp::control::tab_page::tab_page(ui::tree &parent)
	: tab_page(parent, static_cast<std::size_t>(-1)){}

winp::control::tab_page::tab_page(ui::tree &parent, std::size_t index)
	: tab_page(){
	set_parent(&parent, index);
}

winp::control::tab_page::~tab_page(){
	destruct_();
}

winp::utility::error_code winp::control::tab_page::set_title(const std::wstring &value, const std::function<void(tab_page &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_title_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

const std::wstring &winp::control::tab_page::get_title(const std::function<void(const std::wstring &)> &callback) const{
	return *compute_or_post_task_inside_thread_context([=]{
		return &pass_return_ref_value_to_callback(callback, &title_);
	}, (callback != nullptr), &title_);
}

winp::utility::error_code winp::control::tab_page::set_parent_value_(ui::tree *value, bool changing){
	if (changing){
		if (value != nullptr && dynamic_cast<tab *>(value) == nullptr)
			return utility::error_code::invalid_parent;
		return ui::object::set_parent_value_(value, true);
	}

	destroy_();
	return ui::object::set_parent_value_(value, false);
}

winp::utility::error_code winp::control::tab_page::set_index_value_(std::size_t value, bool changing){
	if (changing || handle_ == nullptr)
		return ui::object::set_index_value_(value, changing);

	auto tab_parent = dynamic_cast<tab *>(parent_);
	auto tab_handle = ((tab_parent == nullptr) ? nullptr : tab_parent->get_handle_());

	if (tab_handle != nullptr)
		TabCtrl_DeleteItem(tab_handle, get_insertion_index_(tab_handle));

	if (auto error_code = insert_into_tab_(*tab_parent); error_code != utility::error_code::nil){
		window_surface::destroy_();
		return error_code;
	}

	return ui::object::set_index_value_(value, false);
}

DWORD winp::control::tab_page::get_filtered_styles_(bool is_extended) const{
	return (is_extended ? 0u : WS_OVERLAPPEDWINDOW);
}

winp::utility::error_code winp::control::tab_page::insert_into_tab_(tab &tab_parent){
	auto tab_handle = tab_parent.get_handle_();
	TCITEMW info{
		(TCIF_TEXT | TCIF_IMAGE | TCIF_PARAM),
		0,											//State
		0,											//State mask
		const_cast<wchar_t *>(title_.c_str()),
		0,											//Text buffer size
		-1,											//Image index
		reinterpret_cast<LPARAM>(this)
	};

	auto insertion_count = TabCtrl_GetItemCount(tab_handle), insertion_index = 0, insertion_offset = 0;
	if (0 < insertion_count){
		TCITEMW item_info{ TCIF_PARAM };
		TabCtrl_GetItem(tab_handle, insertion_index, &item_info);

		tab_parent.traverse_children_of_<item>([&](item &child){
			if (&child == this)
				return false;

			if (item_info.lParam == reinterpret_cast<LPARAM>(&child) && ++insertion_index < insertion_count)
				TabCtrl_GetItem(tab_handle, insertion_index, &item_info);

			return (insertion_index < insertion_count);
		});
	}

	return ((TabCtrl_InsertItem(tab_handle, insertion_index, &info) == -1) ? utility::error_code::action_could_not_be_completed : utility::error_code::nil);
}

winp::utility::error_code winp::control::tab_page::set_title_(const std::wstring &value){
	title_ = value;
	if (handle_ == nullptr)
		return utility::error_code::nil;

	TCITEMW info{
		TCIF_TEXT,
		0,
		0,
		const_cast<wchar_t *>(title_.c_str())
	};

	auto tab_parent = dynamic_cast<tab *>(parent_);
	auto tab_handle = ((tab_parent == nullptr) ? nullptr : tab_parent->get_handle_());

	if (tab_handle != nullptr)
		TabCtrl_SetItem(tab_handle, get_insertion_index_(tab_handle), &info);

	return utility::error_code::nil;
}

LRESULT winp::control::tab_page::deactivate_(){
	if ((trigger_event_<events::deactivate_page>().first & events::object::state_default_prevented) != 0u)
		return TRUE;//Default prevented

	hide_();
	return FALSE;
}

LRESULT winp::control::tab_page::activate_(){
	show_();
	trigger_event_<events::activate_page>();
	return 0;
}

int winp::control::tab_page::get_insertion_index_(HWND tab_handle) const{
	auto insertion_count = TabCtrl_GetItemCount(tab_handle), insertion_index = 0;
	if (insertion_count <= 0)
		return false;

	TCITEMW info{ TCIF_PARAM };
	for (; insertion_index < insertion_count; ++insertion_index){
		TabCtrl_GetItem(tab_handle, insertion_index, &info);
		if (info.lParam == reinterpret_cast<LPARAM>(this))//Matched
			break;
	}

	return insertion_index;
}
