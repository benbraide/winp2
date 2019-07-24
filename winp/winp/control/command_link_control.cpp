#include "../app/app_object.h"

#include "command_link_control.h"

winp::control::command_link::command_link() = default;

winp::control::command_link::command_link(tree &parent)
	: command_link(parent, static_cast<std::size_t>(-1)){}

winp::control::command_link::command_link(tree &parent, std::size_t index){
	set_parent(&parent, index);
}

winp::control::command_link::~command_link() = default;

winp::utility::error_code winp::control::command_link::set_note(const std::wstring &value, const std::function<void(command_link &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_note_(value, true));
	}, (callback != nullptr), utility::error_code::nil);
}

const std::wstring &winp::control::command_link::get_note(const std::function<void(const std::wstring &)> &callback) const{
	return *compute_or_post_task_inside_thread_context([=]{
		return &pass_return_ref_value_to_callback(callback, &note_);
	}, (callback != nullptr), &note_);
}

winp::utility::error_code winp::control::command_link::create_(){
	if (auto error_code = push_button::create_(); error_code != utility::error_code::nil)
		return error_code;

	if (!note_.empty())
		SendMessageW(handle_, BCM_SETNOTE, 0, reinterpret_cast<LPARAM>(note_.data()));

	return utility::error_code::nil;
}

DWORD winp::control::command_link::get_persistent_styles_(bool is_extended) const{
	return (push_button::get_persistent_styles_(is_extended) | (is_extended ? 0u : BS_COMMANDLINK));
}

SIZE winp::control::command_link::compute_additional_size_(const SIZE &computed_size) const{
	if (note_.empty())
		return SIZE{ (GetSystemMetrics(SM_CXMENUCHECK) + GetSystemMetrics(SM_CXEDGE) + 15), (GetSystemMetrics(SM_CYEDGE) + 15) };
	return SIZE{ (GetSystemMetrics(SM_CXMENUCHECK) + GetSystemMetrics(SM_CXEDGE) + 15), (GetSystemMetrics(SM_CYEDGE) + 30) };
}

winp::utility::error_code winp::control::command_link::set_note_(const std::wstring &value, bool allow_animation){
	note_ = value;
	if (handle_ != nullptr)
		SendMessageW(handle_, BCM_SETNOTE, 0, reinterpret_cast<LPARAM>(value.data()));

	return update_size_(allow_animation);
}
