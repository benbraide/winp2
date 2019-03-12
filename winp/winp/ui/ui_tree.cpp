#include "../app/app_collection.h"

#include "ui_tree.h"

winp::ui::tree::tree()
	: tree(app::collection::get_main()->get_thread()){}

winp::ui::tree::tree(thread::object &thread)
	: object(thread){}

winp::ui::tree::~tree(){
	destruct();
}

winp::utility::error_code winp::ui::tree::add_child(object &child, const std::function<void(tree &, utility::error_code)> &callback){
	return insert_child(child, static_cast<std::size_t>(-1), callback);
}

winp::utility::error_code winp::ui::tree::insert_child(object &child, std::size_t index, const std::function<void(tree &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=, &child]{
		return pass_return_value_to_callback(callback, *this, insert_child_(child, index));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::tree::remove_child(object &child, const std::function<void(tree &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=, &child]{
		return pass_return_value_to_callback(callback, *this, erase_child_(child.get_index_()));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::tree::erase_child(std::size_t index, const std::function<void(tree &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, erase_child_(index));
	}, (callback != nullptr), utility::error_code::nil);
}

std::size_t winp::ui::tree::find_child(const object &child, const std::function<void(std::size_t)> &callback) const{
	return compute_or_post_task_inside_thread_context([=, &child]{
		return pass_return_value_to_callback(callback, find_child_(child));
	}, (callback != nullptr), static_cast<std::size_t>(-1));
}

winp::ui::object *winp::ui::tree::get_child_at(std::size_t index, const std::function<void(object *)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, get_child_at_(index));
	}, (callback != nullptr), nullptr);
}

const std::list<winp::ui::object *> &winp::ui::tree::get_children(const std::function<void(const std::list<object *> &)> &callback) const{
	return *compute_or_post_task_inside_thread_context([=]{
		return &pass_return_ref_value_to_callback(callback, &children_);
	}, (callback != nullptr), &children_);
}

void winp::ui::tree::traverse_children(const std::function<bool(object &)> &callback, bool block) const{
	execute_or_post_task_inside_thread_context([&]{
		traverse_children_(callback);
	}, !block);
}

void winp::ui::tree::traverse_all_children(const std::function<void(object &)> &callback, bool block) const{
	traverse_children([callback](object &value){
		callback(value);
		return true;
	}, block);
}

winp::utility::error_code winp::ui::tree::destruct_(){
	while (!children_.empty()){//Erase all children
		if (auto error_code = erase_child_(0); error_code != utility::error_code::nil)
			return error_code;
	}

	return object::destruct_();
}

winp::utility::error_code winp::ui::tree::insert_child_(object &child, std::size_t index){
	if (index != static_cast<std::size_t>(-1) && children_.size() <= index)
		return utility::error_code::index_out_of_range;

	if (&child.thread_ != &thread_)
		return utility::error_code::thread_context_mismatch;

	if (child.parent_ == this)
		return utility::error_code::duplicate_entry;

	utility::error_code error_code;
	auto previous_parent = child.parent_;
	
	if ((error_code = child.set_parent_value_(this, true)) != utility::error_code::nil)
		return error_code;

	if ((trigger_event_<events::children_change>(events::children_change::action_type::insert, child, index, true).first & events::object::state_default_prevented) != 0u)
		return utility::error_code::action_prevented;

	if (previous_parent != nullptr && (error_code = previous_parent->erase_child_(child.get_index_())) != utility::error_code::nil)//Remove previous parent
		return error_code;

	if (index < children_.size())
		children_.insert(std::next(children_.begin(), index), &child);
	else
		children_.push_back(&child);

	trigger_event_<events::children_change>(events::children_change::action_type::insert, child, index, false);
	child.set_parent_value_(this, false);

	return utility::error_code::nil;
}

winp::utility::error_code winp::ui::tree::erase_child_(std::size_t index){
	if (index >= children_.size())
		return utility::error_code::index_out_of_range;

	auto it = std::next(children_.begin(), index);
	auto &child = **it;

	if (auto error_code = child.set_parent_value_(nullptr, true); error_code != utility::error_code::nil)
		return error_code;

	if ((trigger_event_<events::children_change>(events::children_change::action_type::remove, child, index, true).first & events::object::state_default_prevented) != 0u)
		return utility::error_code::action_prevented;

	children_.erase(it);
	trigger_event_<events::children_change>(events::children_change::action_type::remove, child, index, false);
	child.set_parent_value_(nullptr, false);

	return utility::error_code::nil;
}

winp::utility::error_code winp::ui::tree::change_child_index_(std::size_t old_index, std::size_t new_index){
	if (children_.size() <= old_index || (new_index != static_cast<std::size_t>(-1) && children_.size() <= new_index))
		return utility::error_code::index_out_of_range;

	if (old_index == new_index || (new_index == static_cast<std::size_t>(-1) && (children_.size() - 1u) == old_index))
		return utility::error_code::nil;//No changes

	auto it = std::next(children_.begin(), old_index);
	auto child = *it;

	if (auto error_code = child->set_index_value_(new_index, true); error_code != utility::error_code::nil)
		return error_code;

	if ((trigger_event_<events::children_change>(events::children_change::action_type::index, *child, new_index, true).first & events::object::state_default_prevented) != 0u)
		return utility::error_code::action_prevented;

	children_.erase(it);
	if (new_index < children_.size())
		children_.insert(std::next(children_.begin(), new_index), child);
	else
		children_.push_back(child);

	trigger_event_<events::children_change>(events::children_change::action_type::index, *child, new_index, false);
	child->set_index_value_(new_index, false);

	return utility::error_code::nil;
}

std::size_t winp::ui::tree::find_child_(const object &child) const{
	if (child.parent_ != this || children_.empty())
		return static_cast<std::size_t>(-1);

	auto it = std::find(children_.begin(), children_.end(), &child);
	return ((it == children_.end()) ? static_cast<size_t>(-1) : static_cast<std::size_t>(std::distance(children_.begin(), it)));
}

winp::ui::object *winp::ui::tree::get_child_at_(std::size_t index) const{
	return ((index < children_.size()) ? *std::next(children_.begin(), index) : nullptr);
}

void winp::ui::tree::traverse_children_(const std::function<bool(object &)> &callback) const{
	if (!children_.empty()){
		for (auto child : children_){
			if (!callback(*child))
				break;
		}
	}
}
