#include "app_object.h"

winp::app::main_object *winp::app::object::get_main(){
	return main_;
}

void winp::app::object::add_thread_(thread::object &thread){
	threads_[thread.local_id_] = &thread;
}

void winp::app::object::remove_thread_(DWORD id){
	if (!threads_.empty())
		threads_.erase(id);
}

winp::app::main_object *winp::app::object::main_ = nullptr;

winp::app::main_object::main_object()
	: thread_(*this){
	if (main_ == nullptr)
		main_ = this;
	else
		throw utility::error_code::multiple_main_apps;
}

winp::thread::object &winp::app::main_object::get_thread(){
	return thread_;
}
