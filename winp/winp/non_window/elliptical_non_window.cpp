#include "../app/app_collection.h"

#include "elliptical_non_window.h"

winp::non_window::ellipsis::ellipsis()
	: ellipsis(app::collection::get_main()->get_thread()){}

winp::non_window::ellipsis::ellipsis(thread::object &thread)
	: ellipsis(thread, true){}

winp::non_window::ellipsis::ellipsis(thread::object &thread, bool init_grid)
	: non_window_surface(thread, init_grid){}

winp::non_window::ellipsis::ellipsis(tree &parent)
	: ellipsis(parent, static_cast<std::size_t>(-1)){}

winp::non_window::ellipsis::ellipsis(tree &parent, std::size_t index)
	: ellipsis(parent.get_thread()){
	set_parent(&parent, index);
}

winp::non_window::ellipsis::~ellipsis() = default;

HRGN winp::non_window::ellipsis::create_handle_() const{
	return CreateEllipticRgn(0, 0, size_.cx, size_.cy);
}

winp::utility::error_code winp::non_window::ellipsis::update_handle_(){
	if (handle_ == nullptr)
		return utility::error_code::nil;

	DeleteObject(handle_);
	handle_ = create_handle_();

	return utility::error_code::nil;
}

winp::non_window::circle::circle()
	: circle(app::collection::get_main()->get_thread()){}

winp::non_window::circle::circle(thread::object &thread)
	: circle(thread, true){}

winp::non_window::circle::circle(thread::object &thread, bool init_grid)
	: ellipsis(thread, init_grid){}

winp::non_window::circle::circle(tree &parent)
	: circle(parent, static_cast<std::size_t>(-1)){}

winp::non_window::circle::circle(tree &parent, std::size_t index)
	: circle(parent.get_thread()){
	set_parent(&parent, index);
}

winp::non_window::circle::~circle() = default;

winp::utility::error_code winp::non_window::circle::set_radius(int value, const std::function<void(circle &, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, set_size_((value * 2), (value * 2)));
	}, (callback != nullptr), utility::error_code::nil);
}

int winp::non_window::circle::get_radius(const std::function<void(int)> &callback) const{
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return synchronized_item_pass_return_value_to_callback(callback, (get_size_().cx / 2));
	}, (callback != nullptr), 0);
}

winp::utility::error_code winp::non_window::circle::set_dimension_(int x, int y, int width, int height){
	return ellipsis::set_dimension_(x, y, (width + (width % 2)), (width + (width % 2)));
}
