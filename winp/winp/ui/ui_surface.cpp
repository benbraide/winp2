#include "../app/app_object.h"

#include "../grid/grid_object.h"

winp::ui::surface::~surface() = default;

winp::utility::error_code winp::ui::surface::set_size(const SIZE &value, const std::function<void(surface &, utility::error_code)> &callback){
	return set_size(value.cx, value.cy, callback);
}

winp::utility::error_code winp::ui::surface::set_size(int width, int height, const std::function<void(surface &, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, set_size_(width, height));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::surface::animate_size(int width, int height, const easing_type &easing, const std::chrono::microseconds &duration, const std::function<void(surface &, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, animate_size_(width, height, easing, duration, callback));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::surface::set_width(int value, const std::function<void(surface &, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, set_size_(value, size_.cy));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::surface::animate_width(int value, const easing_type &easing, const std::chrono::microseconds &duration, const std::function<void(surface &, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, animate_size_(value, size_.cy, easing, duration, callback));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::surface::set_height(int value, const std::function<void(surface &, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, set_size_(size_.cx, value));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::surface::animate_height(int value, const easing_type &easing, const std::chrono::microseconds &duration, const std::function<void(surface &, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, animate_size_(size_.cx, value, easing, duration, callback));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::surface::offset_size(const SIZE &value, const std::function<void(surface &, utility::error_code)> &callback){
	return offset_size(value.cx, value.cy, callback);
}

winp::utility::error_code winp::ui::surface::offset_size(int width, int height, const std::function<void(surface &, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, set_size_((size_.cx + width), (size_.cy + height)));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::surface::offset_width(int value, const std::function<void(surface &, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, set_size_((size_.cx + value), size_.cy));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::surface::offset_height(int value, const std::function<void(surface &, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, set_size_(size_.cx, (size_.cy + value)));
	}, (callback != nullptr), utility::error_code::nil);
}

const SIZE &winp::ui::surface::get_size(const std::function<void(const SIZE &)> &callback) const{
	return *synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return &synchronized_item_pass_return_ref_value_to_callback(callback, &get_size_());
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
		return thread::item::pass_return_value_to_callback(callback, *this, set_position_(x, y));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::surface::animate_position(int x, int y, const easing_type &easing, const std::chrono::microseconds &duration, const std::function<void(surface &, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, animate_position_(x, y, easing, duration, callback));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::surface::set_x_position(int value, const std::function<void(surface &, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, set_position_(value, position_.y));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::surface::animate_x_position(int value, const easing_type &easing, const std::chrono::microseconds &duration, const std::function<void(surface &, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, animate_position_(value, position_.y, easing, duration, callback));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::surface::set_y_position(int value, const std::function<void(surface &, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, set_position_(position_.x, value));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::surface::animate_y_position(int value, const easing_type &easing, const std::chrono::microseconds &duration, const std::function<void(surface &, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, animate_position_(position_.x, value, easing, duration, callback));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::surface::offset_position(const POINT &value, const std::function<void(surface &, utility::error_code)> &callback){
	return offset_position(value.x, value.y, callback);
}

winp::utility::error_code winp::ui::surface::offset_position(int x, int y, const std::function<void(surface &, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, set_position_((position_.x + x), (position_.y + y)));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::surface::offset_x_position(int value, const std::function<void(surface &, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, set_position_((position_.x + value), position_.y));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::surface::offset_y_position(int value, const std::function<void(surface &, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, set_position_(position_.x, position_.x + value));
	}, (callback != nullptr), utility::error_code::nil);
}

const POINT &winp::ui::surface::get_position(const std::function<void(const POINT &)> &callback) const{
	return *synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return &synchronized_item_pass_return_ref_value_to_callback(callback, &get_position_());
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
		return thread::item::pass_return_value_to_callback(callback, *this, set_absolute_position_(x, y));
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
		return thread::item::pass_return_value_to_callback(callback, *this, set_dimension_(x, y, width, height));
	}, (callback != nullptr), utility::error_code::nil);
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

winp::utility::error_code winp::ui::surface::set_size_(int width, int height){
	return set_dimension_(position_.x, position_.y, width, height);
}

winp::utility::error_code winp::ui::surface::animate_size_(int width, int height, const easing_type &easing, const std::chrono::microseconds &duration, const std::function<void(surface &, utility::error_code)> &callback){
	if (auto item_self = dynamic_cast<thread::item *>(this); item_self != nullptr){
		auto state = ++size_animation_state_;
		SIZE start_size = get_size_(), size_offset{ (width - start_size.cx), (height - start_size.cy) };

		item_self->get_thread().animate(easing, [=](float progress, bool has_more){
			if (size_animation_state_ != state){
				if (callback != nullptr)
					callback(*this, utility::error_code::animation_canceled);
				return false;
			}

			set_size_((start_size.cx + static_cast<int>(size_offset.cx * progress)), (start_size.cy + static_cast<int>(size_offset.cy * progress)));
			if (!has_more && callback != nullptr)
				callback(*this, utility::error_code::animation_ended);

			return true;
		}, duration);

		return utility::error_code::animation_started;
	}

	return utility::error_code::action_could_not_be_completed;
}

const SIZE &winp::ui::surface::get_size_() const{
	return size_;
}

SIZE winp::ui::surface::get_client_size_() const{
	return get_size_();
}

POINT winp::ui::surface::get_client_offset_() const{
	return POINT{};
}

POINT winp::ui::surface::get_client_start_offset_() const{
	return POINT{};
}

winp::utility::error_code winp::ui::surface::set_position_(int x, int y){
	return set_dimension_(x, y, size_.cx, size_.cy);
}

winp::utility::error_code winp::ui::surface::animate_position_(int x, int y, const easing_type &easing, const std::chrono::microseconds &duration, const std::function<void(surface &, utility::error_code)> &callback){
	if (auto item_self = dynamic_cast<thread::item *>(this); item_self != nullptr){
		auto state = ++position_animation_state_;
		POINT start_position = get_position_(), position_offset{ (x - start_position.x), (y - start_position.y) };

		item_self->get_thread().animate(easing, [=](float progress, bool has_more){
			if (position_animation_state_ != state){
				if (callback != nullptr)
					callback(*this, utility::error_code::animation_canceled);
				return false;
			}

			set_position_((start_position.x + static_cast<int>(position_offset.x * progress)), (start_position.y + static_cast<int>(position_offset.y * progress)));
			if (!has_more && callback != nullptr)
				callback(*this, utility::error_code::animation_ended);

			return true;
		}, duration);

		return utility::error_code::animation_started;
	}

	return utility::error_code::action_could_not_be_completed;
}

const POINT &winp::ui::surface::get_position_() const{
	return position_;
}

winp::utility::error_code winp::ui::surface::set_absolute_position_(int x, int y){
	auto object_self = dynamic_cast<const object *>(this);
	if (object_self == nullptr)//Position is absolute
		return set_position_(x, y);

	auto surface_parent = dynamic_cast<surface *>(object_self->get_parent());
	if (surface_parent == nullptr)//Position is absolute
		return set_position_(x, y);

	auto parent_absolute_position = surface_parent->get_absolute_position_();
	return set_position_((position_.x - parent_absolute_position.x), (position_.y - parent_absolute_position.y));
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

winp::utility::error_code winp::ui::surface::set_dimension_(int x, int y, int width, int height){
	UINT flags = (SWP_NOMOVE | SWP_NOSIZE);
	if (x != position_.x || y != position_.y)
		flags &= ~SWP_NOMOVE;

	if (width != size_.cx || height != size_.cy)
		flags &= ~SWP_NOSIZE;

	if (flags == (SWP_NOMOVE | SWP_NOSIZE))
		return utility::error_code::nil;//No changes

	WINDOWPOS info{ nullptr, nullptr, x, y, width, height, flags };
	auto item_self = dynamic_cast<thread::item *>(this);

	item_self->get_thread().send_message(*item_self, WM_WINDOWPOSCHANGING, 0, reinterpret_cast<LPARAM>(&info));
	if ((flags & (SWP_NOMOVE | SWP_NOSIZE)) == (SWP_NOMOVE | SWP_NOSIZE))
		return utility::error_code::nil;//No changes

	if ((flags & SWP_NOMOVE) == 0u){
		if (auto error_code = position_change_(true); error_code != utility::error_code::nil)
			return error_code;

		position_.x = info.x;
		position_.y = info.y;

		position_change_(false);
	}

	if ((flags & SWP_NOSIZE) == 0u){
		if (auto error_code = size_change_(true); error_code != utility::error_code::nil)
			return error_code;

		size_.cx = info.cx;
		size_.cy = info.cy;

		size_change_(false);
	}
	
	item_self->get_thread().send_message(*item_self, WM_WINDOWPOSCHANGED, 0, reinterpret_cast<LPARAM>(&info));
	return utility::error_code::nil;
}

winp::utility::error_code winp::ui::surface::position_change_(bool is_changing){
	if (is_changing)
		return utility::error_code::nil;

	if (auto tree_self = dynamic_cast<tree *>(this); tree_self != nullptr){
		tree_self->traverse_all_children_of<surface>([](surface &child){
			auto &child_position = child.get_position_();
			child.set_position_(child_position.x, child_position.y);
		}, true);
	}

	return utility::error_code::nil;
}

winp::utility::error_code winp::ui::surface::size_change_(bool is_changing){
	return utility::error_code::nil;
}

RECT winp::ui::surface::get_dimension_() const{
	return RECT{ position_.x, position_.y, (position_.x + size_.cx), (position_.y + size_.cy) };
}

RECT winp::ui::surface::get_absolute_dimension_() const{
	auto absolute_position = get_absolute_position_();
	return RECT{ absolute_position.x, absolute_position.y, (absolute_position.x + size_.cx), (absolute_position.y + size_.cy) };
}

POINT winp::ui::surface::convert_position_from_absolute_value_(int x, int y) const{
	auto object_self = dynamic_cast<const object *>(this);
	if (object_self == nullptr)//Position is absolute
		return POINT{ (x - position_.x), (y - position_.y) };

	auto surface_parent = dynamic_cast<surface *>(object_self->get_parent());
	if (surface_parent == nullptr)//Position is absolute
		return POINT{ (x - position_.x), (y - position_.y) };

	auto absolute_position = get_absolute_position_();
	auto client_offset = surface_parent->get_client_offset_();

	return POINT{ (x - (absolute_position.x + client_offset.x)), (y - (absolute_position.y + client_offset.y)) };
}

POINT winp::ui::surface::convert_position_to_absolute_value_(int x, int y) const{
	auto object_self = dynamic_cast<const object *>(this);
	if (object_self == nullptr)//Position is absolute
		return POINT{ (x + position_.x), (y + position_.y) };

	auto surface_parent = dynamic_cast<surface *>(object_self->get_parent());
	if (surface_parent == nullptr)//Position is absolute
		return POINT{ (x + position_.x), (y + position_.y) };

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
