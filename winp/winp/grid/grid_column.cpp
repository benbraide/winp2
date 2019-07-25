#include "../app/app_object.h"

#include "grid_row.h"

winp::grid::column::column() = default;

winp::grid::column::column(ui::tree &parent)
	: column(parent, static_cast<std::size_t>(-1)){}

winp::grid::column::column(ui::tree &parent, std::size_t index){
	set_parent(&parent, index);
}

winp::grid::column::~column() = default;

winp::utility::error_code winp::grid::column::refresh(const std::function<void(column &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, refresh_());
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::grid::column::set_parent_value_(ui::tree *value, bool changing){
	if (changing && value != nullptr && dynamic_cast<row *>(value) == nullptr)
		return utility::error_code::invalid_parent;
	return non_window_surface::set_parent_value_(value, changing);
}

int winp::grid::column::compute_fixed_width_(int row_width) const{
	return 0;
}

bool winp::grid::column::is_fixed_() const{
	return false;
}

void winp::grid::column::update_(int x, int y, int width, int height){
	is_updating_ = true;
	set_dimension_(x, y, width, height, 0u, false);
	is_updating_ = false;
}

winp::utility::error_code winp::grid::column::refresh_(){
	if (auto row_parent = dynamic_cast<row *>(parent_); row_parent != nullptr)
		return row_parent->refresh_();
	return utility::error_code::nil;
}

winp::grid::fixed_column::fixed_column() = default;

winp::grid::fixed_column::fixed_column(ui::tree &parent)
	: fixed_column(parent, static_cast<std::size_t>(-1)){}

winp::grid::fixed_column::fixed_column(ui::tree &parent, std::size_t index){
	set_parent(&parent, index);
}

winp::grid::fixed_column::~fixed_column() = default;

winp::utility::error_code winp::grid::fixed_column::update_dimension_(const RECT &previous_dimension, int x, int y, int width, int height, UINT flags){
	auto error_code = non_window_surface::update_dimension_(previous_dimension, x, y, width, height, flags);
	if (!is_updating_)
		refresh_();

	return error_code;
}

int winp::grid::fixed_column::compute_fixed_width_(int row_width) const{
	return get_current_size_().cx;
}

bool winp::grid::fixed_column::is_fixed_() const{
	return true;
}

winp::grid::proportional_column::proportional_column() = default;

winp::grid::proportional_column::proportional_column(ui::tree &parent)
	: proportional_column(parent, static_cast<std::size_t>(-1)){}

winp::grid::proportional_column::proportional_column(ui::tree &parent, std::size_t index){
	set_parent(&parent, index);
}

winp::grid::proportional_column::~proportional_column() = default;

winp::utility::error_code winp::grid::proportional_column::set_proportion(float value, const std::function<void(proportional_column &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_proportion_(value, true));
	}, (callback != nullptr), utility::error_code::nil);
}

float winp::grid::proportional_column::get_proportion(const std::function<void(float)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, value_);
	}, (callback != nullptr), 0.0f);
}

int winp::grid::proportional_column::compute_fixed_width_(int row_width) const{
	return static_cast<int>(row_width * value_);
}

winp::utility::error_code winp::grid::proportional_column::set_proportion_(float value, bool allow_animation){
	if (value == value_)
		return utility::error_code::nil;

	value_ = value;
	if (auto surface_parent = dynamic_cast<surface *>(parent_); surface_parent != nullptr)
		set_size_(compute_fixed_width_(surface_parent->get_current_size().cx), size_.cy, allow_animation);

	return utility::error_code::nil;
}

winp::grid::proportional_shared_column::proportional_shared_column() = default;

winp::grid::proportional_shared_column::proportional_shared_column(ui::tree &parent)
	: proportional_shared_column(parent, static_cast<std::size_t>(-1)){}

winp::grid::proportional_shared_column::proportional_shared_column(ui::tree &parent, std::size_t index){
	set_parent(&parent, index);
}

winp::grid::proportional_shared_column::~proportional_shared_column() = default;

winp::utility::error_code winp::grid::proportional_shared_column::set_proportion(float value, const std::function<void(proportional_shared_column &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_proportion_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

float winp::grid::proportional_shared_column::get_proportion(const std::function<void(float)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, value_);
	}, (callback != nullptr), 0.0f);
}

int winp::grid::proportional_shared_column::compute_fixed_width_(int shared_row_width) const{
	return static_cast<int>(shared_row_width * value_);
}

winp::utility::error_code winp::grid::proportional_shared_column::set_proportion_(float value){
	if (value != value_){
		value_ = value;
		refresh_();
	}

	return utility::error_code::nil;
}
