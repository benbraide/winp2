#include "../app/app_object.h"
#include "../ui/ui_window_surface.h"

winp::events::object::object(thread::item &target, const std::function<void(object &)> &default_handler)
	: object(target, target, default_handler){}

winp::events::object::object(thread::item &target, thread::item &context, const std::function<void(object &)> &default_handler)
	: target_(target), context_(&context), default_handler_(default_handler){
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
	return *context_;
}

void winp::events::object::prevent_default(){
	if (target_.get_thread().is_thread_context())
		states_ |= state_default_prevented;
	else
		throw utility::error_code::outside_thread_context;
}

bool winp::events::object::do_default(){
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;

	if ((states_ & (state_default_prevented | state_default_done)) != 0u)
		return false;//Default prevented or done

	if ((states_ & state_doing_default) != 0u)
		return true;//Default prevented or done

	auto propagation_stopped = false;
	if ((states_ & state_propagation_stopped) != 0u){
		propagation_stopped = true;
		states_ &= ~state_propagation_stopped;
	}

	states_ |= state_doing_default;
	if (default_handler_ == nullptr)
		context_->trigger_event_handler_(*this);
	else//Call default handler
		default_handler_(*this);

	states_ |= state_default_done;
	states_ &= ~state_doing_default;

	if (propagation_stopped)
		states_ |= state_propagation_stopped;
	else
		states_ &= ~state_propagation_stopped;

	return true;
}

void winp::events::object::stop_propagation(){
	if (target_.get_thread().is_thread_context())
		states_ |= state_propagation_stopped;
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

winp::events::object_with_message::~object_with_message() = default;

bool winp::events::object_with_message::do_default(){
	return ((object::do_default() && (states_ & state_default_prevented) == 0u) ? call_default_() : false);
}

const MSG &winp::events::object_with_message::get_message() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return message_;
}

MSG &winp::events::object_with_message::get_message(){
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return message_;
}

bool winp::events::object_with_message::should_call_call_default_() const{
	if (default_callback_ == nullptr || original_message_.hwnd == nullptr || WM_APP <= original_message_.message || (states_ & state_default_prevented) != 0u)
		return false;

	if (auto window_context = dynamic_cast<ui::window_surface *>(context_); window_context != nullptr && window_context->get_handle() == original_message_.hwnd)
		return true;

	return false;
}

bool winp::events::object_with_message::call_default_(){
	if ((states_ & state_default_prevented) == 0u && should_call_call_default_()){//Call default
		if ((states_ & state_result_set) == 0u){//Use result
			states_ |= state_default_result_set;
			result_ = get_called_default_value_();
		}
		else//Discard result
			get_called_default_value_();
	}

	return true;
}

LRESULT winp::events::object_with_message::get_called_default_value_(){
	return CallWindowProcW(default_callback_, original_message_.hwnd, original_message_.message, original_message_.wParam, original_message_.lParam);
}
