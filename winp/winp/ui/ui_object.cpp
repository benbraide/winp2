#include "../app/app_collection.h"

#include "ui_tree.h"

winp::ui::object::object()
	: object(app::collection::get_main()->get_thread()){}

winp::ui::object::object(thread::object &thread)
	: item(thread){}

winp::ui::object::object(tree &parent)
	: object(parent, static_cast<std::size_t>(-1)){}

winp::ui::object::object(tree &parent, std::size_t index)
	: object(parent.thread_){
	set_parent(&parent, index);
}

winp::ui::object::~object(){
	destruct();
}

void winp::ui::object::create(const std::function<void(object &, utility::error_code)> &callback){
	execute_or_post_task_inside_thread_context([&]{
		try{
			create_();
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

void winp::ui::object::destroy(const std::function<void(object &, utility::error_code)> &callback){
	execute_or_post_task_inside_thread_context([&]{
		try{
			destroy_();
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

bool winp::ui::object::is_created(const std::function<void(bool)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, is_created_());
	}, (callback != nullptr), false);
}

void winp::ui::object::set_parent(tree *value, const std::function<void(object &, utility::error_code)> &callback){
	set_parent(value, index_, callback);
}

void winp::ui::object::set_parent(tree *value, std::size_t index, const std::function<void(object &, utility::error_code)> &callback){
	execute_or_post_task_inside_thread_context([&]{
		try{
			set_parent_(value, index);
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

winp::ui::tree *winp::ui::object::get_parent(const std::function<void(tree *)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, get_parent_());
	}, (callback != nullptr), nullptr);
}

bool winp::ui::object::is_ancestor(const tree &target, const std::function<void(bool)> &callback) const{
	return compute_or_post_task_inside_thread_context([=, &target]{
		return pass_return_value_to_callback(callback, is_ancestor_(target));
	}, (callback != nullptr), false);
}

void winp::ui::object::set_index(std::size_t value, const std::function<void(object &, utility::error_code)> &callback){
	execute_or_post_task_inside_thread_context([&]{
		try{
			set_index_(value);
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

std::size_t winp::ui::object::get_index(const std::function<void(std::size_t)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, get_index_());
	}, (callback != nullptr), static_cast<std::size_t>(-1));
}

void winp::ui::object::traverse_ancestors(const std::function<void(tree &)> &callback, bool block) const{
	execute_or_post_task_inside_thread_context([&]{
		traverse_ancestors_(callback);
	}, !block);
}

void winp::ui::object::traverse_siblings(const std::function<void(object &)> &callback, bool block) const{
	execute_or_post_task_inside_thread_context([&]{
		traverse_siblings_(callback);
	}, !block);
}

void winp::ui::object::destruct_(){
	set_parent_(nullptr, static_cast<std::size_t>(-1));//Remove parent
	item::destruct_();
}

void winp::ui::object::create_(){}

void winp::ui::object::destroy_(){}

bool winp::ui::object::is_created_() const{
	return false;
}

void winp::ui::object::set_parent_(tree *value, std::size_t index){
	if (value != nullptr)
		value->insert_child_(*this, index);
	else if (parent_ != nullptr)
		parent_->erase_child_(get_index_());
}

winp::ui::tree *winp::ui::object::get_parent_() const{
	return parent_;
}

bool winp::ui::object::is_ancestor_(const tree &target) const{
	return (parent_ != nullptr && (&target == parent_ || parent_->is_ancestor_(target)));
}

void winp::ui::object::set_index_(std::size_t value){
	if (parent_ == nullptr)
		index_ = value;
	else
		parent_->change_child_index_(get_index_(), value);
}

std::size_t winp::ui::object::get_index_() const{
	return ((parent_ == nullptr) ? index_ : parent_->find_child_(*this));
}

void winp::ui::object::traverse_ancestors_(const std::function<void(tree &)> &callback) const{
	for (auto ancestor = parent_; ancestor != nullptr; ancestor = ancestor->parent_)
		callback(*ancestor);
}

void winp::ui::object::traverse_siblings_(const std::function<void(object &)> &callback) const{
	if (parent_ == nullptr || parent_->children_.empty())
		return;//No siblings

	for (auto sibling : parent_->children_){
		if (sibling != this)
			callback(*sibling);
	}
}
