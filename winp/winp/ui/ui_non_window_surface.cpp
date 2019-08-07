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
		utility::helper::move_rgn(handle_, 0, 0);
		if (hk != nullptr){
			utility::helper::move_rgn(hk->handle_, 0, 0);
			size_ = utility::helper::get_rgn_size(hk->handle_);
		}
		else//No non-client area
			size_ = utility::helper::get_rgn_size(handle_);
		
		thread_.send_message(*this, WM_CREATE);
		if (visible_)
			redraw_(true);

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

RECT winp::ui::non_window_surface::get_client_padding_() const{
	auto hk = find_hook_<ui::non_window_non_client_hook>();
	return ((hk == nullptr) ? RECT{} : hk->padding_);
}

UINT winp::ui::non_window_surface::hit_test_(int x, int y) const{
	if (handle_ == nullptr)
		return visible_surface::hit_test_(x, y);

	utility::helper::move_rgn(handle_, 0, 0);
	return ((PtInRegion(handle_, x, y) == FALSE) ? HTNOWHERE : HTCLIENT);
}

UINT winp::ui::non_window_surface::absolute_hit_test_(int x, int y) const{
	if (handle_ == nullptr)
		return visible_surface::absolute_hit_test_(x, y);

	auto hk = find_hook_<ui::non_window_non_client_hook>();
	if (hk == nullptr)
		return visible_surface::absolute_hit_test_(x, y);

	auto absolute_position = get_absolute_position_();
	auto start_offset = get_client_start_offset_();

	if (hk->handle_ != nullptr){
		utility::helper::move_rgn(hk->handle_, absolute_position);
		if (PtInRegion(hk->handle_, x, y) == FALSE)
			return HTNOWHERE;
	}

	utility::helper::move_rgn(handle_, (absolute_position.x + hk->padding_.left + start_offset.x), (absolute_position.y + hk->padding_.top + start_offset.y));
	if (PtInRegion(handle_, x, y) != FALSE)
		return HTCLIENT;

	auto &size = get_current_size_();
	RECT absolute_dimension{ absolute_position.x, absolute_position.y, (absolute_position.x + size.cx), (absolute_position.y + size.cy) };
	POINT position{ x, y };

	RECT dimension{
		absolute_dimension.left,
		absolute_dimension.top,
		(absolute_dimension.left + hk->padding_.left),
		(absolute_dimension.top + hk->padding_.bottom)
	};

	if (PtInRect(&dimension, position) != FALSE)
		return HTTOPLEFT;

	dimension = RECT{
		(absolute_dimension.left + hk->padding_.left),
		absolute_dimension.top,
		(absolute_dimension.right - hk->padding_.right),
		(absolute_dimension.top + hk->padding_.bottom)
	};

	if (PtInRect(&dimension, position) != FALSE)
		return HTTOP;

	dimension = RECT{
		(absolute_dimension.right - hk->padding_.right),
		absolute_dimension.top,
		absolute_dimension.right,
		(absolute_dimension.top + hk->padding_.bottom)
	};

	if (PtInRect(&dimension, position) != FALSE)
		return HTTOPRIGHT;

	dimension = RECT{
		(absolute_dimension.right - hk->padding_.right),
		(absolute_dimension.top + hk->padding_.bottom),
		absolute_dimension.right,
		(absolute_dimension.bottom - hk->padding_.bottom)
	};

	if (PtInRect(&dimension, position) != FALSE)
		return HTRIGHT;

	dimension = RECT{
		(absolute_dimension.right - hk->padding_.right),
		(absolute_dimension.bottom - hk->padding_.bottom),
		absolute_dimension.right,
		absolute_dimension.bottom
	};

	if (PtInRect(&dimension, position) != FALSE)
		return HTBOTTOMRIGHT;

	dimension = RECT{
		(absolute_dimension.left + hk->padding_.left),
		(absolute_dimension.bottom - hk->padding_.bottom),
		(absolute_dimension.right - hk->padding_.right),
		absolute_dimension.bottom
	};

	if (PtInRect(&dimension, position) != FALSE)
		return HTBOTTOM;

	dimension = RECT{
		absolute_dimension.left,
		(absolute_dimension.bottom - hk->padding_.bottom),
		(absolute_dimension.left + hk->padding_.left),
		absolute_dimension.bottom
	};

	if (PtInRect(&dimension, position) != FALSE)
		return HTBOTTOMLEFT;

	dimension = RECT{
		absolute_dimension.left,
		(absolute_dimension.top + hk->padding_.bottom),
		(absolute_dimension.left + hk->padding_.left),
		(absolute_dimension.bottom - hk->padding_.bottom),
	};

	if (PtInRect(&dimension, position) != FALSE)
		return HTLEFT;

	dimension = RECT{
		(absolute_dimension.left + hk->padding_.left),
		(absolute_dimension.top + hk->padding_.bottom),
		(absolute_dimension.right - hk->padding_.right),
		(absolute_dimension.top + hk->padding_.top)
	};

	if (PtInRect(&dimension, position) != FALSE)
		return HTCAPTION;

	return HTNOWHERE;
}

winp::utility::error_code winp::ui::non_window_surface::update_dimension_(const RECT &previous_dimension, int x, int y, int width, int height, UINT flags){
	if ((flags & (SWP_NOMOVE | SWP_NOSIZE)) == (SWP_NOMOVE | SWP_NOSIZE))
		return visible_surface::update_dimension_(previous_dimension, x, y, width, height, flags);

	auto offset_info = get_first_ancestor_and_relative_offset_<window_surface>();
	if (offset_info.first == nullptr)
		return visible_surface::update_dimension_(previous_dimension, x, y, width, height, flags);

	auto ancestor_handle = offset_info.first->get_handle();
	if (ancestor_handle == nullptr)
		return visible_surface::update_dimension_(previous_dimension, x, y, width, height, flags);

	auto handle = get_outer_handle_();
	if (handle == nullptr)
		return visible_surface::update_dimension_(previous_dimension, x, y, width, height, flags);

	auto is_visible = (is_visible_() && offset_info.first->is_visible());
	if (is_visible){
		utility::helper::move_rgn(handle, (previous_dimension.left + offset_info.second.x), (previous_dimension.top + offset_info.second.y));//Move to 'previous offset'
		InvalidateRgn(ancestor_handle, handle, TRUE);
	}

	if ((flags & SWP_NOSIZE) == 0u)
		update_handle_();

	if (is_visible && (handle = get_outer_handle_()) != nullptr){
		utility::helper::move_rgn(handle, (x + offset_info.second.x), (y + offset_info.second.y));//Move to 'new offset'
		InvalidateRgn(ancestor_handle, handle, TRUE);
	}

	return visible_surface::update_dimension_(previous_dimension, x, y, width, height, flags);
}

winp::utility::error_code winp::ui::non_window_surface::redraw_(bool non_client) const{
	if (!non_client)//Client area
		return redraw_(handle_);

	return (is_visible_() ? redraw_non_client_() : utility::error_code::nil);
}

winp::utility::error_code winp::ui::non_window_surface::redraw_(const RECT &region) const{
	if (handle_ == nullptr || IsRectEmpty(&region) != FALSE || !is_visible_())
		return utility::error_code::nil;

	auto handle = CreateRectRgn(region.left, region.top, region.right, region.bottom);
	if (handle == nullptr)
		return utility::error_code::action_could_not_be_completed;

	auto result = redraw_(handle);
	DeleteObject(handle);

	return result;
}

winp::utility::error_code winp::ui::non_window_surface::redraw_(HRGN rgn) const{
	return ((handle_ == nullptr || !is_visible_()) ? utility::error_code::nil : redraw_client_(rgn));
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
	return ((handle_ == nullptr) ? utility::error_code::nil : redraw_(true));
}

winp::utility::error_code winp::ui::non_window_surface::hide_(){
	if (!visible_ || handle_ == nullptr || parent_ == nullptr || !parent_->is_created())
		return utility::error_code::nil;

	visible_ = false;
	redraw_non_client_();

	return utility::error_code::nil;
}

bool winp::ui::non_window_surface::is_visible_() const{
	return visible_;
}

winp::utility::error_code winp::ui::non_window_surface::redraw_non_client_() const{
	auto handle = get_outer_handle_();
	if (handle == nullptr || handle == handle_)
		return redraw_client_(handle_);

	auto offset_info = get_first_ancestor_and_relative_offset_<window_surface>();
	if (offset_info.first == nullptr)
		return utility::error_code::nil;

	auto ancestor_handle = offset_info.first->get_handle();
	if (ancestor_handle == nullptr)
		return utility::error_code::nil;

	auto &position = get_current_position_();
	utility::helper::move_rgn(handle, (position.x + offset_info.second.x), (position.y + offset_info.second.y));//Move to 'offset'

	return ((InvalidateRgn(ancestor_handle, handle, TRUE) == FALSE) ? utility::error_code::action_could_not_be_completed : utility::error_code::nil);
}

winp::utility::error_code winp::ui::non_window_surface::redraw_client_(HRGN rgn) const{
	if (rgn == nullptr)
		return utility::error_code::nil;

	auto offset_info = get_first_ancestor_and_relative_offset_<window_surface>();
	if (offset_info.first == nullptr)
		return utility::error_code::nil;

	auto ancestor_handle = offset_info.first->get_handle();
	if (ancestor_handle == nullptr)
		return utility::error_code::nil;

	HRGN handle;
	if (rgn != handle_){
		utility::helper::offset_rgn(rgn, get_client_start_offset_());
		utility::helper::move_rgn(handle_, 0, 0);
		handle = utility::helper::intersect_rgn(handle_, rgn);
	}
	else//Client handle
		handle = rgn;

	if (handle == nullptr)
		return utility::error_code::action_could_not_be_completed;

	auto &position = get_current_position_();
	utility::helper::move_rgn(handle, (position.x + offset_info.second.x), (position.y + offset_info.second.y));//Move to 'offset'

	auto result = ((InvalidateRgn(ancestor_handle, handle, TRUE) == FALSE) ? utility::error_code::action_could_not_be_completed : utility::error_code::nil);
	if (handle != handle_)//Free handle
		DeleteObject(handle);

	return result;
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

	utility::helper::move_rgn(handle_, 0, 0);
	if (hk != nullptr)
		utility::helper::move_rgn(hk->handle_, 0, 0);

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

HRGN winp::ui::non_window_surface::prepare_handle_() const{
	auto &position = get_current_position_();
	auto client_offset = get_client_offset_();
	return prepare_handle_(handle_, POINT{ (position.x + client_offset.x), (position.y + client_offset.y) });
}

HRGN winp::ui::non_window_surface::prepare_handle_(HRGN handle, const POINT &offset) const{
	if (handle == nullptr || (handle = utility::helper::copy_rgn(handle)) == nullptr)
		return nullptr;

	utility::helper::move_rgn(handle, offset);
	exclude_siblings_(handle);

	exclude_children_(handle);
	intersect_ancestors_(handle);

	return handle;
}

bool winp::ui::non_window_surface::intersect_ancestors_(HRGN handle) const{
	auto offset = utility::helper::get_rgn_offset(handle);
	traverse_ancestors_of_<visible_surface>([&](visible_surface &ancestor){
		if (!ancestor.is_visible_()){
			if (dynamic_cast<window_surface *>(&ancestor) != nullptr)
				return false;

			utility::helper::offset_rgn(handle, ancestor.get_current_position_());
			return true;
		}

		auto client_start_offset = ancestor.get_client_start_offset_();
		utility::helper::offset_rgn(handle, client_start_offset);

		if (dynamic_cast<window_surface *>(&ancestor) != nullptr){
			auto client_size = ancestor.get_current_client_size_();
			utility::helper::intersect_rgn_alt(handle, RECT{ offset.x, offset.y, client_size.cx, client_size.cy });
		}
		else if (auto non_window_ancestor = dynamic_cast<non_window_surface *>(&ancestor); non_window_ancestor->handle_ != nullptr){//Non-window ancestor
			utility::helper::move_rgn(non_window_ancestor->handle_, 0, 0);
			utility::helper::intersect_rgn_alt(handle, non_window_ancestor->handle_);
		}

		if (dynamic_cast<window_surface *>(&ancestor) != nullptr)
			return false;

		utility::helper::offset_rgn(handle, ancestor.get_current_position_());
		return true;
	});

	return true;
	//return utility::helper::move_rgn(handle, offset);
}

bool winp::ui::non_window_surface::exclude_siblings_(HRGN handle) const{
	traverse_siblings_of_<visible_surface>([&](visible_surface &sibling, bool is_before){
		if (is_before || !sibling.is_visible_())
			return true;

		if (auto non_window_sibling = dynamic_cast<non_window_surface *>(&sibling); non_window_sibling != nullptr){
			auto outer_handle = non_window_sibling->get_outer_handle_();
			if (outer_handle == nullptr)//Not created
				return true;

			auto d0 = utility::helper::get_rgn_dimension(outer_handle);
			auto d1 = utility::helper::get_rgn_dimension(handle_);
			auto d2 = utility::helper::get_rgn_dimension(handle);

			utility::helper::move_rgn(outer_handle, non_window_sibling->get_current_position_());
			d0 = utility::helper::get_rgn_dimension(outer_handle);
			utility::helper::exclude_rgn_alt(handle, outer_handle);

			d2 = utility::helper::get_rgn_dimension(handle);
			d2 = utility::helper::get_rgn_dimension(handle);
		}
		else//No handle
			utility::helper::exclude_rgn_alt(handle, sibling.get_current_dimension_());

		return true;
	});

	return true;
}

bool winp::ui::non_window_surface::exclude_children_(HRGN handle) const{
	auto offset = get_client_start_offset_();
	traverse_children_of_<visible_surface>([&](visible_surface &child){
		if (!child.is_visible_() || child.get_current_background_color_().a == 0.0f)
			return true;

		if (auto non_window_child = dynamic_cast<non_window_surface *>(&child); non_window_child != nullptr){
			auto outer_handle = non_window_child->get_outer_handle_();
			if (outer_handle == nullptr)//Not created
				return true;

			utility::helper::move_rgn(outer_handle, non_window_child->get_current_position_());
			utility::helper::offset_rgn(outer_handle, offset);
			utility::helper::exclude_rgn_alt(handle, outer_handle);
		}
		else{//No handle
			auto dimension = child.get_current_dimension_();
			OffsetRect(&dimension, offset.x, offset.y);
			utility::helper::exclude_rgn_alt(handle, dimension);
		}

		return true;
	});

	return true;
}

bool winp::ui::non_window_surface::exclude_from_clip_(HDC device, const POINT &offset) const{
	if (!is_visible_() || get_current_background_color_().a == 0.0f)
		return true;

	auto outer_handle = get_outer_handle_();
	if (outer_handle == nullptr)//Not created
		return true;

	auto &position = get_current_position_();
	utility::helper::move_rgn(outer_handle, (position.x + offset.x), (position.y + offset.y));
	ExtSelectClipRgn(device, outer_handle, RGN_DIFF);

	return true;
}
