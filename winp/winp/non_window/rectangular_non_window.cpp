#include "../app/app_object.h"

#include "rectangular_non_window.h"

winp::non_window::rectangle::rectangle()
	: rectangle(app::object::get_thread()){}

winp::non_window::rectangle::rectangle(thread::object &thread)
	: rectangle(thread, true){}

winp::non_window::rectangle::rectangle(thread::object &thread, bool init_grid)
	: non_window_surface(thread, init_grid){
	set_event_state_<events::create_non_window_handle, events::update_non_window_handle, events::destroy_non_window_handle>((event_manager_type::state_disable_bounding | event_manager_type::state_disable_triggering));
	add_event_handler_([this](events::create_non_window_handle &e) -> HRGN{
		return CreateRectRgn(0, 0, size_.cx, size_.cy);
	});

	add_event_handler_([this](events::update_non_window_handle &e) -> HRGN{
		return ((SetRectRgn(e.get_handle(), 0, 0, size_.cx, size_.cy) == FALSE) ? nullptr : e.get_handle());
	});
}

winp::non_window::rectangle::rectangle(tree &parent)
	: rectangle(parent, static_cast<std::size_t>(-1)){}

winp::non_window::rectangle::rectangle(tree &parent, std::size_t index)
	: rectangle(parent.get_thread()){
	set_parent(&parent, index);
}

winp::non_window::rectangle::~rectangle() = default;
