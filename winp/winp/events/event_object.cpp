#include "../app/app_object.h"
#include "../ui/ui_window_surface.h"

winp::events::object::object(thread::item &target)
	: object(target, target){}

winp::events::object::object(thread::item &target, thread::item &context)
	: target_(target), context_(context){
	target_.get_thread().current_event_ = this;
}

winp::events::object::~object(){
	if (target_.get_thread().current_event_ == this)
		target_.get_thread().current_event_ = nullptr;
}

winp::thread::object &winp::events::object::get_thread(){
	return target_.get_thread();
}

winp::thread::item &winp::events::object::get_target(){
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return target_;
}

winp::thread::item &winp::events::object::get_context(){
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return context_;
}

void winp::events::object::prevent_default(){
	if (target_.get_thread().is_thread_context())
		states_ |= state_default_prevented;
	else
		throw utility::error_code::outside_thread_context;
}

void winp::events::object::do_default(){
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;

	if ((states_ & (state_default_prevented | state_doing_default | state_default_done)) != 0u)
		return;//Default prevented or done

	auto propagation_stopped = false;
	if ((states_ & state_propagation_stopped) != 0u){
		propagation_stopped = true;
		states_ &= ~state_propagation_stopped;
	}

	states_ |= state_doing_default;
	context_.trigger_event_handler_(*this);

	states_ |= state_default_done;
	states_ &= ~state_doing_default;

	if (propagation_stopped)
		states_ |= state_propagation_stopped;
	else
		states_ &= ~state_propagation_stopped;
}

void winp::events::object::stop_propagation(){
	if (target_.get_thread().is_thread_context())
		states_ |= state_propagation_stopped;
	else
		throw utility::error_code::outside_thread_context;
}

void winp::events::object::unbind_on_exit(){
	if (target_.get_thread().is_thread_context())
		states_ |= state_unbind_on_exit;
	else
		throw utility::error_code::outside_thread_context;
}

unsigned int winp::events::object::get_states() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return states_;
}

bool winp::events::object::is_thread_context() const{
	return target_.get_thread().is_thread_context();
}

bool winp::events::object::set_result_untyped_(const std::any &result){
	return false;
}

winp::events::object_with_message::~object_with_message() = default;

void winp::events::object_with_message::do_default(){
	object::do_default();
	call_default_();
}

const MSG &winp::events::object_with_message::get_message() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return message_info_;
}

MSG &winp::events::object_with_message::get_message(){
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return message_info_;
}

bool winp::events::object_with_message::should_call_default_() const{
	if (default_callback_ == nullptr || message_info_.hwnd == nullptr || WM_APP <= message_info_.message || (states_ & state_default_prevented) != 0u)
		return false;

	if (auto window_context = dynamic_cast<ui::window_surface *>(&context_); window_context != nullptr && window_context->get_handle() == message_info_.hwnd)
		return true;

	return false;
}

void winp::events::object_with_message::call_default_(){
	if ((states_ & (state_default_prevented | state_default_called)) != 0u || !should_call_default_())
		return;

	if ((states_ & state_result_set) == 0u){//Use result
		states_ |= state_default_result_set;
		result_ = get_called_default_value_();
	}
	else//Discard result
		get_called_default_value_();

	states_ |= state_default_called;
}

LRESULT winp::events::object_with_message::get_called_default_value_(){
	return CallWindowProcW(default_callback_, message_info_.hwnd, message_info_.message, message_info_.wParam, message_info_.lParam);
}
