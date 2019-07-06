#include "../app/app_object.h"

#include "window_object.h"

winp::window::object::object(){
	styles_ |= WS_OVERLAPPEDWINDOW;
}

winp::window::object::object(tree &parent)
	: object(parent, static_cast<std::size_t>(-1)){}

winp::window::object::object(tree &parent, std::size_t index)
	: object(){
	set_parent(&parent, index);
}

winp::utility::error_code winp::window::object::set_caption(const std::wstring &value, const std::function<void(object &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_caption_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

const std::wstring &winp::window::object::get_caption(const std::function<void(const std::wstring &)> &callback) const{
	return *compute_or_post_task_inside_thread_context([=]{
		return &pass_return_ref_value_to_callback(callback, &get_caption_());
	}, (callback != nullptr), &caption_);
}

const wchar_t *winp::window::object::get_window_text_() const{
	return caption_.data();
}

winp::utility::error_code winp::window::object::set_caption_(const std::wstring &value){
	caption_ = value;
	if (handle_ == nullptr)
		return utility::error_code::nil;
	return ((SendMessageW(handle_, WM_SETTEXT, 0, reinterpret_cast<WPARAM>(value.data())) == FALSE) ? utility::error_code::action_could_not_be_completed : utility::error_code::nil);
}

const std::wstring &winp::window::object::get_caption_() const{
	return caption_;
}

winp::window::object::~object() = default;
