#include "../app/app_object.h"

#include "tab_control.h"

winp::control::tab_page::tab_page()
	: tab_page(app::object::get_thread()){}

winp::control::tab_page::tab_page(thread::object &thread)
	: window_surface(thread){
	insert_hook<ui::parent_fill_hook>();
	insert_hook<ui::placement_hook>(ui::placement_hook::alignment_type::top_left);

	add_event_handler_([this](events::destroy &e){
		auto tab_parent = dynamic_cast<tab *>(parent_);
		if (tab_parent != nullptr && tab_parent->is_created()){
			TabCtrl_DeleteItem(tab_parent->get_handle_(), actual_index_);

			WINDOWPOS info{ nullptr, nullptr, 0, 0, tab_parent->size_.cx, tab_parent->size_.cy, SWP_NOMOVE };
			thread_.send_message(*tab_parent, WM_WINDOWPOSCHANGED, 0, reinterpret_cast<LPARAM>(&info));
		}
		
		actual_index_ = -1;
	});
}

winp::control::tab_page::tab_page(ui::tree &parent)
	: tab_page(parent, static_cast<std::size_t>(-1)){}

winp::control::tab_page::tab_page(ui::tree &parent, std::size_t index)
	: tab_page(parent.get_thread()){
	set_parent(&parent, index);
}

winp::control::tab_page::~tab_page(){
	destruct();
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

winp::utility::error_code winp::control::tab_page::create_(){
	auto tab_parent = dynamic_cast<tab *>(parent_);
	if (tab_parent == nullptr || !tab_parent->is_created())
		return utility::error_code::parent_not_created;

	if (auto error_code = window_surface::create_(); error_code != utility::error_code::nil || 0 <= actual_index_)
		return error_code;

	TCITEMW info{
		(TCIF_TEXT | TCIF_IMAGE | TCIF_PARAM),
		0,
		0,
		const_cast<wchar_t *>(title_.c_str()),
		0,
		-1,
		reinterpret_cast<LPARAM>(this)
	};

	std::list<tab_page *> children_after;
	auto is_after = false, increment_index = true;

	actual_index_ = 0;
	tab_parent->traverse_children_of_<tab_page>([&](tab_page &child){
		if (&child == this){
			is_after = true;
			increment_index = false;
		}
		else if (!child.is_created_())
			increment_index = false;

		if (increment_index)
			++actual_index_;

		if (is_after){
			children_after.push_back(&child);
			if (0 <= child.actual_index_)
				++child.actual_index_;
		}

		return true;
	});

	if (TabCtrl_InsertItem(tab_parent->get_handle_(), actual_index_, &info) == -1){
		window_surface::destroy_();
		for (auto child_after : children_after){
			if (0 <= child_after->actual_index_)
				--child_after->actual_index_;
		}

		return utility::error_code::action_could_not_be_completed;
	}

	WINDOWPOS msg_info{ nullptr, nullptr, 0, 0, tab_parent->size_.cx, tab_parent->size_.cy, SWP_NOMOVE };
	thread_.send_message(*tab_parent, WM_WINDOWPOSCHANGED, 0, reinterpret_cast<LPARAM>(&msg_info));

	if (auto active_page = tab_parent->get_active_page_(); active_page == this){
		active_page->hide_();
		show_();
	}
	else{//Hide this; Show active
		hide_();
		active_page->show_();
	}

	return utility::error_code::nil;
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
	if (changing)
		return ui::object::set_index_value_(value, true);

	if (is_created_()){
		auto tab_parent = dynamic_cast<tab *>(parent_);
		if (tab_parent != nullptr && tab_parent->is_created()){
			TabCtrl_DeleteItem(tab_parent->get_handle_(), actual_index_);
			actual_index_ = -1;
			create_();
		}
	}

	return ui::object::set_index_value_(value, false);
}

DWORD winp::control::tab_page::get_filtered_styles_(bool is_extended) const{
	return (is_extended ? 0u : WS_OVERLAPPEDWINDOW);
}

winp::utility::error_code winp::control::tab_page::set_title_(const std::wstring &value){
	title_ = value;
	TCITEMW info{
		TCIF_TEXT,
		0,
		0,
		const_cast<wchar_t *>(title_.c_str())
	};

	auto tab_parent = dynamic_cast<tab *>(parent_);
	if (tab_parent != nullptr && tab_parent->is_created())
		TabCtrl_SetItem(tab_parent->get_handle_(), actual_index_, &info);

	return utility::error_code::nil;
}

LRESULT winp::control::tab_page::deactivate_page_(){
	if ((trigger_event_<events::deactivate_page>().first & events::object::state_default_prevented) != 0u)
		return TRUE;//Default prevented

	hide_();
	return FALSE;
}

LRESULT winp::control::tab_page::activate_page_(){
	show_();
	trigger_event_<events::activate_page>();
	return 0;
}
