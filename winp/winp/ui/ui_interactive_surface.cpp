#include "ui_interactive_surface.h"

winp::ui::interactive_surface::~interactive_surface() = default;

bool winp::ui::interactive_surface::is_dialog_message(MSG &msg, const std::function<void(bool)> &callback) const{
	return synchronized_item_compute_or_post_task_inside_thread_context([=, &msg]{
		return synchronized_item_pass_return_value_to_callback(callback, is_dialog_message_(msg));
	}, (callback != nullptr), false);
}

bool winp::ui::interactive_surface::is_dialog_message_(MSG &msg) const{
	return false;
}
