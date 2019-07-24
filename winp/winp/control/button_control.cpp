#include "../app/app_object.h"

#include "button_control.h"

winp::control::button::button()
	: button(WC_BUTTONW, ICC_STANDARD_CLASSES){
	add_event_handler_([this](events::measure_item &e){
		auto size = e.measure_themed_text(get_theme_part_id_(), 0, text_, 0u);
		if (size.cx == 0 && size.cy == 0)//No theme
			size = e.measure_text(text_, font_, 0u);

		SIZE computed_size{ static_cast<int>(size.cx * scale_.width), static_cast<int>(size.cy * scale_.height) };
		auto additional_size = compute_additional_size_(computed_size);

		return SIZE{ (computed_size.cx + additional_size.cx + padding_.cx), (computed_size.cy + additional_size.cy + padding_.cy) };
	});
}

winp::control::button::button(const std::wstring &class_name, DWORD common_id)
	: object(class_name, common_id){}

winp::control::button::~button() = default;

winp::utility::error_code winp::control::button::set_text(const std::wstring &value, const std::function<void(button &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_text_(value, true));
	}, (callback != nullptr), utility::error_code::nil);
}

const std::wstring &winp::control::button::get_text(const std::function<void(const std::wstring &)> &callback) const{
	return *compute_or_post_task_inside_thread_context([=]{
		return &pass_return_ref_value_to_callback(callback, &text_);
	}, (callback != nullptr), &text_);
}

winp::utility::error_code winp::control::button::set_font(HFONT value, const std::function<void(button &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_font_(value, true));
	}, (callback != nullptr), utility::error_code::nil);
}

HFONT winp::control::button::get_font(const std::function<void(HFONT)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, font_);
	}, (callback != nullptr), nullptr);
}

winp::utility::error_code winp::control::button::set_padding(const SIZE &value, const std::function<void(button &, utility::error_code)> &callback){
	return set_padding(value.cx, value.cy);
}

winp::utility::error_code winp::control::button::set_padding(int horizontal, int vertical, const std::function<void(button &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_padding_(horizontal, vertical, true));
	}, (callback != nullptr), utility::error_code::nil);
}

const SIZE &winp::control::button::get_padding(const std::function<void(const SIZE &)> &callback) const{
	return *compute_or_post_task_inside_thread_context([=]{
		return &pass_return_ref_value_to_callback(callback, &padding_);
	}, (callback != nullptr), &padding_);
}

winp::utility::error_code winp::control::button::set_scale(const scale_type &value, const std::function<void(button &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_scale_(value, true));
	}, (callback != nullptr), utility::error_code::nil);
}

const winp::control::button::scale_type &winp::control::button::get_scale(const std::function<void(const scale_type &)> &callback) const{
	return *compute_or_post_task_inside_thread_context([=]{
		return &pass_return_ref_value_to_callback(callback, &scale_);
	}, (callback != nullptr), &scale_);
}

DWORD winp::control::button::get_filtered_styles_(bool is_extended) const{
	return ~(is_extended ? 0u : (WS_TABSTOP | WS_VISIBLE));
}

const wchar_t *winp::control::button::get_window_text_() const{
	return text_.data();
}

int winp::control::button::get_theme_part_id_() const{
	return 0;
}

winp::utility::error_code winp::control::button::set_text_(const std::wstring &value, bool allow_animation){
	text_ = value;
	if (handle_ != nullptr)
		SendMessageW(handle_, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(value.data()));

	return update_size_(allow_animation);
}

winp::utility::error_code winp::control::button::set_font_(HFONT value, bool allow_animation){
	if (value == font_)
		return utility::error_code::nil;

	SendMessageW(handle_, WM_SETFONT, reinterpret_cast<WPARAM>(font_ = value), TRUE);
	return update_size_(allow_animation);
}

winp::utility::error_code winp::control::button::set_padding_(int horizontal, int vertical, bool allow_animation){
	padding_.cx = horizontal;
	padding_.cy = vertical;
	return update_size_(allow_animation);
}

winp::utility::error_code winp::control::button::set_scale_(const scale_type &value, bool allow_animation){
	scale_ = value;
	return update_size_(allow_animation);
}

winp::utility::error_code winp::control::button::update_size_(bool allow_animation){
	MEASUREITEMSTRUCT info{ ODT_BUTTON };
	MSG msg{ nullptr, WM_MEASUREITEM, 0, reinterpret_cast<LPARAM>(&info) };

	trigger_event_<events::measure_item>(msg, nullptr);
	return set_size_(info.itemWidth, info.itemHeight, allow_animation);
}

SIZE winp::control::button::compute_additional_size_(const SIZE &computed_size) const{
	return SIZE{};
}
