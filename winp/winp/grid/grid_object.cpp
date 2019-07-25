#include "../app/app_object.h"

#include "grid_object.h"

winp::grid::object::object(){
	background_color_.a = 0.0f;
}

winp::grid::object::object(ui::tree &parent)
	: object(parent, static_cast<std::size_t>(-1)){}

winp::grid::object::object(ui::tree &parent, std::size_t index)
	: object(){
	set_parent(&parent, index);
}

winp::grid::object::~object() = default;

winp::utility::error_code winp::grid::object::refresh(const std::function<void(object &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, refresh_());
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::grid::object::do_insert_child_(ui::object &child, std::size_t index){
	if (dynamic_cast<row *>(&child) == nullptr)
		return utility::error_code::invalid_child;
	return non_window_surface::do_insert_child_(child, index);
}

void winp::grid::object::child_inserted_(ui::object &child){
	non_window_surface::child_inserted_(child);
	refresh_();
}

void winp::grid::object::child_erased_(ui::object &child){
	non_window_surface::child_erased_(child);
	refresh_();
}

winp::utility::error_code winp::grid::object::update_dimension_(const RECT &previous_dimension, int x, int y, int width, int height, UINT flags){
	auto error_code = non_window_surface::update_dimension_(previous_dimension, x, y, width, height, flags);
	if (!is_updating_)
		refresh_();

	return error_code;
}

winp::ui::surface::grid_type *winp::grid::object::get_grid_() const{
	return nullptr;
}

winp::utility::error_code winp::grid::object::refresh_(){
	if (children_.empty())
		return utility::error_code::nil;

	row *row_child = nullptr;
	std::unordered_map<int, int> fixed_heights;

	auto &current_size = get_current_size_();
	int fixed_height = 0, shared_count = 0, child_index = 0;

	is_updating_ = true;
	for (auto child : children_){//Compute fixed width
		if ((row_child = dynamic_cast<row *>(child)) == nullptr)
			continue;

		if (row_child->is_fixed_())
			fixed_height += (fixed_heights[child_index] = row_child->compute_fixed_height_(current_size.cy));
		else//Shared
			++shared_count;

		++child_index;
	}

	child_index = 0;
	int shared_height = ((current_size.cy < fixed_height) ? 0 : (current_size.cy - fixed_height)), height = 0, y = 0, used_shared_height = 0, updated_shared_count = 0;

	for (auto child : children_){
		if (auto ps_row_child = dynamic_cast<proportional_shared_row *>(child); ps_row_child != nullptr){
			shared_height -= (fixed_heights[child_index] = ps_row_child->compute_fixed_height_(shared_height));
			--shared_count;
		}

		++child_index;
	}

	child_index = 0;
	for (auto child : children_){//Update rows
		if ((row_child = dynamic_cast<row *>(child)) == nullptr)
			continue;

		if (fixed_heights.find(child_index) != fixed_heights.end())
			height = fixed_heights[child_index];
		else if (++updated_shared_count == shared_count)
			height = (shared_height - used_shared_height);
		else if (0 < shared_count)//Shared
			used_shared_height += (height = (shared_height / shared_count));

		row_child->update_(0, y, current_size.cx, height);
		y += height;
		++child_index;
	}

	is_updating_ = false;
	return utility::error_code::nil;
}
