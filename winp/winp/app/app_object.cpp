#include "app_collection.h"

winp::app::object::object(){
	collection::add_(*this);
}

winp::app::object::~object(){
	collection::remove_(*this);
}

void winp::app::object::traverse_threads(const std::function<void(thread::object &)> &callback){
	std::unordered_map<DWORD, thread::object *> list;
	{//Scoped lock
		std::lock_guard<std::mutex> guard(lock_);
		list = threads_;
	}

	for (auto &item : list)
		callback(*item.second);
}

void winp::app::object::add_thread_(thread::object &thread){
	std::lock_guard<std::mutex> guard(lock_);
	threads_[thread.local_id_] = &thread;
}

void winp::app::object::remove_thread_(DWORD id){
	std::lock_guard<std::mutex> guard(lock_);
	if (!threads_.empty())
		threads_.erase(id);
}

winp::app::main_object::main_object()
	: thread_(*this){
	std::lock_guard<std::mutex> guard(collection::lock_);
	if (collection::main_ == nullptr)
		collection::main_ = this;
	else
		throw utility::error_code::multiple_main_apps;
}

winp::thread::object &winp::app::main_object::get_thread(){
	return thread_;
}
