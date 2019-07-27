#include "../app/app_object.h"

#include "ui_non_window_hooks.h"
#include "ui_window_surface.h"

winp::ui::non_window_surface::non_window_surface(){
	insert_hook_<rectangular_non_window_hook>();
	add_event_handler_([this](events::destroy_non_window_handle &e){
		return ((handle_ != nullptr && DeleteObject(handle_) == FALSE) ? utility::error_code::action_could_not_be_completed : utility::error_code::nil);
	});
}

winp::ui::non_window_surface::non_window_surface(tree &parent)
	: non_window_surface(parent, static_cast<std::size_t>(-1)){}

winp::ui::non_window_surface::non_window_surface(tree &parent, std::size_t index)
	: non_window_surface(){
	set_parent(&parent, index);
}

winp::ui::non_window_surface::~non_window_surface(){
	destruct_();
}

winp::utility::error_code winp::ui::non_window_surface::update_handle(const std::function<void(non_window_surface &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, update_handle_());
	}, (callback != nullptr), utility::error_code::nil);
}

HRGN winp::ui::non_window_surface::get_handle(const std::function<void(HRGN)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, get_handle_());
	}, (callback != nullptr), nullptr);
}

HRGN winp::ui::non_window_surface::get_outer_handle(const std::function<void(HRGN)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, get_outer_handle_());
	}, (callback != nullptr), nullptr);
}

HTHEME winp::ui::non_window_surface::get_theme_() const{
	auto window_ancestor = get_first_ancestor_of_<window_surface>(nullptr);
	return ((window_ancestor == nullptr) ? nullptr : window_ancestor->get_theme());
}

std::pair<HDC, HWND> winp::ui::non_window_surface::get_device_context_() const{
	auto window_ancestor = get_first_ancestor_of_<window_surface>(nullptr);
	return ((window_ancestor == nullptr) ? std::make_pair<HDC, HWND>(nullptr, nullptr) : window_ancestor->get_device_context());
}

winp::utility::error_code winp::ui::non_window_surface::create_(){
	if (is_created_())
		return utility::error_code::nil;

	if (is_destructed_())
		return utility::error_code::object_destructed;

	if (parent_ != nullptr)
		parent_->auto_create();

	if ((handle_ = create_handle_()) == nullptr)
		return utility::error_code::action_could_not_be_completed;

	auto hk = find_hook_<ui::non_window_non_client_hook>();
	if (hk != nullptr){//Create non-client handle
		auto result_info = trigger_event_<events::create_non_window_non_client_handle>(nullptr, get_current_size_());
		if ((result_info.first & events::object::state_default_prevented) != 0u){
			destroy_handle_();
			handle_ = nullptr;
			return utility::error_code::action_prevented;
		}

		if ((hk->handle_ = reinterpret_cast<HRGN>(result_info.second)) == nullptr){
			destroy_handle_();
			handle_ = nullptr;
			return utility::error_code::action_could_not_be_completed;
		}
	}

	if (thread_.send_message(*this, WM_NCCREATE) != FALSE){
		RECT dimension{};

		GetRgnBox(handle_, &dimension);
		OffsetRgn(handle_, -dimension.left, -dimension.top);

		if (hk != nullptr){
			GetRgnBox(hk->handle_, &dimension);
			OffsetRgn(hk->handle_, -dimension.left, -dimension.top);
		}
		
		size_ = SIZE{ (dimension.right - dimension.left), (dimension.bottom - dimension.top) };
		thread_.send_message(*this, WM_CREATE);

		if (visible_)
			redraw_();

		return utility::error_code::nil;
	}

	destroy_handle_();
	handle_ = nullptr;

	return utility::error_code::action_prevented;
}

winp::utility::error_code winp::ui::non_window_surface::destroy_(){
	if (handle_ == nullptr || !is_created_())
		return utility::error_code::nil;

	hide_();
	if (auto error_code = destroy_handle_(); error_code != utility::error_code::nil)
		return error_code;

	handle_ = nullptr;
	thread_.send_message(*this, WM_NCDESTROY);

	return utility::error_code::nil;
}

bool winp::ui::non_window_surface::is_created_() const{
	return (handle_ != nullptr);
}

SIZE winp::ui::non_window_surface::get_client_size_() const{
	if (auto hk = find_hook_<ui::non_window_non_client_hook>(); hk != nullptr){
		return SIZE{
			(size_.cx - (hk->padding_.left + hk->padding_.right)),
			(size_.cy - (hk->padding_.top + hk->padding_.bottom))
		};
	}

	return visible_surface::get_client_size_();
}

SIZE winp::ui::non_window_surface::get_current_client_size_() const{
	if (auto hk = find_hook_<ui::non_window_non_client_hook>(); hk != nullptr){
		auto &size = get_current_size_();
		return SIZE{
			(size.cx - (hk->padding_.left + hk->padding_.right)),
			(size.cy - (hk->padding_.top + hk->padding_.bottom))
		};
	}

	return visible_surface::get_current_client_size_();
}

POINT winp::ui::non_window_surface::get_client_offset_() const{
	if (auto hk = find_hook_<ui::non_window_non_client_hook>(); hk != nullptr)
		return POINT{ hk->padding_.left, hk->padding_.top };
	return visible_surface::get_client_offset_();
}

UINT winp::ui::non_window_surface::absolute_hit_test_(int x, int y) const{
	auto hk = find_hook_<ui::non_window_non_client_hook>();
	if (hk == nullptr)
		return visible_surface::absolute_hit_test_(x, y);

	auto client_size = get_current_client_size_();
	auto client_dimension = convert_dimension_to_absolute_value_(RECT{ 0, 0, client_size.cx, client_size.cy });

	POINT position{ x, y };
	if (PtInRect(&client_dimension, position) != FALSE)
		return HTCLIENT;

	RECT dimension{
		(client_dimension.left - hk->padding_.left),
		(client_dimension.top - hk->padding_.top),
		(client_dimension.right + hk->padding_.right),
		client_dimension.top
	};

	if (PtInRect(&dimension, position) != FALSE)
		return HTCAPTION;

	dimension = RECT{
		(client_dimension.left - hk->padding_.left),
		(client_dimension.top + hk->padding_.top),
		client_dimension.left,
		(client_dimension.bottom - hk->padding_.bottom)
	};

	if (PtInRect(&dimension, position) != FALSE)
		return HTLEFT;

	dimension = RECT{
		client_dimension.right,
		(client_dimension.top + hk->padding_.top),
		(client_dimension.right + hk->padding_.right),
		(client_dimension.bottom - hk->padding_.bottom)
	};

	if (PtInRect(&dimension, position) != FALSE)
		return HTRIGHT;

	dimension = RECT{
		(client_dimension.left - hk->padding_.left),
		client_dimension.bottom,
		(client_dimension.right + hk->padding_.right),
		(client_dimension.bottom + hk->padding_.bottom)
	};

	if (PtInRect(&dimension, position) != FALSE)
		return HTBOTTOM;

	return HTNOWHERE;
}

winp::utility::error_code winp::ui::non_window_surface::update_dimension_(const RECT &previous_dimension, int x, int y, int width, int height, UINT flags){
	if ((flags & (SWP_NOMOVE | SWP_NOSIZE)) == (SWP_NOMOVE | SWP_NOSIZE))
		return utility::error_code::nil;

	if ((flags & SWP_NOSIZE) == 0u)
		update_handle_();

	auto visible_parent = dynamic_cast<visible_surface *>(parent_);
	if (!is_visible_() || visible_parent == nullptr || !visible_parent->is_visible())
		return utility::error_code::nil;

	RECT current_dimension{};
	if ((flags & SWP_NOMOVE) != 0u){
		auto &current_position = get_current_position_();
		current_dimension.left = current_position.x;
		current_dimension.top = current_position.y;
	}
	else{//Position changed
		current_dimension.left = x;
		current_dimension.top = y;
	}

	if ((flags & SWP_NOSIZE) != 0u){
		auto &current_size = get_current_size_();
		current_dimension.right = (current_dimension.left + current_size.cx);
		current_dimension.bottom = (current_dimension.top + current_size.cy);
	}
	else{//Size changed
		current_dimension.right = (current_dimension.left + width);
		current_dimension.bottom = (current_dimension.top + height);
	}

	visible_parent->redraw(previous_dimension);
	visible_parent->redraw(current_dimension);

	return visible_surface::update_dimension_(previous_dimension, x, y, width, height, flags);
}

winp::utility::error_code winp::ui::non_window_surface::redraw_() const{
	auto &current_size = get_current_size_();
	return redraw_(RECT{ 0, 0, current_size.cx, current_size.cy });
}

winp::utility::error_code winp::ui::non_window_surface::redraw_(const RECT &region) const{
	if (IsRectEmpty(&region) != FALSE || !is_visible_())
		return utility::error_code::nil;

	if (handle_ == nullptr)
		return utility::error_code::object_not_created;

	auto visible_parent = get_first_ancestor_of_<visible_surface>(nullptr);
	if (visible_parent == nullptr || !dynamic_cast<tree *>(visible_parent)->is_created())
		return utility::error_code::parent_not_created;

	auto current_dimension = get_current_dimension_();
	RECT update_region{ (region.left + current_dimension.left), (region.top + current_dimension.top), (region.right + current_dimension.left), (region.bottom + current_dimension.top) };

	IntersectRect(&update_region, &update_region, &current_dimension);
	return ((IsRectEmpty(&update_region) == FALSE) ? visible_parent->redraw(update_region) : utility::error_code::nil);
}

winp::utility::error_code winp::ui::non_window_surface::set_visibility_(bool is_visible, bool redraw){
	if (redraw)
		return (is_visible ? show_() : hide_());

	visible_ = is_visible;
	return utility::error_code::nil;
}

winp::utility::error_code winp::ui::non_window_surface::show_(){
	if (visible_)
		return utility::error_code::nil;

	visible_ = true;
	return ((handle_ == nullptr) ? utility::error_code::nil : redraw_());
}

winp::utility::error_code winp::ui::non_window_surface::hide_(){
	if (!visible_ || handle_ == nullptr || parent_ == nullptr || !parent_->is_created())
		return utility::error_code::nil;

	visible_ = false;
	if (auto visible_parent = dynamic_cast<visible_surface *>(parent_); visible_parent != nullptr)
		return visible_parent->redraw(get_dimension_());

	return utility::error_code::nil;
}

bool winp::ui::non_window_surface::is_visible_() const{
	return visible_;
}

HRGN winp::ui::non_window_surface::get_handle_() const{
	return handle_;
}

HRGN winp::ui::non_window_surface::get_outer_handle_() const{
	auto hk = find_hook_<ui::non_window_non_client_hook>();
	return ((hk == nullptr) ? handle_ : hk->handle_);
}

HRGN winp::ui::non_window_surface::create_handle_() const{
	auto result_info = trigger_event_<events::create_non_window_handle>(nullptr, get_current_client_size_());
	return (((result_info.first & events::object::state_default_prevented) == 0u) ? reinterpret_cast<HRGN>(result_info.second) : nullptr);
}

winp::utility::error_code winp::ui::non_window_surface::update_handle_(){
	if (!is_created_())
		return utility::error_code::nil;

	auto result_info = trigger_event_<events::create_non_window_handle>(handle_, get_current_client_size_());
	if ((result_info.first & events::object::state_default_prevented) != 0u)
		return utility::error_code::action_prevented;

	auto value = reinterpret_cast<HRGN>(result_info.second);
	if (value == nullptr)
		return utility::error_code::action_could_not_be_completed;

	auto hk = find_hook_<ui::non_window_non_client_hook>();
	if (hk != nullptr){//Create non-client handle
		auto result_info = trigger_event_<events::create_non_window_non_client_handle>(nullptr, get_current_size_());
		if ((result_info.first & events::object::state_default_prevented) != 0u)
			return utility::error_code::action_prevented;

		if (result_info.second == 0)
			return utility::error_code::action_could_not_be_completed;

		hk->handle_ = reinterpret_cast<HRGN>(result_info.second);
	}

	if (value != handle_){//Update
		destroy_handle_();
		handle_ = value;
	}

	RECT dimension{};
	GetRgnBox(handle_, &dimension);
	OffsetRgn(handle_, -dimension.left, -dimension.top);

	if (hk != nullptr){
		GetRgnBox(hk->handle_, &dimension);
		OffsetRgn(hk->handle_, -dimension.left, -dimension.top);
	}

	return utility::error_code::nil;
}

winp::utility::error_code winp::ui::non_window_surface::destroy_handle_(){
	auto result_info = trigger_event_<events::destroy_non_window_handle>(handle_);
	if ((result_info.first & events::object::state_default_prevented) != 0u)
		return utility::error_code::action_prevented;

	if (auto hk = find_hook_<ui::non_window_non_client_hook>(); hk != nullptr){
		trigger_event_<events::destroy_non_window_handle>(hk->handle_);
		hk->handle_ = nullptr;
	}

	return static_cast<utility::error_code>(result_info.second);
}
