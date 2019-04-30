#include "../app/app_collection.h"

#include "rectangular_non_window.h"

winp::non_window::rectangle::rectangle()
	: rectangle(app::collection::get_main()->get_thread()){}

winp::non_window::rectangle::rectangle(thread::object &thread)
	: rectangle(thread, true){}

winp::non_window::rectangle::rectangle(thread::object &thread, bool init_grid)
	: non_window_surface(thread, init_grid){}

winp::non_window::rectangle::rectangle(tree &parent)
	: rectangle(parent, static_cast<std::size_t>(-1)){}

winp::non_window::rectangle::rectangle(tree &parent, std::size_t index)
	: rectangle(parent.get_thread()){
	set_parent(&parent, index);
}

winp::non_window::rectangle::~rectangle() = default;

HRGN winp::non_window::rectangle::create_handle_() const{
	return CreateRectRgn(0, 0, size_.cx, size_.cy);
}

winp::utility::error_code winp::non_window::rectangle::update_handle_(){
	if (handle_ == nullptr)
		return utility::error_code::nil;

	if (SetRectRgn(handle_, 0, 0, size_.cx, size_.cy) == FALSE)
		return utility::error_code::action_could_not_be_completed;

	redraw_();
	return utility::error_code::nil;
}
