#include "../app/app_collection.h"

#include "ui_tree.h"

winp::ui::tree::tree()
	: tree(app::collection::get_main()->get_thread()){}

winp::ui::tree::tree(thread::object &thread)
	: object(thread){}

winp::ui::tree::~tree(){
	destruct();
}

void winp::ui::tree::add_child(object &child, const std::function<void(tree &, utility::error_code)> &callback){
	insert_child(child, static_cast<std::size_t>(-1), callback);
}

void winp::ui::tree::insert_child(object &child, std::size_t index, const std::function<void(tree &, utility::error_code)> &callback){
	execute_or_post_task_inside_thread_context([&]{
		try{
			insert_child_(child, index);
			if (callback != nullptr)
				callback(*this, utility::error_code::nil);
		}
		catch (utility::error_code e){
			if (callback != nullptr)
				callback(*this, e);
			else//Forward exception
				throw;
		}
	}, (callback != nullptr));
}

void winp::ui::tree::remove_child(object &child, const std::function<void(tree &, utility::error_code)> &callback){
	execute_or_post_task_inside_thread_context([&]{
		try{
			erase_child_(child.get_index_());
			if (callback != nullptr)
				callback(*this, utility::error_code::nil);
		}
		catch (utility::error_code e){
			if (callback != nullptr)
				callback(*this, e);
			else//Forward exception
				throw;
		}
	}, (callback != nullptr));
}

void winp::ui::tree::erase_child(std::size_t index, const std::function<void(tree &, utility::error_code)> &callback){
	execute_or_post_task_inside_thread_context([&]{
		try{
			erase_child_(index);
			if (callback != nullptr)
				callback(*this, utility::error_code::nil);
		}
		catch (utility::error_code e){
			if (callback != nullptr)
				callback(*this, e);
			else//Forward exception
				throw;
		}
	}, (callback != nullptr));
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

void winp::ui::tree::traverse_children(const std::function<void(object &)> &callback, bool block) const{
	execute_or_post_task_inside_thread_context([&]{
		traverse_children_(callback);
	}, !block);
}

void winp::ui::tree::destruct_(){
	while (!children_.empty())//Erase all children
		erase_child_(0);
	object::destruct_();
}

void winp::ui::tree::insert_child_(object &child, std::size_t index){
	if (index != static_cast<std::size_t>(-1) && children_.size() <= index)
		throw utility::error_code::index_out_of_range;

	if (&child.thread_ != &thread_)
		throw utility::error_code::thread_context_mismatch;

	if (child.parent_ == this)
		throw utility::error_code::duplicate_entry;

	if (child.parent_ != nullptr)//Remove previous parent
		child.parent_->erase_child_(child.get_index_());

	if ((trigger_event_<events::children_change>(events::children_change::action_type::insert, child, index, true).first & events::object::state_default_prevented) != 0u)
		throw utility::error_code::action_prevented;

	child.set_parent_value_(this);
	if (index < children_.size())
		children_.insert(std::next(children_.begin(), index), &child);
	else
		children_.push_back(&child);

	trigger_event_<events::children_change>(events::children_change::action_type::insert, child, index, false);
}

void winp::ui::tree::erase_child_(std::size_t index){
	if (index >= children_.size())
		throw utility::error_code::index_out_of_range;

	auto it = std::next(children_.begin(), index);
	auto &child = **it;

	if ((trigger_event_<events::children_change>(events::children_change::action_type::remove, child, index, true).first & events::object::state_default_prevented) != 0u)
		throw utility::error_code::action_prevented;

	child.set_parent_value_(nullptr);
	children_.erase(it);

	trigger_event_<events::children_change>(events::children_change::action_type::remove, child, index, false);
}

void winp::ui::tree::change_child_index_(std::size_t old_index, std::size_t new_index){
	if (children_.size() <= old_index || (new_index != static_cast<std::size_t>(-1) && children_.size() <= new_index))
		throw utility::error_code::index_out_of_range;

	if (old_index == new_index || (new_index == static_cast<std::size_t>(-1) && (children_.size() - 1u) == old_index))
		return;//No changes

	auto it = std::next(children_.begin(), old_index);
	auto child = *it;

	if ((trigger_event_<events::children_change>(events::children_change::action_type::index, *child, new_index, true).first & events::object::state_default_prevented) != 0u)
		throw utility::error_code::action_prevented;

	children_.erase(it);
	if (new_index < children_.size())
		children_.insert(std::next(children_.begin(), new_index), child);
	else
		children_.push_back(child);

	trigger_event_<events::children_change>(events::children_change::action_type::index, *child, new_index, false);
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

void winp::ui::tree::traverse_children_(const std::function<void(object &)> &callback) const{
	if (!children_.empty()){
		for (auto child : children_)
			callback(*child);
	}
}
