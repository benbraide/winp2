#include "../app/app_collection.h"

#include "round_rectangular_non_window.h"

winp::non_window::round_rectangle::round_rectangle()
	: round_rectangle(app::collection::get_main()->get_thread()){}

winp::non_window::round_rectangle::round_rectangle(thread::object &thread)
	: round_rectangle(thread, true){}

winp::non_window::round_rectangle::round_rectangle(thread::object &thread, bool init_grid)
	: non_window_surface(thread, init_grid){}

winp::non_window::round_rectangle::round_rectangle(tree &parent)
	: round_rectangle(parent, static_cast<std::size_t>(-1)){}

winp::non_window::round_rectangle::round_rectangle(tree &parent, std::size_t index)
	: round_rectangle(parent.get_thread()){
	set_parent(&parent, index);
}

winp::non_window::round_rectangle::~round_rectangle() = default;

winp::utility::error_code winp::non_window::round_rectangle::set_border_curve_size(const SIZE &value, const std::function<void(round_rectangle &, utility::error_code)> &callback){
	return set_border_curve_size(value.cx, value.cy, callback);
}

winp::utility::error_code winp::non_window::round_rectangle::set_border_curve_size(int width, int height, const std::function<void(round_rectangle &, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, set_border_curve_size_(width, height));
	}, (callback != nullptr), utility::error_code::nil);
}

const SIZE &winp::non_window::round_rectangle::get_border_curve_size(const std::function<void(const SIZE &)> &callback) const{
	return *synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return &synchronized_item_pass_return_ref_value_to_callback(callback, &border_curve_size_);
	}, (callback != nullptr), &border_curve_size_);
}

HRGN winp::non_window::round_rectangle::create_handle_() const{
	return CreateRoundRectRgn(0, 0, size_.cx, size_.cy, border_curve_size_.cx, border_curve_size_.cy);
}

winp::utility::error_code winp::non_window::round_rectangle::update_handle_(){
	if (handle_ == nullptr)
		return utility::error_code::nil;

	DeleteObject(handle_);
	handle_ = create_handle_();

	return utility::error_code::nil;
}

winp::utility::error_code winp::non_window::round_rectangle::set_border_curve_size_(int width, int height){
	if (width == border_curve_size_.cx && height == border_curve_size_.cy)
		return utility::error_code::nil;//No changes

	border_curve_size_ = SIZE{ width, height };
	update_handle_();
	redraw_();

	return utility::error_code::nil;
}
