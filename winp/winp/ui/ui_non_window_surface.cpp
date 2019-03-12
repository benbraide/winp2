#include "../app/app_collection.h"

#include "ui_non_window_surface.h"

winp::ui::non_window_surface::non_window_surface()
	: non_window_surface(app::collection::get_main()->get_thread()){}

winp::ui::non_window_surface::non_window_surface(thread::object &thread)
	: tree(thread){
	background_color_ = convert_colorref_to_colorf(GetSysColor(COLOR_WINDOW), 255);
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
	if (handle_ != nullptr)
		return utility::error_code::nil;

	if (parent_ == nullptr || !parent_->is_created())
		return utility::error_code::parent_not_created;

	if ((handle_ = create_handle_()) == nullptr)
		return utility::error_code::action_could_not_be_completed;

	if (thread_.send_message(*this, WM_NCCREATE) != FALSE){
		if (visible_)
			redraw_();
		return utility::error_code::nil;
	}

	DeleteObject(handle_);
	handle_ = nullptr;

	return utility::error_code::action_prevented;
}

winp::utility::error_code winp::ui::non_window_surface::destroy_(){
	if (handle_ == nullptr)
		return utility::error_code::nil;

	hide_();
	if (DeleteObject(handle_) == FALSE)
		return utility::error_code::action_could_not_be_completed;

	handle_ = nullptr;
	thread_.send_message(*this, WM_NCDESTROY);

	return utility::error_code::nil;
}

bool winp::ui::non_window_surface::is_created_() const{
	return (handle_ != nullptr);
}

winp::utility::error_code winp::ui::non_window_surface::redraw_() const{
	auto dimension = get_dimension_();
	OffsetRect(&dimension, -dimension.left, -dimension.top);
	return redraw_(dimension);
}

winp::utility::error_code winp::ui::non_window_surface::redraw_(const RECT &region) const{
	if (handle_ == nullptr)
		return utility::error_code::object_not_created;

	if (parent_ == nullptr || !parent_->is_created())
		return utility::error_code::parent_not_created;

	if (auto visible_parent = dynamic_cast<visible_surface *>(parent_); visible_parent != nullptr)
		return visible_parent->redraw(RECT{ (region.left + position_.x), (region.top + position_.y), (region.right + position_.x), (region.bottom + position_.y) });

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
