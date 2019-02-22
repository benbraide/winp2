#include "../app/app_collection.h"
#include "../thread/thread_item.h"

winp::events::object::object(thread::item &target, const std::function<void(object &)> &default_handler)
	: object(target, target, default_handler){}

winp::events::object::object(thread::item &target, thread::item &context, const std::function<void(object &)> &default_handler)
	: target_(target), context_(context), default_handler_(default_handler){}

winp::events::object::~object() = default;

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

bool winp::events::object::do_default(){
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;

	if ((states_ & (state_default_prevented | state_default_done)) != 0u)
		return false;//Default prevented or done

	states_ |= state_default_done;
	if (default_handler_ == nullptr)
		context_.trigger_event_(*this, true);
	else//Call default handler
		default_handler_(*this);

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
	if (!object::do_default() || (states_ & state_default_prevented) != 0u)
		return false;

	//TODO: Check if the context is a window object and if handles are same
	if (original_message_.message < WM_APP && (states_ & state_default_prevented) == 0u){//Call default
		if ((states_ & state_result_set) == 0u){//Use result
			states_ |= state_result_set;
			value_ = CallWindowProcW(default_callback_, original_message_.hwnd, original_message_.message, original_message_.wParam, original_message_.lParam);
		}
		else//Discard result
			CallWindowProcW(default_callback_, original_message_.hwnd, original_message_.message, original_message_.wParam, original_message_.lParam);
	}

	return true;
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
