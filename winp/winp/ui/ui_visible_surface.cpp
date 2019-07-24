#include "../app/app_object.h"

#include "ui_visible_surface.h"

winp::ui::visible_surface::visible_surface() = default;

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
		return thread::item::pass_return_value_to_callback(callback, *this, set_background_color_(value, true));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::visible_surface::set_background_color(const D2D1::ColorF &value, const std::function<void(visible_surface, utility::error_code)> &callback){
	return synchronized_item_compute_or_post_task_inside_thread_context([=]{
		return thread::item::pass_return_value_to_callback(callback, *this, set_background_color_(value, true));
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

winp::utility::error_code winp::ui::visible_surface::set_background_color_(const D2D1_COLOR_F &value, bool allow_animation){
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
		return update_background_color_(background_color_);

	auto animation_hk = object_self->find_hook<animation_hook>();
	if (animation_hk == nullptr)//Animation not set
		return update_background_color_(background_color_);

	if (auto color_info = object_self->get_existing_animation_info_<D2D1_COLOR_F>(*animation_hk); color_info != nullptr){
		color_info->is_active = false;
		color_info->value = background_color_;
		++color_info->count;
	}

	if (!allow_animation)
		return update_background_color_(background_color_);

	if (auto error_code = animate_background_color_(*object_self, *animation_hk, start_color); error_code != utility::error_code::nil)
		return error_code;

	return update_background_color_(background_color_);
}

winp::utility::error_code winp::ui::visible_surface::animate_background_color_(object &object_self, animation_hook &hk, const D2D1_COLOR_F &value){
	auto &color_info = object_self.get_animation_info_<D2D1_COLOR_F>(hk);
	auto is_allowed = (color_info.is_allowed && color_info.easing != nullptr && color_info.duration != std::chrono::microseconds(0));

	auto error_code = utility::error_code::nil;
	auto suppression_hk = object_self.find_hook_<animation_suppression_hook>();

	if (suppression_hk != nullptr){
		auto is_suppressed = suppression_hk->type_is_suppressed<D2D1_COLOR_F>();

		if ((is_suppressed || !is_allowed)){//Animation suppressed
			error_code = update_background_color_(background_color_);
			is_allowed = false;
		}

		if (is_suppressed && suppression_hk->is_once())
			object_self.remove_hook_<animation_suppression_hook>();//Remove hook
	}

	if (!is_allowed){//Animation suppressed
		color_info.is_active = false;
		return ((error_code == utility::error_code::nil) ? utility::error_code::animation_suppressed : error_code);
	}

	synchronized_item_trigger_event_<events::animation>(thread::item::event_manager_type::get_key<D2D1_COLOR_F>(), events::animation::progress_type::begin);

	color_info.is_active = true;
	color_info.value = value;

	D2D1_COLOR_F color_delta{ (background_color_.r - value.r), (background_color_.g - value.g), (background_color_.b - value.b), (background_color_.a - value.a) }, last_color = value;
	object_self.thread_.animate(color_info.easing, [count = ++color_info.count, object_self_ref = &object_self, this, value, color_delta, last_color](float progress, bool has_more) mutable{
		auto animation_hk = object_self_ref->find_hook<animation_hook>();
		if (animation_hk == nullptr){
			update_background_color_(value);
			return false;
		}

		auto color_info = object_self_ref->get_existing_animation_info_<D2D1_COLOR_F>(*animation_hk);
		if (color_info == nullptr){
			update_background_color_(value);
			return false;
		}

		if (color_info->is_active && color_info->count == count){//Step
			D2D1_COLOR_F color{ (value.r + (color_delta.r * progress)), (value.g + (color_delta.g * progress)), (value.b + (color_delta.b * progress)), (value.a + (color_delta.a * progress)) };

			if (!compare_colors(color, last_color)){
				color_info->value = color;
				update_background_color_(color);
			}

			last_color = color;
			synchronized_item_trigger_event_<events::animation>(thread::item::event_manager_type::get_key<D2D1_COLOR_F>(), events::animation::progress_type::step);

			if (!has_more){//Ended
				color_info->is_active = false;
				synchronized_item_trigger_event_<events::animation>(thread::item::event_manager_type::get_key<D2D1_COLOR_F>(), events::animation::progress_type::end);
			}
		}
		else if (color_info->count == count){//Not active
			color_info->value = value;
			update_background_color_(value);
			synchronized_item_trigger_event_<events::animation>(thread::item::event_manager_type::get_key<D2D1_COLOR_F>(), events::animation::progress_type::cancel);
		}
		else//Canceled
			synchronized_item_trigger_event_<events::animation>(thread::item::event_manager_type::get_key<D2D1_COLOR_F>(), events::animation::progress_type::cancel);

		return (color_info->is_active && color_info->count == count);
	}, color_info.duration);

	return utility::error_code::animation_started;
}

winp::utility::error_code winp::ui::visible_surface::update_background_color_(const D2D1_COLOR_F &value){
	return redraw_();
}

const D2D1_COLOR_F &winp::ui::visible_surface::get_background_color_() const{
	return background_color_;
}

const D2D1_COLOR_F &winp::ui::visible_surface::get_current_background_color_() const{
	auto object_self = dynamic_cast<const object *>(this);
	if (object_self == nullptr)
		return background_color_;

	auto animation_hk = object_self->find_hook<animation_hook>();
	if (animation_hk == nullptr)
		return background_color_;

	if (auto position_info = object_self->get_existing_animation_info_<D2D1_COLOR_F>(*animation_hk); position_info != nullptr && position_info->is_active)
		return *std::any_cast<D2D1_COLOR_F>(&position_info->value);

	return background_color_;
}
