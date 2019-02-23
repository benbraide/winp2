#include "ui_visible_surface.h"

winp::ui::visible_surface::~visible_surface() = default;

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
	return (is_visible ? show(callback) : hide(callback));
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
	background_color_ = value;
	return utility::error_code::nil;
}

const D2D1::ColorF &winp::ui::visible_surface::get_background_color_() const{
	return background_color_;
}
