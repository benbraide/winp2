#include "../app/app_collection.h"

#include "ui_tree.h"
#include "ui_surface.h"

winp::ui::surface::~surface() = default;

void winp::ui::surface::set_size(const SIZE &value, const std::function<void(surface &, utility::error_code)> &callback){
	set_size(value.cx, value.cy, callback);
}

void winp::ui::surface::set_size(int width, int height, const std::function<void(surface &, utility::error_code)> &callback){
	synchronized_item_execute_or_post_task_inside_thread_context([&]{
		try{
			set_size_(width, height);
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

void winp::ui::surface::set_width(int value, const std::function<void(surface &, utility::error_code)> &callback){
	synchronized_item_execute_or_post_task_inside_thread_context([&]{
		try{
			set_width_(value);
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

void winp::ui::surface::set_height(int value, const std::function<void(surface &, utility::error_code)> &callback){
	synchronized_item_execute_or_post_task_inside_thread_context([&]{
		try{
			set_height_(value);
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

void winp::ui::surface::offset_size(const SIZE &value, const std::function<void(surface &, utility::error_code)> &callback){
	offset_size(value.cx, value.cy, callback);
}

void winp::ui::surface::offset_size(int width, int height, const std::function<void(surface &, utility::error_code)> &callback){
	synchronized_item_execute_or_post_task_inside_thread_context([&]{
		try{
			offset_size_(width, height);
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

void winp::ui::surface::offset_width(int value, const std::function<void(surface &, utility::error_code)> &callback){
	synchronized_item_execute_or_post_task_inside_thread_context([&]{
		try{
			offset_width_(value);
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

void winp::ui::surface::offset_height(int value, const std::function<void(surface &, utility::error_code)> &callback){
	synchronized_item_execute_or_post_task_inside_thread_context([&]{
		try{
			offset_height_(value);
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

const SIZE &winp::ui::surface::get_size(const std::function<void(const SIZE &)> &callback) const{
	return *synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return &synchronized_item_pass_return_ref_value_to_callback(callback, &get_size_());
	}, (callback != nullptr), &size_);
}

int winp::ui::surface::get_width(const std::function<void(int)> &callback) const{
	if (callback == nullptr)
		return get_size(nullptr).cx;

	return get_size([=](const SIZE &value){
		callback(value.cx);
	}).cx;
}

int winp::ui::surface::get_height(const std::function<void(int)> &callback) const{
	if (callback == nullptr)
		return get_size(nullptr).cy;

	return get_size([=](const SIZE &value){
		callback(value.cy);
	}).cy;
}

void winp::ui::surface::set_position(const POINT &value, const std::function<void(surface &, utility::error_code)> &callback){
	set_position(value.x, value.y, callback);
}

void winp::ui::surface::set_position(int x, int y, const std::function<void(surface &, utility::error_code)> &callback){
	synchronized_item_execute_or_post_task_inside_thread_context([&]{
		try{
			set_position_(x, y);
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

void winp::ui::surface::set_x_position(int value, const std::function<void(surface &, utility::error_code)> &callback){
	synchronized_item_execute_or_post_task_inside_thread_context([&]{
		try{
			set_x_position_(value);
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

void winp::ui::surface::set_y_position(int value, const std::function<void(surface &, utility::error_code)> &callback){
	synchronized_item_execute_or_post_task_inside_thread_context([&]{
		try{
			set_y_position_(value);
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

void winp::ui::surface::offset_position(const POINT &value, const std::function<void(surface &, utility::error_code)> &callback){
	offset_position(value.x, value.y, callback);
}

void winp::ui::surface::offset_position(int x, int y, const std::function<void(surface &, utility::error_code)> &callback){
	synchronized_item_execute_or_post_task_inside_thread_context([&]{
		try{
			offset_position_(x, y);
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

void winp::ui::surface::offset_x_position(int value, const std::function<void(surface &, utility::error_code)> &callback){
	synchronized_item_execute_or_post_task_inside_thread_context([&]{
		try{
			offset_x_position_(value);
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

void winp::ui::surface::offset_y_position(int value, const std::function<void(surface &, utility::error_code)> &callback){
	synchronized_item_execute_or_post_task_inside_thread_context([&]{
		try{
			offset_y_position_(value);
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

const POINT &winp::ui::surface::get_position(const std::function<void(const POINT &)> &callback) const{
	return *synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return &synchronized_item_pass_return_ref_value_to_callback(callback, &get_position_());
	}, (callback != nullptr), &position_);
}

int winp::ui::surface::get_x_position(const std::function<void(int)> &callback) const{
	if (callback == nullptr)
		return get_position(nullptr).x;

	return get_position([=](const POINT &value){
		callback(value.x);
	}).x;
}

int winp::ui::surface::get_y_position(const std::function<void(int)> &callback) const{
	if (callback == nullptr)
		return get_position(nullptr).y;

	return get_position([=](const POINT &value){
		callback(value.y);
	}).y;
}

void winp::ui::surface::set_absolute_position(const POINT &value, const std::function<void(surface &, utility::error_code)> &callback){
	set_absolute_position(value.x, value.y, callback);
}

void winp::ui::surface::set_absolute_position(int x, int y, const std::function<void(surface &, utility::error_code)> &callback){
	synchronized_item_execute_or_post_task_inside_thread_context([&]{
		try{
			set_absolute_position_(x, y);
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

POINT winp::ui::surface::get_absolute_position(const std::function<void(const POINT &)> &callback) const{
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return synchronized_item_pass_return_value_to_callback(callback, get_absolute_position_());
	}, (callback != nullptr), POINT{});
}

int winp::ui::surface::get_absolute_x_position(const std::function<void(int)> &callback) const{
	if (callback == nullptr)
		return get_absolute_position(nullptr).x;

	return get_absolute_position([=](const POINT &value){
		callback(value.x);
	}).x;
}

int winp::ui::surface::get_absolute_y_position(const std::function<void(int)> &callback) const{
	if (callback == nullptr)
		return get_absolute_position(nullptr).y;

	return get_absolute_position([=](const POINT &value){
		callback(value.y);
	}).y;
}

void winp::ui::surface::set_dimension(const RECT &value, const std::function<void(surface &, utility::error_code)> &callback){
	set_dimension(value.left, value.top, (value.right - value.left), (value.bottom - value.top), callback);
}

void winp::ui::surface::set_dimension(const POINT &position, const SIZE &size, const std::function<void(surface &, utility::error_code)> &callback){
	set_dimension(position.x, position.y, size.cx, size.cy, callback);
}

void winp::ui::surface::set_dimension(int x, int y, int width, int height, const std::function<void(surface &, utility::error_code)> &callback){
	synchronized_item_execute_or_post_task_inside_thread_context([&]{
		try{
			set_dimension_(x, y, width, height);
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

RECT winp::ui::surface::get_dimension(const std::function<void(const RECT &)> &callback) const{
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return synchronized_item_pass_return_value_to_callback(callback, get_dimension_());
	}, (callback != nullptr), RECT{});
}

RECT winp::ui::surface::get_absolute_dimension(const std::function<void(const RECT &)> &callback) const{
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return synchronized_item_pass_return_value_to_callback(callback, get_absolute_dimension_());
	}, (callback != nullptr), RECT{});
}

POINT winp::ui::surface::convert_position_from_absolute_value(const POINT &value, const std::function<void(const POINT &)> &callback) const{
	return convert_position_from_absolute_value(value.x, value.y, callback);
}

POINT winp::ui::surface::convert_position_from_absolute_value(int x, int y, const std::function<void(const POINT &)> &callback) const{
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return synchronized_item_pass_return_value_to_callback(callback, convert_position_from_absolute_value(x, y));
	}, (callback != nullptr), POINT{});
}

int winp::ui::surface::convert_x_position_from_absolute_value(int value, const std::function<void(int)> &callback) const{
	if (callback == nullptr)
		return convert_position_from_absolute_value(value, 0, nullptr).x;

	return convert_position_from_absolute_value(value, 0, [=](const POINT &value){
		callback(value.x);
	}).x;
}

int winp::ui::surface::convert_y_position_from_absolute_value(int value, const std::function<void(int)> &callback) const{
	if (callback == nullptr)
		return convert_position_from_absolute_value(0, value, nullptr).y;

	return convert_position_from_absolute_value(0, value, [=](const POINT &value){
		callback(value.y);
	}).y;
}

POINT winp::ui::surface::convert_position_to_absolute_value(const POINT &value, const std::function<void(const POINT &)> &callback) const{
	return convert_position_to_absolute_value(value.x, value.y, callback);
}

POINT winp::ui::surface::convert_position_to_absolute_value(int x, int y, const std::function<void(const POINT &)> &callback) const{
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return synchronized_item_pass_return_value_to_callback(callback, convert_position_to_absolute_value(x, y));
	}, (callback != nullptr), POINT{});
}

int winp::ui::surface::convert_x_position_to_absolute_value(int value, const std::function<void(int)> &callback) const{
	if (callback == nullptr)
		return convert_position_to_absolute_value(value, 0, nullptr).x;

	return convert_position_to_absolute_value(value, 0, [=](const POINT &value){
		callback(value.x);
	}).x;
}

int winp::ui::surface::convert_y_position_to_absolute_value(int value, const std::function<void(int)> &callback) const{
	if (callback == nullptr)
		return convert_position_to_absolute_value(0, value, nullptr).y;

	return convert_position_to_absolute_value(0, value, [=](const POINT &value){
		callback(value.y);
	}).y;
}

RECT winp::ui::surface::convert_dimension_from_absolute_value(const RECT &value, const std::function<void(const RECT &)> &callback) const{
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return synchronized_item_pass_return_value_to_callback(callback, convert_dimension_from_absolute_value_(value));
	}, (callback != nullptr), RECT{});
}

RECT winp::ui::surface::convert_dimension_to_absolute_value(const RECT &value, const std::function<void(const RECT &)> &callback) const{
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return synchronized_item_pass_return_value_to_callback(callback, convert_dimension_to_absolute_value_(value));
	}, (callback != nullptr), RECT{});
}

winp::ui::surface::hit_target winp::ui::surface::hit_test(const POINT &value, const std::function<void(hit_target)> &callback) const{
	return hit_test(value.x, value.y, callback);
}

winp::ui::surface::hit_target winp::ui::surface::hit_test(int x, int y, const std::function<void(hit_target)> &callback) const{
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return synchronized_item_pass_return_value_to_callback(callback, hit_test_(x, y));
	}, (callback != nullptr), hit_target::nil);
}

winp::ui::surface::hit_target winp::ui::surface::absolute_hit_test(const POINT &value, const std::function<void(hit_target)> &callback) const{
	return absolute_hit_test(value.x, value.y, callback);
}

winp::ui::surface::hit_target winp::ui::surface::absolute_hit_test(int x, int y, const std::function<void(hit_target)> &callback) const{
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return synchronized_item_pass_return_value_to_callback(callback, absolute_hit_test_(x, y));
	}, (callback != nullptr), hit_target::nil);
}

void winp::ui::surface::set_size_(int width, int height){
	size_.cx = width;
	size_.cy = height;
}

void winp::ui::surface::set_width_(int value){
	set_size_(value, size_.cy);
}

void winp::ui::surface::set_height_(int value){
	set_size_(size_.cx, value);
}

void winp::ui::surface::offset_size_(int width, int height){
	size_.cx += width;
	size_.cy += height;
}

void winp::ui::surface::offset_width_(int value){
	offset_size_(value, 0);
}

void winp::ui::surface::offset_height_(int value){
	offset_size_(0, value);
}

const SIZE &winp::ui::surface::get_size_() const{
	return size_;
}

void winp::ui::surface::set_position_(int x, int y){
	position_.x = x;
	position_.y = y;
}

void winp::ui::surface::set_x_position_(int value){
	set_position_(value, position_.x);
}

void winp::ui::surface::set_y_position_(int value){
	set_position_(position_.y, value);
}

void winp::ui::surface::offset_position_(int x, int y){
	position_.x += x;
	position_.y += y;
}

void winp::ui::surface::offset_x_position_(int value){
	offset_position_(value, 0);
}

void winp::ui::surface::offset_y_position_(int value){
	offset_position_(0, value);
}

const POINT &winp::ui::surface::get_position_() const{
	return position_;
}

void winp::ui::surface::set_absolute_position_(int x, int y){
	auto object_self = dynamic_cast<const object *>(this);
	if (object_self == nullptr)//Position is absolute
		return set_position_(x, y);

	auto surface_parent = dynamic_cast<surface *>(object_self->get_parent());
	if (surface_parent == nullptr)//Position is absolute
		return set_position_(x, y);

	auto parent_absolute_position = surface_parent->get_absolute_position_();
	set_position_((position_.x - parent_absolute_position.x), (position_.y - parent_absolute_position.y));
}

POINT winp::ui::surface::get_absolute_position_() const{
	auto object_self = dynamic_cast<const object *>(this);
	if (object_self == nullptr)//Position is absolute
		return position_;

	auto surface_parent = dynamic_cast<surface *>(object_self->get_parent());
	if (surface_parent == nullptr)//Position is absolute
		return position_;

	auto parent_absolute_position = surface_parent->get_absolute_position_();
	return POINT{ (parent_absolute_position.x + position_.x), (parent_absolute_position.y + position_.y) };
}

void winp::ui::surface::set_dimension_(int x, int y, int width, int height){
	position_.x = x;
	position_.y = y;
	size_.cx = width;
	size_.cy = height;
}

RECT winp::ui::surface::get_dimension_() const{
	return RECT{ position_.x, position_.y, (position_.x + size_.cx), (position_.y + size_.cy) };
}

RECT winp::ui::surface::get_absolute_dimension_() const{
	auto absolute_position = get_absolute_position_();
	return RECT{ absolute_position.x, absolute_position.y, (absolute_position.x + size_.cx), (absolute_position.y + size_.cy) };
}

POINT winp::ui::surface::convert_position_from_absolute_value_(int x, int y) const{
	auto absolute_position = get_absolute_position_();
	return POINT{ (x - absolute_position.x), (y - absolute_position.y) };
}

POINT winp::ui::surface::convert_position_to_absolute_value_(int x, int y) const{
	auto absolute_position = get_absolute_position_();
	return POINT{ (x + absolute_position.x), (y + absolute_position.y) };
}

RECT winp::ui::surface::convert_dimension_from_absolute_value_(const RECT &value) const{
	auto relative_position = convert_position_from_absolute_value_(value.left, value.top);
	return RECT{ relative_position.x, relative_position.y, ((value.right - value.left) + relative_position.x), ((value.bottom - value.top) + relative_position.y) };
}

RECT winp::ui::surface::convert_dimension_to_absolute_value_(const RECT &value) const{
	auto absolute_position = convert_position_to_absolute_value_(value.left, value.top);
	return RECT{ absolute_position.x, absolute_position.y, ((value.right - value.left) + absolute_position.x), ((value.bottom - value.top) + absolute_position.y) };
}

winp::ui::surface::hit_target winp::ui::surface::hit_test_(int x, int y) const{
	return ((0 <= x && 0 <= y && x < size_.cx && y < size_.cy) ? hit_target::inside : hit_target::nil);
}

winp::ui::surface::hit_target winp::ui::surface::absolute_hit_test_(int x, int y) const{
	auto relative_position = convert_position_from_absolute_value_(x, y);
	return hit_test_(relative_position.x, relative_position.y);
}