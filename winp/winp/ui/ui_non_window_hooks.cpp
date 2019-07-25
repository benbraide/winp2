#include "ui_non_window_surface.h"
#include "ui_non_window_hooks.h"

winp::ui::non_window_shape_hook::non_window_shape_hook(object &target)
	: base_type(target){}

winp::ui::non_window_shape_hook::~non_window_shape_hook() = default;

bool winp::ui::non_window_shape_hook::setup_(){
	if (!base_type::setup_())
		return false;

	target_.remove_similar_hooks<non_window_shape_hook>();
	return true;
}

winp::ui::rectangular_non_window_hook::rectangular_non_window_hook(object &target)
	: non_window_shape_hook(target){
	if (typed_target_ != nullptr){
		target_.events().bind([this](events::create_non_window_handle &e){
			auto &current_size = e.get_current_size();
			if (auto handle = e.get_handle(); handle != nullptr)
				return ((SetRectRgn(handle, 0, 0, current_size.cx, current_size.cy) == FALSE) ? nullptr : handle);

			return CreateRectRgn(0, 0, current_size.cx, current_size.cy);
		}, this);
	}
}

winp::ui::rectangular_non_window_hook::~rectangular_non_window_hook() = default;

winp::ui::round_rectangular_non_window_hook::round_rectangular_non_window_hook(object &target)
	: non_window_shape_hook(target){
	if (typed_target_ != nullptr){
		target_.events().bind([this](events::create_non_window_handle &e){
			auto &current_size = e.get_current_size();
			return CreateRoundRectRgn(0, 0, current_size.cx, current_size.cy, border_curve_size_.cx, border_curve_size_.cy);
		}, this);
	}
}

winp::ui::round_rectangular_non_window_hook::round_rectangular_non_window_hook(object &target, const SIZE &border_curve_size)
	: round_rectangular_non_window_hook(target, border_curve_size.cx, border_curve_size.cy){}

winp::ui::round_rectangular_non_window_hook::round_rectangular_non_window_hook(object &target, int border_curve_width, int border_curve_height)
	: round_rectangular_non_window_hook(target){
	border_curve_size_.cx = border_curve_width;
	border_curve_size_.cy = border_curve_height;
}

winp::ui::round_rectangular_non_window_hook::~round_rectangular_non_window_hook() = default;

winp::utility::error_code winp::ui::round_rectangular_non_window_hook::set_border_curve_size(const SIZE &value, const std::function<void(round_rectangular_non_window_hook &, utility::error_code)> &callback){
	return set_border_curve_size(value.cx, value.cy, callback);
}

winp::utility::error_code winp::ui::round_rectangular_non_window_hook::set_border_curve_size(int width, int height, const std::function<void(round_rectangular_non_window_hook &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_border_curve_size_(width, height));
	}, (callback != nullptr), utility::error_code::nil);
}

const SIZE &winp::ui::round_rectangular_non_window_hook::get_border_curve_size(const std::function<void(const SIZE &)> &callback) const{
	return *compute_or_post_task_inside_thread_context([=]{
		return &pass_return_ref_value_to_callback(callback, &border_curve_size_);
	}, (callback != nullptr), &border_curve_size_);
}

winp::utility::error_code winp::ui::round_rectangular_non_window_hook::set_border_curve_size_(int width, int height){
	if (width == border_curve_size_.cx && height == border_curve_size_.cy)
		return utility::error_code::nil;//No changes

	border_curve_size_.cx = width;
	border_curve_size_.cy = height;

	typed_target_->update_handle();
	typed_target_->redraw();

	return utility::error_code::nil;
}

winp::ui::elliptical_non_window_hook::elliptical_non_window_hook(object &target)
	: non_window_shape_hook(target){
	if (typed_target_ != nullptr){
		target_.events().bind([this](events::create_non_window_handle &e){
			auto &current_size = e.get_current_size();
			return CreateEllipticRgn(0, 0, current_size.cx, current_size.cy);
		}, this);
	}
}

winp::ui::elliptical_non_window_hook::~elliptical_non_window_hook() = default;

winp::ui::non_window_non_client_hook::non_window_non_client_hook(object &target)
	: base_type(target){
	if (typed_target_ != nullptr){
		target_.events().bind([this](events::create_non_window_handle &e){
			auto &current_size = e.get_current_size();
			return CreateEllipticRgn(0, 0, current_size.cx, current_size.cy);
		}, this);
	}
}

winp::ui::non_window_non_client_hook::~non_window_non_client_hook() = default;
