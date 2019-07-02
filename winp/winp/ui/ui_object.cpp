#include "../app/app_object.h"

#include "ui_tree.h"

winp::ui::object::object()
	: object(app::object::get_thread()){}

winp::ui::object::object(thread::object &thread)
	: item(thread){
	insert_hook<ui::auto_create_hook>();
}

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

winp::utility::error_code winp::ui::object::auto_create(const std::function<void(object &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, auto_create_());
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

winp::utility::error_code winp::ui::object::set_parent(tree &value, const std::function<void(object &, utility::error_code)> &callback){
	return set_parent(&value, callback);
}

winp::utility::error_code winp::ui::object::set_parent(tree &value, std::size_t index, const std::function<void(object &, utility::error_code)> &callback){
	return set_parent(&value, index, callback);
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

winp::ui::object *winp::ui::object::get_previous_sibling(const std::function<void(object *)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, get_sibling_(true));
	}, (callback != nullptr), nullptr);
}

winp::ui::object *winp::ui::object::get_next_sibling(const std::function<void(object *)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, get_sibling_(false));
	}, (callback != nullptr), nullptr);
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

void winp::ui::object::traverse_hooks(const std::function<bool(hook &)> &callback, bool block) const{
	execute_or_post_task_inside_thread_context([&]{
		traverse_hooks_(callback);
	}, !block);
}

void winp::ui::object::traverse_all_hooks(const std::function<void(hook &)> &callback, bool block) const{
	traverse_hooks([callback](hook &value){
		callback(value);
		return true;
	}, block);
}

bool winp::ui::object::is_dialog_message(MSG &msg) const{
	return compute_task_inside_thread_context([&]{
		return is_dialog_message_(msg);
	});
}

winp::utility::error_code winp::ui::object::destruct_(){
	destroy_();
	if (parent_ != nullptr){//Remove parent
		set_parent_(nullptr, static_cast<std::size_t>(-1));
		parent_ = nullptr;
	}

	return item::destruct_();
}

winp::utility::error_code winp::ui::object::create_(){
	return utility::error_code::not_supported;
}

winp::utility::error_code winp::ui::object::auto_create_(){
	if (is_created_())
		return utility::error_code::nil;
	return (has_hook<ui::auto_create_hook>() ? create_() : utility::error_code::action_could_not_be_completed);
}

winp::utility::error_code winp::ui::object::destroy_(){
	return utility::error_code::not_supported;
}

bool winp::ui::object::is_created_() const{
	return true;
}

winp::utility::error_code winp::ui::object::set_parent_(tree *value, std::size_t index){
	if (parent_ == value)
		return utility::error_code::duplicate_entry;

	if (value == nullptr)
		return parent_->erase_child_(get_index_());
	
	return value->insert_child_(*this, index);
}

winp::utility::error_code winp::ui::object::set_parent_value_(tree *value, bool changing){
	auto previous_value = parent_;
	if (changing)
		return (((trigger_event_<events::parent_change>(value, previous_value, true).first & events::object::state_default_prevented) == 0u) ? utility::error_code::nil : utility::error_code::action_prevented);

	parent_ = value;
	trigger_event_<events::parent_change>(value, previous_value, false);

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

winp::utility::error_code winp::ui::object::set_index_value_(std::size_t value, bool changing){
	if (changing)
		return (((trigger_event_<events::index_change>(value, true).first & events::object::state_default_prevented) == 0u) ? utility::error_code::nil : utility::error_code::action_prevented);

	trigger_event_<events::index_change>(value, false);
	return utility::error_code::nil;
}

std::size_t winp::ui::object::get_index_() const{
	return ((parent_ == nullptr) ? index_ : parent_->find_child_(*this));
}

winp::ui::object *winp::ui::object::get_sibling_(bool is_previous) const{
	if (parent_ == nullptr || parent_->children_.empty())
		return nullptr;//No siblings

	auto past_self = false;
	object *target_sibling = nullptr;

	for (auto sibling : parent_->children_){
		if (past_self)
			return (is_previous ? nullptr : sibling);

		if (sibling != this){
			if (is_previous)
				target_sibling = sibling;
		}
		else if (!is_previous)
			past_self = true;
		else
			break;
	}

	return target_sibling;
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

void winp::ui::object::traverse_hooks_(const std::function<bool(hook &)> &callback) const{
	if (hooks_.empty())
		return;//Empty list

	for (auto &info : hooks_){
		if (!callback(*info.second))
			return;
	}
}

bool winp::ui::object::is_dialog_message_(MSG &msg) const{
	return false;
}

void winp::ui::object::cancel_animation_(animation_hook::key_type key){
	if (auto animation_hk = find_hook_<animation_hook>(); animation_hk != nullptr)
		cancel_animation_(*animation_hk, key);
}

void winp::ui::object::cancel_animation_(animation_hook &hk, animation_hook::key_type key){
	hk.cancel_(key);
}

winp::ui::animation_hook::key_info &winp::ui::object::get_animation_info_(animation_hook &hk, animation_hook::key_type key) const{
	return hk.get_(key);
}

winp::ui::animation_hook::key_info *winp::ui::object::get_existing_animation_info_(animation_hook &hk, animation_hook::key_type key) const{
	return hk.get_existing_(key);
}
