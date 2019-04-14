#include "../app/app_collection.h"

#include "grid_object.h"

winp::grid::object::object()
	: object(app::collection::get_main()->get_thread()){}

winp::grid::object::object(thread::object &thread)
	: rectangle(thread, false){}

winp::grid::object::object(ui::tree &parent)
	: object(parent, static_cast<std::size_t>(-1)){}

winp::grid::object::object(ui::tree &parent, std::size_t index)
	: object(parent.get_thread()){
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
	rectangle::child_inserted_(child);
	refresh_();
}

void winp::grid::object::child_erased_(ui::object &child){
	rectangle::child_erased_(child);
	refresh_();
}

winp::utility::error_code winp::grid::object::set_dimension_(int x, int y, int width, int height){
	auto error_code = non_window_surface::set_dimension_(x, y, width, height);
	if (!is_updating_)
		refresh_();

	return error_code;
}

bool winp::grid::object::is_transparent_background_() const{
	return true;
}

winp::utility::error_code winp::grid::object::refresh_(){
	if (children_.empty())
		return utility::error_code::nil;

	row *row_child = nullptr;
	std::vector<int> fixed_heights(children_.size(), 0);
	int fixed_height = 0, shared_count = 0, child_index = 0;

	is_updating_ = true;
	for (auto child : children_){//Compute fixed width
		if ((row_child = dynamic_cast<row *>(child)) == nullptr)
			continue;

		if (row_child->is_fixed_())
			fixed_height += (fixed_heights[child_index] = row_child->compute_fixed_height_(size_.cy));
		else//Shared
			++shared_count;

		++child_index;
	}

	child_index = 0;
	int shared_height = (((size_.cy - fixed_height) < 0) ? 0 : (size_.cy - fixed_height)), height = 0, y = 0, used_shared_height = 0, updated_shared_count = 0;

	for (auto child : children_){//Update rows
		if ((row_child = dynamic_cast<row *>(child)) == nullptr)
			continue;

		if (row_child->is_fixed_())
			height = fixed_heights[child_index];
		else if (++updated_shared_count == shared_count)
			height = (shared_height - used_shared_height);
		else if (0 < shared_count)//Shared
			height = (shared_height / shared_count);

		row_child->update_(0, y, size_.cx, height);
		used_shared_height += height;

		y += height;
		++child_index;
	}

	is_updating_ = false;
	return utility::error_code::nil;
}
