#include "../app/app_collection.h"

#include "ui_visible_surface.h"

winp::ui::visible_surface::visible_surface(tree *tree_self)
	: surface(tree_self){}

winp::ui::visible_surface::~visible_surface() = default;

winp::utility::error_code winp::ui::visible_surface::redraw(const std::function<void(visible_surface, utility::error_code)> &callback) const{
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, redraw_());
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::visible_surface::redraw(const RECT &region, const std::function<void(visible_surface, utility::error_code)> &callback) const{
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, redraw_(region));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::visible_surface::show(const std::function<void(visible_surface, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, show_());
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::visible_surface::hide(const std::function<void(visible_surface, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, hide_());
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::visible_surface::set_visibility(bool is_visible, const std::function<void(visible_surface, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, set_visibility_(is_visible, true));
	}, (callback != nullptr), utility::error_code::nil);
}

bool winp::ui::visible_surface::is_visible(const std::function<void(bool)> &callback) const{
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return synchronized_item_pass_return_value_to_callback(callback, is_visible_());
	}, (callback != nullptr), false);
}

winp::utility::error_code winp::ui::visible_surface::set_background_transparency(bool is_transparent, const std::function<void(visible_surface, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, set_background_transparency_(is_transparent));
	}, (callback != nullptr), utility::error_code::nil);
}

bool winp::ui::visible_surface::is_transparent_background(const std::function<void(bool)> &callback) const{
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return synchronized_item_pass_return_value_to_callback(callback, is_transparent_background_());
	}, (callback != nullptr), false);
}

winp::utility::error_code winp::ui::visible_surface::set_background_color(const D2D1::ColorF &value, const std::function<void(visible_surface, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, set_background_color_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::visible_surface::animate_background_color(const D2D1::ColorF &value, const easing_type &easing, const std::chrono::microseconds &duration, const std::function<void(surface &, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, animate_background_color_(value, easing, duration, callback));
	}, (callback != nullptr), utility::error_code::nil);
}

const D2D1::ColorF &winp::ui::visible_surface::get_background_color(const std::function<void(const D2D1::ColorF &)> &callback) const{
	return *synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return &synchronized_item_pass_return_ref_value_to_callback(callback, &get_background_color_());
	}, (callback != nullptr), &background_color_);
}

COLORREF winp::ui::visible_surface::convert_rgb_to_rgba(COLORREF rgb, BYTE a){
	return (RGB(GetRValue(rgb), GetGValue(rgb), GetBValue(rgb)) | (static_cast<COLORREF>(a) << 24));
}

COLORREF winp::ui::visible_surface::convert_colorf_to_colorref(const D2D1::ColorF &value){
	return (RGB(static_cast<BYTE>(value.r * 255.0f), static_cast<BYTE>(value.g * 255.0f), static_cast<BYTE>(value.b * 255.0f)) | (static_cast<COLORREF>(value.a * 255.0f) << 24));
}

D2D1::ColorF winp::ui::visible_surface::convert_colorref_to_colorf(COLORREF value){
	return D2D1::ColorF(
		(GetRValue(value) / 255.0f),
		(GetGValue(value) / 255.0f),
		(GetBValue(value) / 255.0f),
		((LOBYTE((value) >> 24)) / 255.0f)
	);
}

D2D1::ColorF winp::ui::visible_surface::convert_colorref_to_colorf(COLORREF value, BYTE a){
	return D2D1::ColorF(
		(GetRValue(value) / 255.0f),
		(GetGValue(value) / 255.0f),
		(GetBValue(value) / 255.0f),
		(a / 255.0f)
	);
}

bool winp::ui::visible_surface::compare_colors(const D2D1::ColorF &first, const D2D1::ColorF &second){
	return (convert_colorf_to_colorref(first) == convert_colorf_to_colorref(second));
}

winp::utility::error_code winp::ui::visible_surface::position_change_(bool is_changing){
	if (auto error_code = surface::position_change_(is_changing); error_code != utility::error_code::nil)
		return error_code;

	if (is_changing && is_visible_()){
		hide_();
		set_visibility_(true, false);
	}
	else if (!is_changing)
		redraw_();

	return utility::error_code::nil;
}

winp::utility::error_code winp::ui::visible_surface::size_change_(bool is_changing){
	if (auto error_code = surface::size_change_(is_changing); error_code != utility::error_code::nil)
		return error_code;

	if (is_changing && is_visible_()){
		hide_();
		set_visibility_(true, false);
	}
	else if (!is_changing)
		redraw_();

	return utility::error_code::nil;
}

winp::utility::error_code winp::ui::visible_surface::redraw_() const{
	return utility::error_code::not_supported;
}

winp::utility::error_code winp::ui::visible_surface::redraw_(const RECT &region) const{
	return utility::error_code::not_supported;
}

winp::utility::error_code winp::ui::visible_surface::set_visibility_(bool is_visible, bool redraw){
	if (redraw)
		return (is_visible ? show_() : hide_());
	return utility::error_code::nil;
}

winp::utility::error_code winp::ui::visible_surface::show_(){
	return utility::error_code::not_supported;
}

winp::utility::error_code winp::ui::visible_surface::hide_(){
	return utility::error_code::not_supported;
}

bool winp::ui::visible_surface::is_visible_() const{
	return false;
}

winp::utility::error_code winp::ui::visible_surface::set_background_transparency_(bool is_transparent){
	return utility::error_code::not_supported;
}

bool winp::ui::visible_surface::is_transparent_background_() const{
	return false;
}

winp::utility::error_code winp::ui::visible_surface::set_background_color_(const D2D1::ColorF &value){
	if ((synchronized_item_trigger_event_<events::background_color_change>(value, true).first & events::object::state_default_prevented) != 0u)
		return utility::error_code::action_prevented;

	background_color_ = value;
	synchronized_item_trigger_event_<events::background_color_change>(value, false);
	redraw_();

	return utility::error_code::nil;
}

winp::utility::error_code winp::ui::visible_surface::animate_background_color_(const D2D1::ColorF &value, const easing_type &easing, const std::chrono::microseconds &duration, const std::function<void(surface &, utility::error_code)> &callback){
	if (auto item_self = dynamic_cast<thread::item *>(this); item_self != nullptr){
		auto state = ++background_color_animation_state_;
		D2D1::ColorF start_color = get_background_color_(), color_offset{ (value.r - start_color.r), (value.g - start_color.g), (value.b - start_color.b) };

		item_self->get_thread().animate(easing, [=](float progress, bool has_more){
			if (background_color_animation_state_ != state){
				if (callback != nullptr)
					callback(*this, utility::error_code::animation_canceled);
				return false;
			}

			set_background_color_(D2D1::ColorF((start_color.r + (color_offset.r * progress)), (start_color.g + (color_offset.g * progress)), (start_color.b + (color_offset.b * progress))));
			if (!has_more && callback != nullptr)
				callback(*this, utility::error_code::animation_ended);

			return true;
		}, duration);

		return utility::error_code::animation_started;
	}

	return utility::error_code::action_could_not_be_completed;
}

const D2D1::ColorF &winp::ui::visible_surface::get_background_color_() const{
	return background_color_;
}
