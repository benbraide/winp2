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

winp::ui::interactive_surface *winp::ui::interactive_surface::get_deepest_moused_() const{
	return ((moused_ == nullptr) ? const_cast<interactive_surface *>(this) : moused_->get_deepest_moused_());
}

winp::ui::interactive_surface *winp::ui::interactive_surface::find_deepest_moused_(const POINT &absolute_position) const{
	auto tree_self = dynamic_cast<const tree *>(this);
	if (tree_self == nullptr)
		return nullptr;

	interactive_surface *deepest_moused = nullptr;
	tree_self->traverse_children([&](object &child){
		if (dynamic_cast<window_surface *>(&child) != nullptr)
			return true;

		if (auto interactive_child = dynamic_cast<interactive_surface *>(&child); interactive_child != nullptr){
			if (interactive_child->hit_test(absolute_position) != HTNOWHERE && (deepest_moused = interactive_child->find_deepest_moused_(absolute_position)) == nullptr)
				deepest_moused = interactive_child;
		}

		return (deepest_moused == nullptr);
	}, true);

	return deepest_moused;
}

void winp::ui::interactive_surface::create_moused_link_with_ancestors_(){
	if (auto object_self = dynamic_cast<object *>(this); object_self != nullptr){
		if (auto interactive_parent = dynamic_cast<interactive_surface *>(object_self->get_parent()); interactive_parent != nullptr && interactive_parent->moused_ != this){
			interactive_parent->moused_ = this;
			interactive_parent->create_moused_link_with_ancestors_();
		}
	}
}

void winp::ui::interactive_surface::remove_moused_link_(){
	if (moused_ != nullptr){
		moused_->remove_moused_link_();
		moused_ = nullptr;
	}
}
