#include "../app/app_object.h"

#include "grid_object.h"

winp::grid::row::row()
	: row(app::object::get_thread()){}

winp::grid::row::row(thread::object &thread)
	: custom(thread){
	background_color_.a = 0.0f;
	add_event_handler_([this](events::create_non_window_handle &e) -> HRGN{
		if ((e.get_states() & events::object::state_result_set) == 0u)
			return CreateRectRgn(0, 0, size_.cx, size_.cy);
		return nullptr;
	});

	add_event_handler_([this](events::update_non_window_handle &e) -> HRGN{
		if ((e.get_states() & events::object::state_result_set) == 0u && SetRectRgn(e.get_handle(), 0, 0, size_.cx, size_.cy) != FALSE)
			return e.get_handle();
		return nullptr;
	});
}

winp::grid::row::row(ui::tree &parent)
	: row(parent, static_cast<std::size_t>(-1)){}

winp::grid::row::row(ui::tree &parent, std::size_t index)
	: row(parent.get_thread()){
	set_parent(&parent, index);
}

winp::grid::row::~row() = default;

winp::utility::error_code winp::grid::row::refresh(const std::function<void(row &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, refresh_());
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::grid::row::set_parent_value_(ui::tree *value, bool changing){
	if (changing && value != nullptr && dynamic_cast<grid::object *>(value) == nullptr)
		return utility::error_code::invalid_parent;
	return custom::set_parent_value_(value, changing);
}

winp::utility::error_code winp::grid::row::do_insert_child_(ui::object &child, std::size_t index){
	if (dynamic_cast<column *>(&child) == nullptr)
		return utility::error_code::invalid_child;
	return custom::do_insert_child_(child, index);
}

void winp::grid::row::child_inserted_(ui::object &child){
	custom::child_inserted_(child);
	refresh_();
}

void winp::grid::row::child_erased_(ui::object &child){
	custom::child_erased_(child);
	refresh_();
}

winp::ui::surface::grid_type *winp::grid::row::get_grid_() const{
	return nullptr;
}

int winp::grid::row::compute_fixed_height_(int grid_height) const{
	return 0;
}

bool winp::grid::row::is_fixed_() const{
	return false;
}

void winp::grid::row::update_(int x, int y, int width, int height){
	is_updating_ = true;
	set_dimension_(x, y, width, height);

	if (children_.empty()){
		is_updating_ = false;
		return;
	}

	column *column_child = nullptr;
	std::unordered_map<int, int> fixed_widths;

	int fixed_width = 0, shared_count = 0, child_index = 0;
	for (auto child : children_){//Compute fixed width
		if ((column_child = dynamic_cast<column *>(child)) == nullptr)
			continue;

		if (column_child->is_fixed_())
			fixed_width += (fixed_widths[child_index] = column_child->compute_fixed_width_(size_.cx));
		else//Shared
			++shared_count;

		++child_index;
	}

	child_index = 0;
	int shared_width = ((size_.cx < fixed_width) ? 0 : (size_.cx - fixed_width)), used_shared_width = 0, updated_shared_count = 0;

	for (auto child : children_){
		if (auto ps_column_child = dynamic_cast<proportional_shared_column *>(child); ps_column_child != nullptr){
			shared_width -= (fixed_widths[child_index] = ps_column_child->compute_fixed_width_(shared_width));
			--shared_count;
		}

		++child_index;
	}

	x = 0;
	child_index = 0;
	
	for (auto child : children_){//Update columns
		if ((column_child = dynamic_cast<column *>(child)) == nullptr)
			continue;

		if (fixed_widths.find(child_index) != fixed_widths.end())
			width = fixed_widths[child_index];
		else if (++updated_shared_count == shared_count)
			width = (shared_width - used_shared_width);
		else if (0 < shared_count)//Shared
			used_shared_width += (width = (shared_width / shared_count));

		column_child->update_(x, 0, width, size_.cy);
		x += width;
		++child_index;
	}

	is_updating_ = false;
}

winp::utility::error_code winp::grid::row::refresh_(){
	if (auto object_parent = dynamic_cast<grid::object *>(parent_); object_parent != nullptr)
		return object_parent->refresh_();
	return utility::error_code::nil;
}

winp::grid::fixed_row::fixed_row()
	: fixed_row(app::object::get_thread()){}

winp::grid::fixed_row::fixed_row(thread::object &thread)
	: row(thread){}

winp::grid::fixed_row::fixed_row(ui::tree &parent)
	: fixed_row(parent, static_cast<std::size_t>(-1)){}

winp::grid::fixed_row::fixed_row(ui::tree &parent, std::size_t index)
	: fixed_row(parent.get_thread()){
	set_parent(&parent, index);
}

winp::grid::fixed_row::~fixed_row() = default;

winp::utility::error_code winp::grid::fixed_row::set_dimension_(int x, int y, int width, int height){
	auto error_code = non_window_surface::set_dimension_(x, y, width, height);
	if (!is_updating_)
		refresh_();

	return error_code;
}

int winp::grid::fixed_row::compute_fixed_height_(int grid_height) const{
	return size_.cy;
}

bool winp::grid::fixed_row::is_fixed_() const{
	return true;
}

winp::grid::proportional_row::proportional_row()
	: proportional_row(app::object::get_thread()){}

winp::grid::proportional_row::proportional_row(thread::object &thread)
	: fixed_row(thread){}

winp::grid::proportional_row::proportional_row(ui::tree &parent)
	: proportional_row(parent, static_cast<std::size_t>(-1)){}

winp::grid::proportional_row::proportional_row(ui::tree &parent, std::size_t index)
	: proportional_row(parent.get_thread()){
	set_parent(&parent, index);
}

winp::grid::proportional_row::~proportional_row() = default;

winp::utility::error_code winp::grid::proportional_row::set_proportion(float value, const std::function<void(proportional_row &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_proportion_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

float winp::grid::proportional_row::get_proportion(const std::function<void(float)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, value_);
	}, (callback != nullptr), 0.0f);
}

int winp::grid::proportional_row::compute_fixed_height_(int grid_height) const{
	return static_cast<int>(grid_height * value_);
}

winp::utility::error_code winp::grid::proportional_row::set_proportion_(float value){
	if (value == value_){
		value_ = value;
		refresh_();
	}

	return utility::error_code::nil;
}

winp::grid::proportional_shared_row::proportional_shared_row()
	: proportional_shared_row(app::object::get_thread()){}

winp::grid::proportional_shared_row::proportional_shared_row(thread::object &thread)
	: row(thread){}

winp::grid::proportional_shared_row::proportional_shared_row(ui::tree &parent)
	: proportional_shared_row(parent, static_cast<std::size_t>(-1)){}

winp::grid::proportional_shared_row::proportional_shared_row(ui::tree &parent, std::size_t index)
	: proportional_shared_row(parent.get_thread()){
	set_parent(&parent, index);
}

winp::grid::proportional_shared_row::~proportional_shared_row() = default;

winp::utility::error_code winp::grid::proportional_shared_row::set_proportion(float value, const std::function<void(proportional_shared_row &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_proportion_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

float winp::grid::proportional_shared_row::get_proportion(const std::function<void(float)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, value_);
	}, (callback != nullptr), 0.0f);
}

int winp::grid::proportional_shared_row::compute_fixed_height_(int shared_row_height) const{
	return static_cast<int>(shared_row_height * value_);
}

winp::utility::error_code winp::grid::proportional_shared_row::set_proportion_(float value){
	if (value != value_){
		value_ = value;
		refresh_();
	}

	return utility::error_code::nil;
}
