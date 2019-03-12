#include "app_collection.h"

winp::app::main_object *winp::app::collection::get_main(){
	return main_;
}

winp::app::object *winp::app::collection::find_app(DWORD id){
	std::lock_guard<std::mutex> guard(lock_);
	if (apps_.empty())
		return nullptr;

	auto it = apps_.find(id);
	return ((it == apps_.end()) ? nullptr : it->second);
}

winp::app::object *winp::app::collection::find_app(const std::thread::id &id){
	std::lock_guard<std::mutex> guard(lock_);
	if (apps_.empty())
		return nullptr;

	for (auto &info : apps_){
		if (info.second->get_id() == id)
			return info.second;
	}

	return nullptr;
}

winp::thread::object *winp::app::collection::find_thread(DWORD id){
	std::lock_guard<std::mutex> guard(lock_);
	return unsafe_find_thread_(id);
}

winp::thread::object *winp::app::collection::find_thread(const std::thread::id &id){
	std::lock_guard<std::mutex> guard(lock_);
	if (apps_.empty())
		return nullptr;

	for (auto &info : apps_){
		if (auto thread = info.second->find_thread(id); thread != nullptr)
			return thread;
	}

	return nullptr;
}

winp::app::object *winp::app::collection::get_current_app(){
	return current_app_;
}

winp::app::object *winp::app::collection::get_current_thread_app(){
	auto current_thread = get_current_thread();
	return ((current_thread == nullptr) ? current_app_ : &current_thread->get_app());
}

winp::thread::object *winp::app::collection::get_current_thread(){
	return object::get_current_thread();
}

DWORD winp::app::collection::convert_thread_id_to_local_id(const std::thread::id &value){
	if (value == std::this_thread::get_id())
		return GetCurrentThreadId();

	std::lock_guard<std::mutex> guard(lock_);
	if (apps_.empty())
		return 0;

	for (auto &item : apps_){
		if (auto id = item.second->convert_thread_id_to_local_id(value); id != 0)
			return id;
	}

	return 0;
}

std::thread::id winp::app::collection::convert_local_thread_id_to_id(DWORD value){
	if (value == GetCurrentThreadId())
		return std::this_thread::get_id();

	std::lock_guard<std::mutex> guard(lock_);
	if (apps_.empty())
		return std::thread::id();

	for (auto &item : apps_){
		if (auto id = item.second->convert_local_thread_id_to_id(value); id != std::thread::id())
			return id;
	}

	return std::thread::id();
}

void winp::app::collection::traverse_objects(const std::function<void(object &)> &callback, DWORD context_id, int task_priority, unsigned __int64 task_id){
	std::lock_guard<std::mutex> guard(lock_);
	if (apps_.empty())
		return;

	if (auto context = ((context_id == 0) ? nullptr : unsafe_find_thread_(context_id)); context != nullptr){
		for (auto &item : apps_){
			context->get_queue().post_task([callback, app = item.second]{
				callback(*app);
			}, task_priority, task_id);
		}
	}
	else{//Use current context
		for (auto &item : apps_)
			callback(*item.second);
	}
}

void winp::app::collection::traverse_objects(const std::function<void(object &)> &callback, thread::object &context, int task_priority, unsigned __int64 task_id){
	std::lock_guard<std::mutex> guard(lock_);
	if (apps_.empty())
		return;

	for (auto &item : apps_){
		context.get_queue().post_task([callback, app = item.second]{
			callback(*app);
		}, task_priority, task_id);
	}
}

void winp::app::collection::traverse_threads(const std::function<void(thread::object &)> &callback, DWORD context_id, int task_priority, unsigned __int64 task_id){
	std::lock_guard<std::mutex> guard(lock_);
	if (apps_.empty())
		return;

	if (auto context = ((context_id == 0) ? nullptr : unsafe_find_thread_(context_id)); context != nullptr){
		for (auto &item : apps_)
			item.second->traverse_threads(callback, *context, task_priority, task_id);
	}
	else{//Use current context
		for (auto &item : apps_)
			item.second->traverse_threads(callback);
	}
}

void winp::app::collection::traverse_threads(const std::function<void(thread::object &)> &callback, thread::object &context, int task_priority, unsigned __int64 task_id){
	std::lock_guard<std::mutex> guard(lock_);
	if (apps_.empty())
		return;

	for (auto &item : apps_)
		item.second->traverse_threads(callback, context, task_priority, task_id);
}

void winp::app::collection::enable_automatic_thread_creation(){
	std::lock_guard<std::mutex> guard(lock_);
	automatic_thread_creation_enabled_ = true;
}

void winp::app::collection::add_(object &item){
	std::lock_guard<std::mutex> guard(lock_);
	apps_[item.get_local_id()] = &item;
}

void winp::app::collection::remove_(DWORD id){
	std::lock_guard<std::mutex> guard(lock_);
	if (!apps_.empty())
		apps_.erase(id);
}

winp::thread::object *winp::app::collection::unsafe_find_thread_(DWORD id){
	if (apps_.empty())
		return nullptr;

	for (auto &info : apps_){
		if (auto thread = info.second->find_thread(id); thread != nullptr)
			return thread;
	}

	return nullptr;
}

DWORD winp::app::collection::create_thread_(){
	std::lock_guard<std::mutex> guard(lock_);
	if (!automatic_thread_creation_enabled_)
		return 0;

	if (main_ == nullptr)//Create main thread
		created_main_object_ = std::make_shared<app::main_object>();

	auto created_thread = std::make_shared<thread::object>();
	created_threads_[created_thread->get_local_id()] = created_thread;

	return created_thread->get_local_id();
}

void winp::app::collection::destroy_thread_(DWORD id){
	if (id == 0)
		return;

	std::shared_ptr<thread::object> created_thread;
	{//Scoped lock
		std::lock_guard<std::mutex> guard(lock_);
		if (created_threads_.empty())
			return;

		auto it = created_threads_.find(id);
		if (it == created_threads_.end())
			return;

		created_thread = it->second;
		created_threads_.erase(it);
	}
}

std::unordered_map<DWORD, std::shared_ptr<winp::thread::object>> winp::app::collection::created_threads_;

std::shared_ptr<winp::app::main_object> winp::app::collection::created_main_object_;

std::unordered_map<DWORD, winp::app::object *> winp::app::collection::apps_;

winp::utility::random_wide_string_generator winp::app::collection::random_wide_string_generator_;

winp::app::main_object *winp::app::collection::main_ = nullptr;

std::mutex winp::app::collection::lock_;

bool winp::app::collection::automatic_thread_creation_enabled_ = false;

thread_local winp::app::collection::automatic_thread_creation_class winp::app::collection::automatic_thread_creation_;

thread_local winp::app::object *winp::app::collection::current_app_ = nullptr;

winp::app::collection::automatic_thread_creation_class::automatic_thread_creation_class()
	: thread_id_(collection::create_thread_()){}

winp::app::collection::automatic_thread_creation_class::~automatic_thread_creation_class(){
	collection::destroy_thread_(thread_id_);
}
