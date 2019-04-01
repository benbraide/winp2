#include "../app/app_collection.h"

#include "label_control.h"

winp::control::label::label()
	: label(app::collection::get_main()->get_thread()){}

winp::control::label::label(thread::object &thread)
	: button(thread, WC_STATICW, ICC_STANDARD_CLASSES){
	add_event_handler_([this](events::hit_test &e){
		if (io_event_count_ != 0u){
			e.prevent_default();
			e.set_result_if_not_set(surface::absolute_hit_test_(e.get_position().x, e.get_position().y));
		}
	});

	add_event_change_handler_<
		events::mouse_leave,
		events::mouse_move,
		events::mouse_down,
		events::mouse_up,
		events::mouse_dbl_clk,
		events::mouse_wheel,
		events::key_down,
		events::key_up,
		events::key_press
	>([this](std::size_t current, std::size_t previous){
		if (current < previous)
			--io_event_count_;
		else
			++io_event_count_;
	});

	padding_.cx = 0;
	padding_.cy = 0;
}

winp::control::label::label(tree &parent)
	: label(parent, static_cast<std::size_t>(-1)){}

winp::control::label::label(tree &parent, std::size_t index)
	: label(parent.get_thread()){
	set_parent(&parent, index);
}

winp::control::label::~label() = default;

DWORD winp::control::label::get_persistent_styles_(bool is_extended) const{
	return (button::get_persistent_styles_(is_extended) | (is_extended ? 0u : SS_CENTER | SS_CENTERIMAGE));
}
