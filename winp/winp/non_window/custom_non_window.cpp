#include "../app/app_object.h"

#include "custom_non_window.h"

HRGN winp::events::update_non_window_handle::get_handle() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return handle_;
}

HRGN winp::events::destroy_non_window_handle::get_handle() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return handle_;
}

winp::non_window::custom::custom()
	: custom(app::object::get_thread()){}

winp::non_window::custom::custom(thread::object &thread)
	: custom(thread, true){}

winp::non_window::custom::custom(thread::object &thread, bool init_grid)
	: non_window_surface(thread, init_grid){}

winp::non_window::custom::custom(tree &parent)
	: custom(parent, static_cast<std::size_t>(-1)){}

winp::non_window::custom::custom(tree &parent, std::size_t index)
	: custom(parent.get_thread()){
	set_parent(&parent, index);
}

winp::non_window::custom::~custom(){
	destruct();
}

winp::utility::error_code winp::non_window::custom::create_(){
	if (auto error_code = non_window_surface::create_(); error_code != utility::error_code::nil)
		return error_code;

	if (handle_ != nullptr){
		RECT dimension{};

		GetRgnBox(handle_, &dimension);
		OffsetRgn(handle_, -dimension.left, -dimension.top);

		size_ = SIZE{ (dimension.right - dimension.left), (dimension.bottom - dimension.top) };
	}

	return utility::error_code::nil;
}

HRGN winp::non_window::custom::create_handle_() const{
	auto result = trigger_event_<events::create_non_window_handle>();
	return (((result.first & events::object::state_default_prevented) == 0u) ? reinterpret_cast<HRGN>(result.second) : nullptr);
}

winp::utility::error_code winp::non_window::custom::update_handle_(){
	auto result = trigger_event_<events::update_non_window_handle>(handle_);
	if ((result.first & events::object::state_default_prevented) != 0u)
		return utility::error_code::action_prevented;

	if (auto value = reinterpret_cast<HRGN>(result.second); value != nullptr){
		RECT dimension{};
		if (value != handle_ && events_manager_.get_bound_count<events::destroy_non_window_handle>() == 0u)
			DeleteObject(handle_);
		else if (value != handle_)
			trigger_event_<events::destroy_non_window_handle>(handle_);

		GetRgnBox((handle_ = value), &dimension);
		OffsetRgn(handle_, -dimension.left, -dimension.top);

		size_ = SIZE{ (dimension.right - dimension.left), (dimension.bottom - dimension.top) };
	}

	return utility::error_code::nil;
}

winp::utility::error_code winp::non_window::custom::destroy_handle_(){
	if (events_manager_.get_bound_count<events::destroy_non_window_handle>() == 0u)
		return non_window_surface::destroy_handle_();

	auto result = trigger_event_<events::destroy_non_window_handle>(handle_);
	if ((result.first & events::object::state_default_prevented) != 0u)
		return utility::error_code::action_prevented;

	return utility::error_code::nil;
}
