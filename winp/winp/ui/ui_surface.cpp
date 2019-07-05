#include "../app/app_object.h"

#include "../grid/grid_object.h"

#include "ui_window_surface.h"

winp::ui::surface::~surface() = default;

winp::utility::error_code winp::ui::surface::set_size(const SIZE &value, const std::function<void(surface &, utility::error_code)> &callback){
	return set_size(value.cx, value.cy, callback);
}

winp::utility::error_code winp::ui::surface::set_size(int width, int height, const std::function<void(surface &, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, set_size_(width, height, true));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::surface::set_width(int value, const std::function<void(surface &, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, set_size_(value, size_.cy, true));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::surface::set_height(int value, const std::function<void(surface &, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, set_size_(size_.cx, value, true));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::surface::offset_size(const SIZE &value, const std::function<void(surface &, utility::error_code)> &callback){
	return offset_size(value.cx, value.cy, callback);
}

winp::utility::error_code winp::ui::surface::offset_size(int width, int height, const std::function<void(surface &, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, set_size_((size_.cx + width), (size_.cy + height), true));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::surface::offset_width(int value, const std::function<void(surface &, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, set_size_((size_.cx + value), size_.cy, true));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::surface::offset_height(int value, const std::function<void(surface &, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, set_size_(size_.cx, (size_.cy + value), true));
	}, (callback != nullptr), utility::error_code::nil);
}

const SIZE &winp::ui::surface::get_size(const std::function<void(const SIZE &)> &callback) const{
	return *synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return &synchronized_item_pass_return_ref_value_to_callback(callback, &get_size_());
	}, (callback != nullptr), &size_);
}

const SIZE &winp::ui::surface::get_current_size(const std::function<void(const SIZE &)> &callback) const{
	return *synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return &synchronized_item_pass_return_ref_value_to_callback(callback, &get_current_size_());
	}, (callback != nullptr), &size_);
}

int winp::ui::surface::get_width(const std::function<void(int)> &callback) const{
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return synchronized_item_pass_return_value_to_callback(callback, get_size_().cx);
	}, (callback != nullptr), 0);
}

int winp::ui::surface::get_height(const std::function<void(int)> &callback) const{
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return synchronized_item_pass_return_value_to_callback(callback, get_size_().cy);
	}, (callback != nullptr), 0);
}

SIZE winp::ui::surface::get_client_size(const std::function<void(const SIZE &)> &callback) const{
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return synchronized_item_pass_return_value_to_callback(callback, get_client_size_());
	}, (callback != nullptr), SIZE{});
}

SIZE winp::ui::surface::get_current_client_size(const std::function<void(const SIZE &)> &callback) const{
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return synchronized_item_pass_return_value_to_callback(callback, get_current_client_size_());
	}, (callback != nullptr), SIZE{});
}

int winp::ui::surface::get_client_width(const std::function<void(int)> &callback) const{
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return synchronized_item_pass_return_value_to_callback(callback, get_client_size_().cx);
	}, (callback != nullptr), 0);
}

int winp::ui::surface::get_client_height(const std::function<void(int)> &callback) const{
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return synchronized_item_pass_return_value_to_callback(callback, get_client_size_().cy);
	}, (callback != nullptr), 0);
}

POINT winp::ui::surface::get_client_offset(const std::function<void(const POINT &)> &callback) const{
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return synchronized_item_pass_return_value_to_callback(callback, get_client_offset_());
	}, (callback != nullptr), POINT{});
}

int winp::ui::surface::get_client_x_offset(const std::function<void(int)> &callback) const{
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return synchronized_item_pass_return_value_to_callback(callback, get_client_offset_().x);
	}, (callback != nullptr), 0);
}

int winp::ui::surface::get_client_y_offset(const std::function<void(int)> &callback) const{
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return synchronized_item_pass_return_value_to_callback(callback, get_client_offset_().y);
	}, (callback != nullptr), 0);
}

POINT winp::ui::surface::get_client_start_offset(const std::function<void(const POINT &)> &callback) const{
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return synchronized_item_pass_return_value_to_callback(callback, get_client_start_offset_());
	}, (callback != nullptr), POINT{});
}

winp::utility::error_code winp::ui::surface::set_position(const POINT &value, const std::function<void(surface &, utility::error_code)> &callback){
	return set_position(value.x, value.y, callback);
}

winp::utility::error_code winp::ui::surface::set_position(int x, int y, const std::function<void(surface &, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, set_position_(x, y, true));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::surface::set_x_position(int value, const std::function<void(surface &, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, set_position_(value, position_.y, true));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::surface::set_y_position(int value, const std::function<void(surface &, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, set_position_(position_.x, value, true));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::surface::offset_position(const POINT &value, const std::function<void(surface &, utility::error_code)> &callback){
	return offset_position(value.x, value.y, callback);
}

winp::utility::error_code winp::ui::surface::offset_position(int x, int y, const std::function<void(surface &, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, set_position_((position_.x + x), (position_.y + y), true));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::surface::offset_x_position(int value, const std::function<void(surface &, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, set_position_((position_.x + value), position_.y, true));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::surface::offset_y_position(int value, const std::function<void(surface &, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, set_position_(position_.x, (position_.y + value), true));
	}, (callback != nullptr), utility::error_code::nil);
}

const POINT &winp::ui::surface::get_position(const std::function<void(const POINT &)> &callback) const{
	return *synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return &synchronized_item_pass_return_ref_value_to_callback(callback, &get_position_());
	}, (callback != nullptr), &position_);
}

const POINT &winp::ui::surface::get_current_position(const std::function<void(const POINT &)> &callback) const{
	return *synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return &synchronized_item_pass_return_ref_value_to_callback(callback, &get_current_position_());
	}, (callback != nullptr), &position_);
}

int winp::ui::surface::get_x_position(const std::function<void(int)> &callback) const{
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return synchronized_item_pass_return_value_to_callback(callback, get_position_().x);
	}, (callback != nullptr), 0);
}

int winp::ui::surface::get_y_position(const std::function<void(int)> &callback) const{
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return synchronized_item_pass_return_value_to_callback(callback, get_position_().y);
	}, (callback != nullptr), 0);
}

winp::utility::error_code winp::ui::surface::set_absolute_position(const POINT &value, const std::function<void(surface &, utility::error_code)> &callback){
	return set_absolute_position(value.x, value.y, callback);
}

winp::utility::error_code winp::ui::surface::set_absolute_position(int x, int y, const std::function<void(surface &, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, set_absolute_position_(x, y, true));
	}, (callback != nullptr), utility::error_code::nil);
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

winp::utility::error_code winp::ui::surface::set_dimension(const RECT &value, const std::function<void(surface &, utility::error_code)> &callback){
	return set_dimension(value.left, value.top, (value.right - value.left), (value.bottom - value.top), callback);
}

winp::utility::error_code winp::ui::surface::set_dimension(const POINT &position, const SIZE &size, const std::function<void(surface &, utility::error_code)> &callback){
	return set_dimension(position.x, position.y, size.cx, size.cy, callback);
}

winp::utility::error_code winp::ui::surface::set_dimension(int x, int y, int width, int height, const std::function<void(surface &, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, set_dimension_(x, y, width, height, 0u, true));
	}, (callback != nullptr), utility::error_code::nil);
}

RECT winp::ui::surface::get_dimension(const std::function<void(const RECT &)> &callback) const{
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return synchronized_item_pass_return_value_to_callback(callback, get_dimension_());
	}, (callback != nullptr), RECT{});
}

RECT winp::ui::surface::get_current_dimension(const std::function<void(const RECT &)> &callback) const{
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return synchronized_item_pass_return_value_to_callback(callback, get_current_dimension_());
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

UINT winp::ui::surface::hit_test(const POINT &value, const std::function<void(UINT)> &callback) const{
	return hit_test(value.x, value.y, callback);
}

UINT winp::ui::surface::hit_test(int x, int y, const std::function<void(UINT)> &callback) const{
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return synchronized_item_pass_return_value_to_callback(callback, hit_test_(x, y));
	}, (callback != nullptr), 0u);
}

UINT winp::ui::surface::absolute_hit_test(const POINT &value, const std::function<void(UINT)> &callback) const{
	return absolute_hit_test(value.x, value.y, callback);
}

UINT winp::ui::surface::absolute_hit_test(int x, int y, const std::function<void(UINT)> &callback) const{
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return synchronized_item_pass_return_value_to_callback(callback, absolute_hit_test_(x, y));
	}, (callback != nullptr), 0u);
}

winp::ui::surface::grid_type *winp::ui::surface::get_grid(const std::function<void(grid_type &)> &callback) const{
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		auto grid = get_grid_();
		if (grid != nullptr && callback != nullptr)
			callback(*grid);
		return grid;
	}, (callback != nullptr), nullptr);
}

bool winp::ui::surface::has_grid(const std::function<void(bool)> &callback) const{
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return synchronized_item_pass_return_value_to_callback(callback, (grid_ != nullptr));
	}, (callback != nullptr), false);
}

winp::utility::error_code winp::ui::surface::set_size_(int width, int height, bool allow_animation){
	return set_dimension_(0, 0, width, height, SWP_NOMOVE, allow_animation);
}

const SIZE &winp::ui::surface::get_size_() const{
	return size_;
}

const SIZE &winp::ui::surface::get_current_size_() const{
	auto object_self = dynamic_cast<const object *>(this);
	if (object_self == nullptr)
		return size_;

	auto animation_hk = object_self->find_hook<animation_hook>();
	if (animation_hk == nullptr)
		return size_;

	if (auto size_info = object_self->get_existing_animation_info_<SIZE>(*animation_hk); size_info != nullptr && size_info->is_active)
		return *std::any_cast<SIZE>(&size_info->value);

	return size_;
}

SIZE winp::ui::surface::get_client_size_() const{
	return get_size_();
}

SIZE winp::ui::surface::get_current_client_size_() const{
	return get_current_size_();
}

POINT winp::ui::surface::get_client_offset_() const{
	return POINT{};
}

POINT winp::ui::surface::get_client_start_offset_() const{
	return POINT{};
}

winp::utility::error_code winp::ui::surface::set_position_(int x, int y, bool allow_animation){
	return set_dimension_(x, y, 0, 0, SWP_NOSIZE, allow_animation);
}

const POINT &winp::ui::surface::get_position_() const{
	return position_;
}

const POINT &winp::ui::surface::get_current_position_() const{
	auto object_self = dynamic_cast<const object *>(this);
	if (object_self == nullptr)
		return position_;

	auto animation_hk = object_self->find_hook<animation_hook>();
	if (animation_hk == nullptr)
		return position_;

	if (auto position_info = object_self->get_existing_animation_info_<POINT>(*animation_hk); position_info != nullptr && position_info->is_active)
		return *std::any_cast<POINT>(&position_info->value);

	return position_;
}

winp::utility::error_code winp::ui::surface::set_absolute_position_(int x, int y, bool allow_animation){
	auto &current_position = get_current_position_();
	auto object_self = dynamic_cast<const object *>(this);

	if (object_self == nullptr)//Position is absolute
		return set_position_(x, y, allow_animation);

	auto surface_parent = dynamic_cast<surface *>(object_self->get_parent());
	if (surface_parent == nullptr)//Position is absolute
		return set_position_(x, y, allow_animation);

	auto parent_absolute_position = surface_parent->get_absolute_position_();
	return set_position_((current_position.x - parent_absolute_position.x), (current_position.y - parent_absolute_position.y), allow_animation);
}

POINT winp::ui::surface::get_absolute_position_() const{
	auto &current_position = get_current_position_();
	auto object_self = dynamic_cast<const object *>(this);

	if (object_self == nullptr)//Position is absolute
		return current_position;

	auto surface_parent = dynamic_cast<surface *>(object_self->get_parent());
	if (surface_parent == nullptr)//Position is absolute
		return current_position;

	auto parent_absolute_position = surface_parent->get_absolute_position_();
	auto parent_client_offset = surface_parent->get_client_offset_();

	return POINT{ (parent_absolute_position.x + parent_client_offset.x + current_position.x), (parent_absolute_position.y + parent_client_offset.y + current_position.y) };
}

winp::utility::error_code winp::ui::surface::set_dimension_(int x, int y, int width, int height, UINT flags, bool allow_animation){
	flags &= (SWP_NOMOVE | SWP_NOSIZE);
	if (x == position_.x && y == position_.y)
		flags |= SWP_NOMOVE;

	if (width == size_.cx && height == size_.cy)
		flags |= SWP_NOSIZE;

	if ((flags & (SWP_NOMOVE | SWP_NOSIZE)) == (SWP_NOMOVE | SWP_NOSIZE))
		return utility::error_code::nil;//No changes

	WINDOWPOS info{ nullptr, nullptr, x, y, width, height, flags };
	MSG msg{ nullptr, WM_WINDOWPOSCHANGING, 0, reinterpret_cast<LPARAM>(&info) };

	if ((synchronized_item_trigger_event_<events::position_change>(true, msg, nullptr).first & events::object::state_default_prevented) != 0u)
		return utility::error_code::action_prevented;

	if (info.x == position_.x && info.y == position_.y)
		flags |= SWP_NOMOVE;

	if (info.cx == size_.cx && info.cy == size_.cy)
		flags |= SWP_NOSIZE;

	if ((info.flags & (SWP_NOMOVE | SWP_NOSIZE)) == (SWP_NOMOVE | SWP_NOSIZE))
		return utility::error_code::nil;//No changes

	auto start_position = position_;
	auto start_size = size_;
	RECT previous_dimension{ start_position.x, start_position.y, (start_position.x + start_size.cx), (start_position.y + start_size.cy) };

	if ((info.flags & SWP_NOMOVE) == 0u){
		position_.x = info.x;
		position_.y = info.y;
	}

	if ((info.flags & SWP_NOSIZE) == 0u){
		size_.cx = info.cx;
		size_.cy = info.cy;
	}

	msg.message = WM_WINDOWPOSCHANGED;
	synchronized_item_trigger_event_<events::position_change>(false, msg, nullptr);

	auto object_self = dynamic_cast<object *>(this);
	if (object_self == nullptr)
		return update_dimension_(previous_dimension, position_.x, position_.y, size_.cx, size_.cy, info.flags);

	auto animation_hk = object_self->find_hook_<animation_hook>();
	if (animation_hk == nullptr)//Animation not set
		return update_dimension_(previous_dimension, position_.x, position_.y, size_.cx, size_.cy, info.flags);

	if ((info.flags & SWP_NOMOVE) == 0u){//Cancel animation, if any
		if (auto move_info = object_self->get_existing_animation_info_<POINT>(*animation_hk); move_info != nullptr){
			move_info->is_active = false;
			move_info->value = position_;
			++move_info->count;
		}
	}

	if ((info.flags & SWP_NOSIZE) == 0u){//Cancel animation, if any
		if (auto size_info = object_self->get_existing_animation_info_<SIZE>(*animation_hk); size_info != nullptr){
			size_info->is_active = false;
			size_info->value = size_;
			++size_info->count;
		}
	}

	if (!allow_animation)
		return update_dimension_(previous_dimension, position_.x, position_.y, size_.cx, size_.cy, info.flags);

	if (auto error_code = animate_dimension_(*object_self, *animation_hk, previous_dimension, start_position.x, start_position.y, start_size.cx, start_size.cy, flags); error_code != utility::error_code::nil)
		return error_code;

	return update_dimension_(previous_dimension, position_.x, position_.y, size_.cx, size_.cy, info.flags);
}

winp::utility::error_code winp::ui::surface::animate_dimension_(object &object_self, animation_hook &hk, const RECT &previous_dimension, int x, int y, int width, int height, UINT flags){
	auto dimension_info = object_self.get_existing_animation_info_<RECT>(hk);

	auto move_info = object_self.get_existing_animation_info_<POINT>(hk);
	if (move_info == nullptr){//Create new
		move_info = &object_self.get_animation_info_<POINT>(hk);
		if (dimension_info != nullptr){//Inherit values
			move_info->is_allowed = dimension_info->is_allowed;
			move_info->easing = dimension_info->easing;
			move_info->duration = dimension_info->duration;
		}
	}

	auto size_info = object_self.get_existing_animation_info_<SIZE>(hk);
	if (size_info == nullptr){//Create new
		size_info = &object_self.get_animation_info_<SIZE>(hk);
		if (dimension_info != nullptr){//Inherit values
			size_info->is_allowed = dimension_info->is_allowed;
			size_info->easing = dimension_info->easing;
			size_info->duration = dimension_info->duration;
		}
	}

	auto move_is_allowed = ((flags & SWP_NOMOVE) == 0u && move_info->is_allowed && move_info->easing != nullptr && move_info->duration != std::chrono::microseconds(0));
	auto size_is_allowed = ((flags & SWP_NOSIZE) == 0u && size_info->is_allowed && size_info->easing != nullptr && size_info->duration != std::chrono::microseconds(0));

	auto error_code = utility::error_code::nil;
	auto suppression_hk = object_self.find_hook_<animation_suppression_hook>();

	if (suppression_hk != nullptr){
		auto move_suppressed = ((flags & SWP_NOMOVE) == 0u && (suppression_hk->type_is_suppressed<POINT>() || suppression_hk->type_is_suppressed<RECT>()));
		auto size_suppressed = ((flags & SWP_NOSIZE) == 0u && (suppression_hk->type_is_suppressed<SIZE>() || suppression_hk->type_is_suppressed<RECT>()));

		if ((move_suppressed || !move_is_allowed) && (size_suppressed || !size_is_allowed)){//Position and size animations suppressed
			error_code = update_dimension_(previous_dimension, position_.x, position_.y, size_.cx, size_.cy, flags);
			flags |= (SWP_NOMOVE | SWP_NOSIZE);
		}
		else if ((move_suppressed || !move_is_allowed)){//Position animation suppressed
			error_code = update_dimension_(previous_dimension, position_.x, position_.y, size_.cx, size_.cy, (flags & ~SWP_NOSIZE));
			flags |= SWP_NOMOVE;
		}
		else if ((size_suppressed || !size_is_allowed)){//Size animation suppressed
			error_code = update_dimension_(previous_dimension, position_.x, position_.y, size_.cx, size_.cy, (flags & ~SWP_NOMOVE));
			flags |= SWP_NOSIZE;
		}

		if ((move_suppressed || size_suppressed) && suppression_hk->is_once())
			object_self.remove_hook_<animation_suppression_hook>();//Remove hook
	}

	if ((flags & SWP_NOMOVE) != 0u)
		move_info->is_active = false;//Position animation suppressed

	if ((flags & SWP_NOSIZE) != 0u)
		size_info->is_active = false;//Size animation suppressed

	if ((flags & (SWP_NOMOVE | SWP_NOSIZE)) == (SWP_NOMOVE | SWP_NOSIZE))//Position and size animations suppressed
		return ((error_code == utility::error_code::nil) ? utility::error_code::animation_suppressed : error_code);

	if ((flags & SWP_NOMOVE) == 0u){//Animate position
		synchronized_item_trigger_event_<events::animation>(thread::item::event_manager_type::get_key<POINT>(), events::animation::progress_type::begin);

		move_info->is_active = true;
		move_info->value = POINT{ x, y };

		POINT position_delta{ (position_.x - x), (position_.y - y) }, last_position = POINT{ x, y };
		object_self.thread_.animate(move_info->easing, [count = ++move_info->count, object_self_ref = &object_self, this, x, y, position_delta, last_position](float progress, bool has_more) mutable{
			auto &current_size = get_current_size_();
			RECT previous_dimension{ last_position.x, last_position.y, (last_position.x + current_size.cx), (last_position.y + current_size.cy) };

			auto animation_hk = object_self_ref->find_hook<animation_hook>();
			if (animation_hk == nullptr){
				update_dimension_(previous_dimension, position_.x, position_.y, 0, 0, SWP_NOSIZE);
				return false;
			}

			auto move_info = object_self_ref->get_existing_animation_info_<POINT>(*animation_hk);
			if (move_info == nullptr){
				update_dimension_(previous_dimension, position_.x, position_.y, 0, 0, SWP_NOSIZE);
				return false;
			}

			if (move_info->is_active && move_info->count == count){//Step
				POINT position{ (x + static_cast<int>(position_delta.x * progress)), (y + static_cast<int>(position_delta.y * progress)) };

				if (position.x != last_position.x || position.y != last_position.y){
					move_info->value = position;
					update_dimension_(previous_dimension, position.x, position.y, 0, 0, SWP_NOSIZE);
				}

				last_position = position;
				synchronized_item_trigger_event_<events::animation>(thread::item::event_manager_type::get_key<POINT>(), events::animation::progress_type::step);

				if (!has_more){//Ended
					move_info->is_active = false;
					synchronized_item_trigger_event_<events::animation>(thread::item::event_manager_type::get_key<POINT>(), events::animation::progress_type::end);
				}
			}
			else if (move_info->count == count){//Not active
				move_info->value = POINT{ x, y };
				update_dimension_(previous_dimension, x, y, 0, 0, SWP_NOSIZE);
				synchronized_item_trigger_event_<events::animation>(thread::item::event_manager_type::get_key<POINT>(), events::animation::progress_type::cancel);
			}
			else//Canceled
				synchronized_item_trigger_event_<events::animation>(thread::item::event_manager_type::get_key<POINT>(), events::animation::progress_type::cancel);

			return (move_info->is_active && move_info->count == count);
		}, move_info->duration);
	}

	if ((flags & SWP_NOSIZE) == 0u){//Animate size
		synchronized_item_trigger_event_<events::animation>(thread::item::event_manager_type::get_key<SIZE>(), events::animation::progress_type::begin);

		size_info->is_active = true;
		size_info->value = SIZE{ width, height };

		SIZE size_delta{ (size_.cx - width), (size_.cy - height) }, last_size = SIZE{ width, height };
		object_self.thread_.animate(size_info->easing, [count = ++size_info->count, object_self_ref = &object_self, this, width, height, size_delta, last_size](float progress, bool has_more) mutable{
			auto &current_position = get_current_position_();
			RECT previous_dimension{ current_position.x, current_position.y, (current_position.x + last_size.cx), (current_position.y + last_size.cy) };

			auto animation_hk = object_self_ref->find_hook<animation_hook>();
			if (animation_hk == nullptr){
				update_dimension_(previous_dimension, 0, 0, size_.cx, size_.cy, SWP_NOMOVE);
				return false;
			}

			auto size_info = object_self_ref->get_existing_animation_info_<SIZE>(*animation_hk);
			if (size_info == nullptr){
				update_dimension_(previous_dimension, 0, 0, size_.cx, size_.cy, SWP_NOMOVE);
				return false;
			}

			if (size_info->is_active && size_info->count == count){//Step
				SIZE size{ (width + static_cast<int>(size_delta.cx * progress)), (height + static_cast<int>(size_delta.cy * progress)) };

				if (size.cx != last_size.cx || size.cy != last_size.cy){
					size_info->value = size;
					update_dimension_(previous_dimension, 0, 0, size.cx, size.cy, SWP_NOMOVE);
				}

				last_size = size;
				synchronized_item_trigger_event_<events::animation>(thread::item::event_manager_type::get_key<SIZE>(), events::animation::progress_type::step);

				if (!has_more){//Ended
					size_info->is_active = false;
					synchronized_item_trigger_event_<events::animation>(thread::item::event_manager_type::get_key<SIZE>(), events::animation::progress_type::end);
				}
			}
			else if (size_info->count == count){//Not active
				size_info->value = SIZE{ width, height };
				update_dimension_(previous_dimension, 0, 0, width, height, SWP_NOMOVE);
				synchronized_item_trigger_event_<events::animation>(thread::item::event_manager_type::get_key<SIZE>(), events::animation::progress_type::cancel);
			}
			else//Canceled
				synchronized_item_trigger_event_<events::animation>(thread::item::event_manager_type::get_key<SIZE>(), events::animation::progress_type::cancel);

			return (size_info->is_active && size_info->count == count);
		}, size_info->duration);
	}

	return utility::error_code::animation_started;
}

winp::utility::error_code winp::ui::surface::update_dimension_(const RECT &previous_dimension, int x, int y, int width, int height, UINT flags){
	if (auto object_self = dynamic_cast<ui::object *>(this); object_self != nullptr){
		object_self->trigger_event_<events::position_updated>(flags);
		if (!object_self->has_hook_<ui::no_drag_position_updated_hook>())
			object_self->trigger_event_<events::non_drag_position_updated>(flags);
	}

	if (auto tree_self = dynamic_cast<tree *>(this); tree_self != nullptr && (flags & SWP_NOMOVE) == 0u){
		tree_self->traverse_all_children_of<window_surface>([](window_surface &child){
			child.update_position_();
		}, true);
	}

	return utility::error_code::nil;
}

RECT winp::ui::surface::get_dimension_() const{
	return RECT{ position_.x, position_.y, (position_.x + size_.cx), (position_.y + size_.cy) };
}

RECT winp::ui::surface::get_current_dimension_() const{
	auto &current_position = get_current_position_();
	auto &current_size = get_current_size_();

	return RECT{ current_position.x, current_position.y, (current_position.x + current_size.cx), (current_position.y + current_size.cy) };
}

RECT winp::ui::surface::get_absolute_dimension_() const{
	auto absolute_position = get_absolute_position_();
	return RECT{ absolute_position.x, absolute_position.y, (absolute_position.x + size_.cx), (absolute_position.y + size_.cy) };
}

POINT winp::ui::surface::convert_position_from_absolute_value_(int x, int y) const{
	auto &current_position = get_current_position_();
	auto object_self = dynamic_cast<const object *>(this);

	if (object_self == nullptr)//Position is absolute
		return POINT{ (x - current_position.x), (y - current_position.y) };

	auto surface_parent = dynamic_cast<surface *>(object_self->get_parent());
	if (surface_parent == nullptr)//Position is absolute
		return POINT{ (x - current_position.x), (y - current_position.y) };

	auto absolute_position = get_absolute_position_();
	auto client_offset = surface_parent->get_client_start_offset_();

	return POINT{ (x - (absolute_position.x + client_offset.x)), (y - (absolute_position.y + client_offset.y)) };
}

POINT winp::ui::surface::convert_position_to_absolute_value_(int x, int y) const{
	auto &current_position = get_current_position_();
	auto object_self = dynamic_cast<const object *>(this);

	if (object_self == nullptr)//Position is absolute
		return POINT{ (x + current_position.x), (y + current_position.y) };

	auto surface_parent = dynamic_cast<surface *>(object_self->get_parent());
	if (surface_parent == nullptr)//Position is absolute
		return POINT{ (x + current_position.x), (y + current_position.y) };

	auto absolute_position = get_absolute_position_();
	auto client_offset = surface_parent->get_client_offset_();

	return POINT{ (x + absolute_position.x + client_offset.x), (y + absolute_position.y + client_offset.y) };
}

RECT winp::ui::surface::convert_dimension_from_absolute_value_(const RECT &value) const{
	auto relative_position = convert_position_from_absolute_value_(value.left, value.top);
	return RECT{ relative_position.x, relative_position.y, ((value.right - value.left) + relative_position.x), ((value.bottom - value.top) + relative_position.y) };
}

RECT winp::ui::surface::convert_dimension_to_absolute_value_(const RECT &value) const{
	auto absolute_position = convert_position_to_absolute_value_(value.left, value.top);
	return RECT{ absolute_position.x, absolute_position.y, ((value.right - value.left) + absolute_position.x), ((value.bottom - value.top) + absolute_position.y) };
}

UINT winp::ui::surface::hit_test_(int x, int y) const{
	return ((0 <= x && 0 <= y && x < size_.cx && y < size_.cy) ? HTCLIENT : HTNOWHERE);
}

UINT winp::ui::surface::absolute_hit_test_(int x, int y) const{
	auto relative_position = convert_position_from_absolute_value_(x, y);
	return hit_test_(relative_position.x, relative_position.y);
}

winp::ui::surface::grid_type *winp::ui::surface::get_grid_() const{
	if (grid_ == nullptr){
		grid_ = std::make_shared<grid_type>(*dynamic_cast<tree *>(const_cast<surface *>(this)));
		grid_->insert_hook<parent_fill_hook>();
		grid_->insert_hook<placement_hook>(placement_hook::alignment_type::top_left);
		grid_->create();
	}

	return grid_.get();
}
