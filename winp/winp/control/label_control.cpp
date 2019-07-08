#include "../app/app_object.h"

#include "label_control.h"

winp::control::label::label()
	: button(WC_STATICW, ICC_STANDARD_CLASSES){
	add_event_handler_([this](events::hit_test &e){
		e.prevent_default();
		e.set_result_if_not_set(surface::absolute_hit_test_(e.get_position().x, e.get_position().y));
	});

	padding_.cx = 0;
	padding_.cy = 0;
}

winp::control::label::label(tree &parent)
	: label(parent, static_cast<std::size_t>(-1)){}

winp::control::label::label(tree &parent, std::size_t index)
	: label(){
	set_parent(&parent, index);
}

winp::control::label::~label() = default;

DWORD winp::control::label::get_persistent_styles_(bool is_extended) const{
	return (button::get_persistent_styles_(is_extended) | (is_extended ? 0u : SS_CENTER | SS_CENTERIMAGE));
}
