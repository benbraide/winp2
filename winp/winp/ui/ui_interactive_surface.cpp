#include "ui_window_surface.h"

winp::ui::interactive_surface::~interactive_surface() = default;

bool winp::ui::interactive_surface::is_dialog_message(MSG &msg, const std::function<void(bool)> &callback) const{
	return synchronized_item_compute_or_post_task_inside_thread_context([=, &msg]{
		return synchronized_item_pass_return_value_to_callback(callback, is_dialog_message_(msg));
	}, (callback != nullptr), false);
}

bool winp::ui::interactive_surface::is_dialog_message_(MSG &msg) const{
	return false;
}

winp::ui::interactive_surface *winp::ui::interactive_surface::find_deepest_moused_(const POINT &absolute_position) const{
	auto tree_self = dynamic_cast<const tree *>(this);
	if (tree_self == nullptr)
		return nullptr;

	interactive_surface *deepest_moused = nullptr;
	tree_self->traverse_children([&](object &child){
		if (!child.is_created() || dynamic_cast<window_surface *>(&child) != nullptr)
			return true;

		if (auto interactive_child = dynamic_cast<interactive_surface *>(&child); interactive_child != nullptr && dynamic_cast<window_surface *>(&child) == nullptr){
			if (interactive_child->is_visible() && interactive_child->absolute_hit_test(absolute_position) != HTNOWHERE && (deepest_moused = interactive_child->find_deepest_moused_(absolute_position)) == nullptr)
				deepest_moused = interactive_child;
		}

		return (deepest_moused == nullptr);
	}, true);

	return deepest_moused;
}
