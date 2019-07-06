#include "../app/app_object.h"

#include "elliptical_non_window.h"

winp::non_window::ellipsis::ellipsis(){
	set_event_state_<events::create_non_window_handle, events::update_non_window_handle, events::destroy_non_window_handle>((event_manager_type::state_disable_bounding | event_manager_type::state_disable_triggering));
	add_event_handler_([this](events::create_non_window_handle &e) -> HRGN{
		auto &current_size = get_current_size_();
		return CreateEllipticRgn(0, 0, current_size.cx, current_size.cy);
	});

	add_event_handler_([this](events::update_non_window_handle &e) -> HRGN{
		auto &current_size = get_current_size_();
		return CreateEllipticRgn(0, 0, current_size.cx, current_size.cy);
	});
}

winp::non_window::ellipsis::ellipsis(tree &parent)
	: ellipsis(parent, static_cast<std::size_t>(-1)){}

winp::non_window::ellipsis::ellipsis(tree &parent, std::size_t index)
	: ellipsis(){
	set_parent(&parent, index);
}

winp::non_window::ellipsis::~ellipsis() = default;

winp::non_window::circle::circle() = default;

winp::non_window::circle::circle(tree &parent)
	: circle(parent, static_cast<std::size_t>(-1)){}

winp::non_window::circle::circle(tree &parent, std::size_t index){
	set_parent(&parent, index);
}

winp::non_window::circle::~circle() = default;

winp::utility::error_code winp::non_window::circle::set_radius(int value, const std::function<void(circle &, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, set_size_((value * 2), (value * 2), true));
	}, (callback != nullptr), utility::error_code::nil);
}

int winp::non_window::circle::get_radius(const std::function<void(int)> &callback) const{
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return synchronized_item_pass_return_value_to_callback(callback, (get_size_().cx / 2));
	}, (callback != nullptr), 0);
}

winp::utility::error_code winp::non_window::circle::set_dimension_(int x, int y, int width, int height, UINT flags, bool allow_animation){
	return ellipsis::set_dimension_(x, y, (width + (width % 2)), (width + (width % 2)), flags, allow_animation);
}
