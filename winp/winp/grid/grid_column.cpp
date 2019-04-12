#include "../app/app_collection.h"

#include "grid_row.h"

winp::grid::column::column()
	: column(app::collection::get_main()->get_thread()){}

winp::grid::column::column(thread::object &thread)
	: rectangle(thread){}

winp::grid::column::column(ui::tree &parent)
	: column(parent, static_cast<std::size_t>(-1)){}

winp::grid::column::column(ui::tree &parent, std::size_t index)
	: column(parent.get_thread()){
	set_parent(&parent, index);
}

winp::grid::column::~column(){
	destruct();
}

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
	set_dimension_(x, y, width, height);
	is_updating_ = false;
}

winp::utility::error_code winp::grid::column::refresh_(){
	if (auto row_parent = dynamic_cast<row *>(parent_); row_parent != nullptr)
		return row_parent->refresh_();
	return utility::error_code::nil;
}

winp::grid::fixed_column::fixed_column()
	: fixed_column(app::collection::get_main()->get_thread()){}

winp::grid::fixed_column::fixed_column(thread::object &thread)
	: column(thread){}

winp::grid::fixed_column::fixed_column(ui::tree &parent)
	: fixed_column(parent, static_cast<std::size_t>(-1)){}

winp::grid::fixed_column::fixed_column(ui::tree &parent, std::size_t index)
	: fixed_column(parent.get_thread()){
	set_parent(&parent, index);
}

winp::grid::fixed_column::~fixed_column() = default;

winp::utility::error_code winp::grid::fixed_column::set_dimension_(int x, int y, int width, int height){
	auto error_code = non_window_surface::set_dimension_(x, y, width, height);
	if (!is_updating_)
		refresh_();

	return error_code;
}

int winp::grid::fixed_column::compute_fixed_width_(int row_width) const{
	return size_.cx;
}

bool winp::grid::fixed_column::is_fixed_() const{
	return true;
}

winp::grid::proportional_column::proportional_column()
	: proportional_column(app::collection::get_main()->get_thread()){}

winp::grid::proportional_column::proportional_column(thread::object &thread)
	: fixed_column(thread){}

winp::grid::proportional_column::proportional_column(ui::tree &parent)
	: proportional_column(parent, static_cast<std::size_t>(-1)){}

winp::grid::proportional_column::proportional_column(ui::tree &parent, std::size_t index)
	: proportional_column(parent.get_thread()){
	set_parent(&parent, index);
}

winp::grid::proportional_column::~proportional_column() = default;

winp::utility::error_code winp::grid::proportional_column::set_proportion(float value, const std::function<void(proportional_column &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_proportion_(value));
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

winp::utility::error_code winp::grid::proportional_column::set_proportion_(float value){
	if (value != value_){
		value_ = value;
		refresh_();
	}

	return utility::error_code::nil;
}
