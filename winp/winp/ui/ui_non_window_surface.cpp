#include "../app/app_object.h"

#include "ui_window_surface.h"

winp::ui::non_window_surface::non_window_surface()
	: non_window_surface(app::object::get_thread()){}

winp::ui::non_window_surface::non_window_surface(thread::object &thread)
	: tree(thread){
	current_background_color_ = background_color_ = convert_colorref_to_colorf(GetSysColor(COLOR_WINDOW), 255);
}

winp::ui::non_window_surface::non_window_surface(tree &parent)
	: non_window_surface(parent, static_cast<std::size_t>(-1)){}

winp::ui::non_window_surface::non_window_surface(tree &parent, std::size_t index)
	: non_window_surface(parent.get_thread()){
	set_parent(&parent, index);
}

winp::ui::non_window_surface::~non_window_surface(){
	destruct();
}

HRGN winp::ui::non_window_surface::get_handle(const std::function<void(HRGN)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, get_handle_());
	}, (callback != nullptr), nullptr);
}

winp::utility::error_code winp::ui::non_window_surface::create_(){
	if (is_created_())
		return utility::error_code::nil;

	if (is_destructed_)
		return utility::error_code::object_destructed;

	if (parent_ != nullptr)
		parent_->auto_create();

	if ((handle_ = create_handle_()) == nullptr)
		return utility::error_code::action_could_not_be_completed;

	if (thread_.send_message(*this, WM_NCCREATE) != FALSE){
		RECT dimension{};

		GetRgnBox(handle_, &dimension);
		OffsetRgn(handle_, -dimension.left, -dimension.top);

		size_ = SIZE{ (dimension.right - dimension.left), (dimension.bottom - dimension.top) };
		thread_.send_message(*this, WM_CREATE);

		if (visible_)
			redraw_();

		return utility::error_code::nil;
	}

	DeleteObject(handle_);
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

winp::utility::error_code winp::ui::non_window_surface::dimension_change_(int x, int y, int width, int height, UINT flags){
	if ((flags & (SWP_NOMOVE | SWP_NOSIZE)) == (SWP_NOMOVE | SWP_NOSIZE))
		return utility::error_code::nil;

	auto previous_dimension = current_dimension_;
	if ((flags & SWP_NOMOVE) == 0u){
		current_dimension_.right += (x - current_dimension_.left);
		current_dimension_.bottom += (y - current_dimension_.top);
		current_dimension_.left = x;
		current_dimension_.top = y;
	}

	if ((flags & SWP_NOSIZE) == 0u){
		current_dimension_.right = (current_dimension_.left + width);
		current_dimension_.bottom = (current_dimension_.top + height);
		update_handle_();
	}

	auto visible_parent = dynamic_cast<visible_surface *>(parent_);
	if (!is_visible_() || visible_parent == nullptr || !visible_parent->is_visible())
		return utility::error_code::nil;

	if (IsRectEmpty(&current_dimension_) == FALSE)//Flush previous dimension
		visible_parent->redraw(previous_dimension);

	if ((flags & (SWP_NOMOVE | SWP_NOSIZE)) != (SWP_NOMOVE | SWP_NOSIZE))//Update view
		visible_parent->redraw(current_dimension_);

	return visible_surface::dimension_change_(x, y, width, height, flags);
}

winp::utility::error_code winp::ui::non_window_surface::redraw_() const{
	return redraw_(RECT{ 0, 0, (current_dimension_.right - current_dimension_.left), (current_dimension_.bottom - current_dimension_.top) });
}

winp::utility::error_code winp::ui::non_window_surface::redraw_(const RECT &region) const{
	if (IsRectEmpty(&region) != FALSE)
		return utility::error_code::nil;

	if (handle_ == nullptr)
		return utility::error_code::object_not_created;

	auto visible_parent = get_first_ancestor_of_<visible_surface>(nullptr);
	if (visible_parent == nullptr || !dynamic_cast<tree *>(visible_parent)->is_created())
		return utility::error_code::parent_not_created;

	if (!is_visible_() || !visible_parent->is_visible())
		return utility::error_code::nil;

	RECT update_region{ (region.left + position_.x), (region.top + position_.y), (region.right + position_.x), (region.bottom + position_.y) };
	IntersectRect(&update_region, &update_region, &current_dimension_);

	return ((IsRectEmpty(&update_region) == FALSE) ? visible_parent->redraw() : utility::error_code::nil);
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

HRGN winp::ui::non_window_surface::create_handle_() const{
	auto result = trigger_event_<events::create_non_window_handle>();
	return (((result.first & events::object::state_default_prevented) == 0u) ? reinterpret_cast<HRGN>(result.second) : nullptr);
}

winp::utility::error_code winp::ui::non_window_surface::update_handle_(){
	auto result = trigger_event_<events::update_non_window_handle>(handle_);
	if ((result.first & events::object::state_default_prevented) != 0u)
		return utility::error_code::action_prevented;

	if (auto value = reinterpret_cast<HRGN>(result.second); value != nullptr){
		destroy_handle_();
		handle_ = value;
	}

	return utility::error_code::nil;
}

winp::utility::error_code winp::ui::non_window_surface::destroy_handle_(){
	if (events_manager_.get_bound_count<events::destroy_non_window_handle>() == 0u)
		return ((DeleteObject(handle_) == FALSE) ? utility::error_code::action_could_not_be_completed : utility::error_code::nil);

	auto result = trigger_event_<events::destroy_non_window_handle>(handle_);
	if ((result.first & events::object::state_default_prevented) != 0u)
		return utility::error_code::action_prevented;

	return utility::error_code::nil;
}
