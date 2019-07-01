#include "../app/app_object.h"

#include "button_control.h"

winp::control::button::button(thread::object &thread)
	: button(thread, WC_BUTTONW, ICC_STANDARD_CLASSES){}

winp::control::button::button(thread::object &thread, const std::wstring &class_name, DWORD common_id)
	: object(thread, class_name, common_id){}

winp::control::button::~button() = default;

winp::utility::error_code winp::control::button::set_text(const std::wstring &value, const std::function<void(button &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_text_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

const std::wstring &winp::control::button::get_text(const std::function<void(const std::wstring &)> &callback) const{
	return *compute_or_post_task_inside_thread_context([=]{
		return &pass_return_ref_value_to_callback(callback, &text_);
	}, (callback != nullptr), &text_);
}

winp::utility::error_code winp::control::button::set_font(HFONT value, const std::function<void(button &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_font_(value));
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
		return pass_return_value_to_callback(callback, *this, set_padding_(horizontal, vertical));
	}, (callback != nullptr), utility::error_code::nil);
}

const SIZE &winp::control::button::get_padding(const std::function<void(const SIZE &)> &callback) const{
	return *compute_or_post_task_inside_thread_context([=]{
		return &pass_return_ref_value_to_callback(callback, &padding_);
	}, (callback != nullptr), &padding_);
}

winp::utility::error_code winp::control::button::set_scale(const scale_type &value, const std::function<void(button &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_scale_(value));
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

winp::utility::error_code winp::control::button::set_text_(const std::wstring &value){
	text_ = value;
	if (handle_ != nullptr)
		SendMessageW(handle_, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(value.data()));

	return update_size_();
}

winp::utility::error_code winp::control::button::set_font_(HFONT value){
	if (value == font_)
		return utility::error_code::nil;

	SendMessageW(handle_, WM_SETFONT, reinterpret_cast<WPARAM>(font_ = value), TRUE);
	return update_size_();
}

winp::utility::error_code winp::control::button::set_padding_(int horizontal, int vertical){
	padding_.cx = horizontal;
	padding_.cy = vertical;
	return update_size_();
}

winp::utility::error_code winp::control::button::set_scale_(const scale_type &value){
	scale_ = value;
	return update_size_();
}

winp::utility::error_code winp::control::button::update_size_(){
	auto size = compute_size_(), additional_size = compute_additional_size_();
	return set_size_((size.cx + additional_size.cx + padding_.cx), (size.cy + additional_size.cy + padding_.cy), false);
}

SIZE winp::control::button::compute_size_() const{
	static const auto symbol_list = L"AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz";

	auto device = GetDC(handle_);
	if (device == nullptr)
		return SIZE{};

	SaveDC(device);
	SelectObject(device, font_);

	SIZE size{}, symbols_size{};
	GetTextExtentPoint32W(device, symbol_list, static_cast<int>(std::wcslen(symbol_list)), &symbols_size);

	if (!text_.empty())
		GetTextExtentPoint32W(device, text_.data(), static_cast<int>(text_.size()), &size);

	RestoreDC(device, -1);
	ReleaseDC(handle_, device);

	return SIZE{ static_cast<int>(size.cx * scale_.width), static_cast<int>(symbols_size.cy * scale_.height) };
}

SIZE winp::control::button::compute_additional_size_() const{
	return SIZE{};
}
