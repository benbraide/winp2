#include "../app/app_object.h"

#include "rectangular_non_window.h"

winp::non_window::rectangle::rectangle(){
	set_event_state_<events::create_non_window_handle, events::update_non_window_handle, events::destroy_non_window_handle>((event_manager_type::state_disable_bounding | event_manager_type::state_disable_triggering));
	add_event_handler_([this](events::create_non_window_handle &e) -> HRGN{
		auto &current_size = get_current_size_();
		return CreateRectRgn(0, 0, current_size.cx, current_size.cy);
	});

	add_event_handler_([this](events::update_non_window_handle &e) -> HRGN{
		auto &current_size = get_current_size_();
		return ((SetRectRgn(e.get_handle(), 0, 0, current_size.cx, current_size.cy) == FALSE) ? nullptr : e.get_handle());
	});
}

winp::non_window::rectangle::rectangle(tree &parent)
	: rectangle(parent, static_cast<std::size_t>(-1)){}

winp::non_window::rectangle::rectangle(tree &parent, std::size_t index)
	: rectangle(){
	set_parent(&parent, index);
}

winp::non_window::rectangle::~rectangle() = default;
