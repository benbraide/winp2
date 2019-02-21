#include "ui_visible_surface.h"

winp::ui::visible_surface::~visible_surface() = default;

void winp::ui::visible_surface::show(const std::function<void(visible_surface, utility::error_code)> &callback){
	synchronized_item_execute_or_post_task_inside_thread_context([&]{
		try{
			show_();
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

void winp::ui::visible_surface::hide(const std::function<void(visible_surface, utility::error_code)> &callback){
	synchronized_item_execute_or_post_task_inside_thread_context([&]{
		try{
			hide_();
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

void winp::ui::visible_surface::set_visibility(bool is_visible, const std::function<void(visible_surface, utility::error_code)> &callback){
	if (is_visible)
		show(callback);
	else
		hide(callback);
}

bool winp::ui::visible_surface::is_visible(const std::function<void(bool)> &callback) const{
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return synchronized_item_pass_return_value_to_callback(callback, is_visible_());
	}, (callback != nullptr), false);
}

void winp::ui::visible_surface::set_background_transparency(bool is_transparent, const std::function<void(visible_surface, utility::error_code)> &callback){
	synchronized_item_execute_or_post_task_inside_thread_context([&]{
		try{
			set_background_transparency_(is_transparent);
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

bool winp::ui::visible_surface::is_transparent_background(const std::function<void(bool)> &callback) const{
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return synchronized_item_pass_return_value_to_callback(callback, is_transparent_background_());
	}, (callback != nullptr), false);
}

void winp::ui::visible_surface::set_background_color(const D2D1::ColorF &value, const std::function<void(visible_surface, utility::error_code)> &callback){
	synchronized_item_execute_or_post_task_inside_thread_context([&]{
		try{
			set_background_color_(value);
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

void winp::ui::visible_surface::show_(){}

void winp::ui::visible_surface::hide_(){}

bool winp::ui::visible_surface::is_visible_() const{
	return false;
}

void winp::ui::visible_surface::set_background_transparency_(bool is_transparent){}

bool winp::ui::visible_surface::is_transparent_background_() const{
	return false;
}

void winp::ui::visible_surface::set_background_color_(const D2D1::ColorF &value){
	background_color_ = value;
}

const D2D1::ColorF &winp::ui::visible_surface::get_background_color_() const{
	return background_color_;
}
