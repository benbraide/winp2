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

winp::utility::error_code winp::ui::object::create(const std::function<void(object &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, create_());
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::object::destroy(const std::function<void(object &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, destroy_());
	}, (callback != nullptr), utility::error_code::nil);
}

bool winp::ui::object::is_created(const std::function<void(bool)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, is_created_());
	}, (callback != nullptr), false);
}

winp::utility::error_code winp::ui::object::set_parent(tree *value, const std::function<void(object &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_parent_(value, index_));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::object::set_parent(tree *value, std::size_t index, const std::function<void(object &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_parent_(value, index));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::object::remove_from_parent(const std::function<void(object &, utility::error_code)> &callback){
	return set_parent(nullptr, static_cast<std::size_t>(-1), callback);
}

winp::ui::tree *winp::ui::object::get_parent(const std::function<void(tree *)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, get_parent_());
	}, (callback != nullptr), nullptr);
}

winp::ui::tree *winp::ui::object::get_top_ancestor(const std::function<void(tree *)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, get_top_ancestor_());
	}, (callback != nullptr), nullptr);
}

bool winp::ui::object::is_ancestor(const tree &target, const std::function<void(bool)> &callback) const{
	return compute_or_post_task_inside_thread_context([=, &target]{
		return pass_return_value_to_callback(callback, is_ancestor_(target));
	}, (callback != nullptr), false);
}

winp::utility::error_code winp::ui::object::set_index(std::size_t value, const std::function<void(object &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_index_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

std::size_t winp::ui::object::get_index(const std::function<void(std::size_t)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, get_index_());
	}, (callback != nullptr), static_cast<std::size_t>(-1));
}

void winp::ui::object::traverse_ancestors(const std::function<bool(tree &)> &callback, bool block) const{
	execute_or_post_task_inside_thread_context([&]{
		traverse_ancestors_(callback);
	}, !block);
}

void winp::ui::object::traverse_all_ancestors(const std::function<void(tree &)> &callback, bool block) const{
	traverse_ancestors([callback](tree &value){
		callback(value);
		return true;
	}, block);
}

void winp::ui::object::traverse_siblings(const std::function<bool(object &)> &callback, bool block) const{
	execute_or_post_task_inside_thread_context([&]{
		traverse_siblings_(callback);
	}, !block);
}

void winp::ui::object::traverse_all_siblings(const std::function<void(object &)> &callback, bool block) const{
	traverse_siblings([callback](object &value){
		callback(value);
		return true;
	}, block);
}

winp::utility::error_code winp::ui::object::destruct_(){
	if (parent_ != nullptr){//Remove parent
		if (auto error_code = set_parent_(nullptr, static_cast<std::size_t>(-1)); error_code != utility::error_code::nil)
			return error_code;
	}

	if (auto error_code = destroy_(); error_code != utility::error_code::nil && error_code != utility::error_code::not_supported)
		return error_code;

	return item::destruct_();
}

winp::utility::error_code winp::ui::object::create_(){
	return utility::error_code::not_supported;
}

winp::utility::error_code winp::ui::object::destroy_(){
	return utility::error_code::not_supported;
}

bool winp::ui::object::is_created_() const{
	return false;
}

winp::utility::error_code winp::ui::object::set_parent_(tree *value, std::size_t index){
	if (parent_ == value)
		return utility::error_code::duplicate_entry;

	if (value == nullptr)
		return parent_->erase_child_(get_index_());
	
	return value->insert_child_(*this, index);
}

winp::utility::error_code winp::ui::object::set_parent_value_(tree *value){
	if ((trigger_event_<events::parent_change>(value, true).first & events::object::state_default_prevented) != 0u)
		return utility::error_code::action_prevented;

	parent_ = value;
	trigger_event_<events::parent_change>(value, false);

	return utility::error_code::nil;
}

winp::ui::tree *winp::ui::object::get_parent_() const{
	return parent_;
}

winp::ui::tree *winp::ui::object::get_top_ancestor_() const{
	if (auto parent_top_ancestor = ((parent_ == nullptr) ? nullptr : parent_->get_top_ancestor_()); parent_top_ancestor != nullptr)
		return parent_top_ancestor;
	return parent_;
}

bool winp::ui::object::is_ancestor_(const tree &target) const{
	return (parent_ != nullptr && (&target == parent_ || parent_->is_ancestor_(target)));
}

winp::utility::error_code winp::ui::object::set_index_(std::size_t value){
	if (parent_ == nullptr)
		index_ = value;
	else if (auto error_code = parent_->change_child_index_(get_index_(), value); error_code != utility::error_code::nil)
		return error_code;

	return utility::error_code::nil;
}

std::size_t winp::ui::object::get_index_() const{
	return ((parent_ == nullptr) ? index_ : parent_->find_child_(*this));
}

void winp::ui::object::traverse_ancestors_(const std::function<bool(tree &)> &callback) const{
	for (auto ancestor = parent_; ancestor != nullptr; ancestor = ancestor->parent_){
		if (!callback(*ancestor))
			break;
	}
}

void winp::ui::object::traverse_siblings_(const std::function<bool(object &)> &callback) const{
	if (parent_ == nullptr || parent_->children_.empty())
		return;//No siblings

	for (auto sibling : parent_->children_){
		if (sibling != this && !callback(*sibling))
			break;
	}
}
