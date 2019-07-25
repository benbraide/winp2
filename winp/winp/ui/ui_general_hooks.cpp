#include "../app/app_object.h"
#include "../utility/animation_timing.h"

#include "ui_general_hooks.h"

winp::ui::auto_create_hook::auto_create_hook(object &target)
	: hook(target){}

winp::ui::auto_create_hook::~auto_create_hook() = default;

winp::ui::animation_hook::animation_hook(object &target)
	: animation_hook(target, utility::animation_timing::linear::ease, std::chrono::milliseconds(500)){}

winp::ui::animation_hook::animation_hook(object &target, const easing_type &easing)
	: animation_hook(target, easing, std::chrono::milliseconds(500)){}

winp::ui::animation_hook::animation_hook(object &target, const std::chrono::microseconds &duration)
	: animation_hook(target, utility::animation_timing::linear::ease, duration){}

winp::ui::animation_hook::animation_hook(object &target, const easing_type &easing, const std::chrono::microseconds &duration)
	: hook(target), easing_(easing), duration_(duration){}

winp::ui::animation_hook::~animation_hook() = default;

winp::utility::error_code winp::ui::animation_hook::set_easing(const easing_type &value, const std::function<void(animation_hook &, utility::error_code)> &callback){
	return target_.compute_or_post_task_inside_thread_context([=]{
		return target_.pass_return_value_to_callback(callback, *this, set_easing_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::animation_hook::set_easing(key_type key, const easing_type &value, const std::function<void(animation_hook &, utility::error_code)> &callback){
	return target_.compute_or_post_task_inside_thread_context([=]{
		return target_.pass_return_value_to_callback(callback, *this, set_easing_(key, value));
	}, (callback != nullptr), utility::error_code::nil);
}

const winp::ui::animation_hook::easing_type &winp::ui::animation_hook::get_easing(const std::function<void(const easing_type &)> &callback) const{
	return *target_.compute_or_post_task_inside_thread_context([=]{
		return &target_.pass_return_ref_value_to_callback(callback, &easing_);
	}, (callback != nullptr), &easing_);
}

const winp::ui::animation_hook::easing_type &winp::ui::animation_hook::get_easing(key_type key, const std::function<void(const easing_type &)> &callback) const{
	return *target_.compute_or_post_task_inside_thread_context([=]{
		return &target_.pass_return_ref_value_to_callback(callback, &get_easing(key));
	}, (callback != nullptr), &easing_);
}

winp::utility::error_code winp::ui::animation_hook::set_duration(const std::chrono::microseconds &value, const std::function<void(animation_hook &, utility::error_code)> &callback){
	return target_.compute_or_post_task_inside_thread_context([=]{
		return target_.pass_return_value_to_callback(callback, *this, set_duration_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::animation_hook::set_duration(key_type key, const std::chrono::microseconds &value, const std::function<void(animation_hook &, utility::error_code)> &callback){
	return target_.compute_or_post_task_inside_thread_context([=]{
		return target_.pass_return_value_to_callback(callback, *this, set_duration_(key, value));
	}, (callback != nullptr), utility::error_code::nil);
}

const std::chrono::microseconds &winp::ui::animation_hook::get_duration(const std::function<void(const std::chrono::microseconds &)> &callback) const{
	return *target_.compute_or_post_task_inside_thread_context([=]{
		return &target_.pass_return_ref_value_to_callback(callback, &duration_);
	}, (callback != nullptr), &duration_);
}

const std::chrono::microseconds &winp::ui::animation_hook::get_duration(key_type key, const std::function<void(const std::chrono::microseconds &)> &callback) const{
	return *target_.compute_or_post_task_inside_thread_context([=]{
		return &target_.pass_return_ref_value_to_callback(callback, &get_duration_(key));
	}, (callback != nullptr), &duration_);
}

void winp::ui::animation_hook::set_allowed_state(bool is_allowed){
	target_.execute_task_inside_thread_context([=]{
		is_allowed_ = is_allowed;
	});
}

void winp::ui::animation_hook::set_allowed_state(key_type key, bool is_allowed){
	target_.execute_task_inside_thread_context([=]{
		if (!info_list_.empty()){
			if (auto it = info_list_.find(key); it != info_list_.end())
				it->second.is_allowed = is_allowed;
		}
	});
}

bool winp::ui::animation_hook::is_allowed() const{
	return target_.compute_task_inside_thread_context([=]{
		return is_allowed_;
	});
}

bool winp::ui::animation_hook::is_allowed(key_type key) const{
	return target_.compute_task_inside_thread_context([=]{
		if (info_list_.empty())
			return is_allowed_;

		if (auto it = info_list_.find(key); it != info_list_.end())
			return it->second.is_allowed;

		return is_allowed_;
	});
}

bool winp::ui::animation_hook::is_active(key_type key) const{
	return target_.compute_task_inside_thread_context([=]{
		if (info_list_.empty())
			return false;

		if (auto it = info_list_.find(key); it != info_list_.end())
			return it->second.is_active;

		return false;
	});
}

bool winp::ui::animation_hook::is_active() const{
	return target_.compute_task_inside_thread_context([=]{
		if (info_list_.empty())
			return false;

		for (auto &info : info_list_){
			if (info.second.is_active)
				return true;
		}

		return false;
	});
}

winp::utility::error_code winp::ui::animation_hook::set_easing_(const easing_type &value){
	easing_ = value;
	return utility::error_code::nil;
}

winp::utility::error_code winp::ui::animation_hook::set_easing_(key_type key, const easing_type &value){
	if (auto it = info_list_.find(key); it != info_list_.end())
		it->second.easing = value;
	else
		info_list_[key] = key_info{ true, false, value, duration_, std::any(), 0u };

	return utility::error_code::nil;
}

const winp::ui::animation_hook::easing_type &winp::ui::animation_hook::get_easing_(key_type key) const{
	if (info_list_.empty())
		return easing_;

	if (auto it = info_list_.find(key); it != info_list_.end())
		return it->second.easing;

	return easing_;
}

winp::utility::error_code winp::ui::animation_hook::set_duration_(const std::chrono::microseconds &value){
	duration_ = value;
	return utility::error_code::nil;
}

winp::utility::error_code winp::ui::animation_hook::set_duration_(key_type key, const std::chrono::microseconds &value){
	if (auto it = info_list_.find(key); it != info_list_.end())
		it->second.duration = value;
	else
		info_list_[key] = key_info{ true, false, easing_, value, std::any(), 0u };

	return utility::error_code::nil;
}

const std::chrono::microseconds &winp::ui::animation_hook::get_duration_(key_type key) const{
	if (info_list_.empty())
		return duration_;

	if (auto it = info_list_.find(key); it != info_list_.end())
		return it->second.duration;

	return duration_;
}

void winp::ui::animation_hook::cancel_(key_type key){
	if (auto data = get_existing_(key); data != nullptr && data->is_active){
		data->is_active = false;
		++data->count;
	}
}

winp::ui::animation_hook::key_info &winp::ui::animation_hook::get_(key_type key) const{
	if (auto it = info_list_.find(key); it != info_list_.end())
		return it->second;

	return (info_list_[key] = key_info{ is_allowed_, false, easing_, duration_, std::any(), 0u });
}

winp::ui::animation_hook::key_info *winp::ui::animation_hook::get_existing_(key_type key) const{
	if (info_list_.empty())
		return nullptr;

	if (auto it = info_list_.find(key); it != info_list_.end())
		return &it->second;

	return nullptr;
}

winp::ui::animation_suppression_hook::animation_suppression_hook(object &target, bool once)
	: hook(target), once_(once){}

winp::ui::animation_suppression_hook::~animation_suppression_hook() = default;

bool winp::ui::animation_suppression_hook::is_once(const std::function<void(bool)> &callback) const{
	return target_.compute_or_post_task_inside_thread_context([=]{
		return target_.pass_return_value_to_callback(callback, once_);
	}, (callback != nullptr), false);
}

void winp::ui::animation_suppression_hook::suppress_type(key_type key){
	target_.execute_task_inside_thread_context([=]{
		suppressed_list_[key] = true;
	});
}

void winp::ui::animation_suppression_hook::unsuppress_type(key_type key){
	target_.execute_task_inside_thread_context([=]{
		if (!suppressed_list_.empty())
			suppressed_list_.erase(key);
	});
}

bool winp::ui::animation_suppression_hook::type_is_suppressed(key_type key) const{
	return target_.compute_task_inside_thread_context([=]{
		if (suppressed_list_.empty())
			return true;

		auto it = suppressed_list_.find(key);
		return (it != suppressed_list_.end() && it->second);
	});
}
