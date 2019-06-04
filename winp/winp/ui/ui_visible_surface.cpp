#include "../app/app_object.h"

#include "ui_visible_surface.h"

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

winp::utility::error_code winp::ui::visible_surface::set_background_brush(ID2D1Brush *value, const std::function<void(visible_surface, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, set_background_brush_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

ID2D1Brush *winp::ui::visible_surface::get_background_brush(const std::function<void(bool)> &callback) const{
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return synchronized_item_pass_return_value_to_callback(callback, background_brush_);
	}, (callback != nullptr), nullptr);
}

winp::utility::error_code winp::ui::visible_surface::set_background_color(const D2D1_COLOR_F &value, const std::function<void(visible_surface, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, set_background_color_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::visible_surface::set_background_color(const D2D1::ColorF &value, const std::function<void(visible_surface, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, set_background_color_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

const D2D1_COLOR_F &winp::ui::visible_surface::get_background_color(const std::function<void(const D2D1_COLOR_F &)> &callback) const{
	return *synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return &synchronized_item_pass_return_ref_value_to_callback(callback, &get_background_color_());
	}, (callback != nullptr), &background_color_);
}

COLORREF winp::ui::visible_surface::convert_rgb_to_rgba(COLORREF rgb, BYTE a){
	return (RGB(GetRValue(rgb), GetGValue(rgb), GetBValue(rgb)) | (static_cast<COLORREF>(a) << 24));
}

COLORREF winp::ui::visible_surface::convert_colorf_to_colorref(const D2D1_COLOR_F &value){
	return (RGB(static_cast<BYTE>(value.r * 255.0f), static_cast<BYTE>(value.g * 255.0f), static_cast<BYTE>(value.b * 255.0f)) | (static_cast<COLORREF>(value.a * 255.0f) << 24));
}

COLORREF winp::ui::visible_surface::convert_colorf_to_colorref(const D2D1::ColorF &value){
	return (RGB(static_cast<BYTE>(value.r * 255.0f), static_cast<BYTE>(value.g * 255.0f), static_cast<BYTE>(value.b * 255.0f)) | (static_cast<COLORREF>(value.a * 255.0f) << 24));
}

D2D1_COLOR_F winp::ui::visible_surface::convert_colorref_to_colorf(COLORREF value){
	return D2D1::ColorF(
		(GetRValue(value) / 255.0f),
		(GetGValue(value) / 255.0f),
		(GetBValue(value) / 255.0f),
		((LOBYTE((value) >> 24)) / 255.0f)
	);
}

D2D1_COLOR_F winp::ui::visible_surface::convert_colorref_to_colorf(COLORREF value, BYTE a){
	return D2D1::ColorF(
		(GetRValue(value) / 255.0f),
		(GetGValue(value) / 255.0f),
		(GetBValue(value) / 255.0f),
		(a / 255.0f)
	);
}

bool winp::ui::visible_surface::compare_colors(const D2D1_COLOR_F &first, const D2D1_COLOR_F &second){
	return (convert_colorf_to_colorref(first) == convert_colorf_to_colorref(second));
}

bool winp::ui::visible_surface::compare_colors(const D2D1::ColorF &first, const D2D1::ColorF &second){
	return (convert_colorf_to_colorref(first) == convert_colorf_to_colorref(second));
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

winp::utility::error_code winp::ui::visible_surface::set_background_brush_(ID2D1Brush *value){
	if (value == background_brush_)//Do nothing
		return utility::error_code::nil;

	if ((synchronized_item_trigger_event_<events::background_brush_change>(value, true).first & events::object::state_default_prevented) != 0u)
		return utility::error_code::action_prevented;

	background_brush_ = value;
	synchronized_item_trigger_event_<events::background_brush_change>(value, false);
	redraw_();

	return utility::error_code::nil;
}

winp::utility::error_code winp::ui::visible_surface::set_background_color_(const D2D1_COLOR_F &value){
	if (compare_colors(value, background_color_))
		return utility::error_code::nil;//No changes

	if ((synchronized_item_trigger_event_<events::background_color_change>(value, true).first & events::object::state_default_prevented) != 0u)
		return utility::error_code::action_prevented;

	auto start_color = background_color_;
	background_color_ = value;

	synchronized_item_trigger_event_<events::background_color_change>(value, false);
	if (background_brush_ != nullptr)//Color ignored
		return utility::error_code::nil;

	auto object_self = dynamic_cast<object *>(this);
	if (object_self == nullptr)
		return background_color_change_(value);

	auto animation_suppression_hk = object_self->find_hook<animation_suppression_hook>();
	if (animation_suppression_hk != nullptr && (animation_suppression_hk->type_is_suppressed<D2D1_COLOR_F>() || animation_suppression_hk->type_is_suppressed<COLORREF>())){
		++background_color_animation_state_;//Cancel animation, if any
		if (animation_suppression_hk->is_once())//Remove hook
			object_self->remove_hook<animation_suppression_hook>();

		return background_color_change_(value);
	}

	if (auto animation_hk = object_self->find_hook<animation_hook>(); animation_hk != nullptr){//Animate values
		auto easing = animation_hk->get_easing();
		if (easing == nullptr)//Easing required
			return background_color_change_(value);

		auto duration = animation_hk->get_duration();
		auto state = ++background_color_animation_state_;

		D2D1_COLOR_F color_delta{ (value.r - start_color.r), (value.g - start_color.g), (value.b - start_color.b), (value.a - start_color.a) }, last_color = start_color;
		object_self->get_thread().animate(easing, [=](float progress, bool has_more) mutable{
			if (background_color_animation_state_ == state){
				D2D1_COLOR_F color{ (start_color.r + (color_delta.r * progress)), (start_color.g + (color_delta.g * progress)), (start_color.b + (color_delta.b * progress)), (start_color.a + (color_delta.a * progress)) };
				if (!compare_colors(color, last_color))
					background_color_change_(color);
				last_color = color;
			}

			return (background_color_animation_state_ == state);
		}, duration);

		return utility::error_code::animation_started;
	}

	return background_color_change_(value);
}

winp::utility::error_code winp::ui::visible_surface::background_color_change_(const D2D1_COLOR_F &value){
	current_background_color_ = value;
	return redraw_();
}

const D2D1_COLOR_F &winp::ui::visible_surface::get_background_color_() const{
	return background_color_;
}
