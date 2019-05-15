#include "app_object.h"

const WNDCLASSEXW &winp::app::object::get_class_info(){
	return class_info_;
}

const std::wstring &winp::app::object::get_class_name(){
	return class_name_;
}

int winp::app::object::run(){
	return ((current_thread_ == nullptr) ? 0 : current_thread_->run());
}

void winp::app::object::stop(int exit_code){
	if (current_thread_ != nullptr)
		current_thread_->stop(exit_code);
}

winp::thread::object *winp::app::object::find_thread(DWORD id){
	if (id == GetCurrentThreadId())
		return current_thread_;

	std::lock_guard<std::mutex> guard(lock_);
	if (threads_.empty())
		return nullptr;

	auto it = threads_.find(id);
	return ((it == threads_.end()) ? nullptr : it->second.get());
}

winp::thread::object *winp::app::object::find_thread(const std::thread::id &id){
	if (id == std::this_thread::get_id())
		return current_thread_;

	std::lock_guard<std::mutex> guard(lock_);
	if (threads_.empty())
		return nullptr;

	for (auto &info : threads_){
		if (info.second->get_id() == id)
			return info.second.get();
	}

	return nullptr;
}

winp::thread::object &winp::app::object::get_thread(){
	if (current_thread_ == nullptr){
		std::lock_guard<std::mutex> guard(lock_);
		threads_[GetCurrentThreadId()].reset(new thread::object);
	}

	return *current_thread_;
}

winp::thread::object *winp::app::object::get_current_thread(){
	return current_thread_;
}

DWORD winp::app::object::convert_thread_id_to_local_id(const std::thread::id &value){
	if (value == std::this_thread::get_id())
		return GetCurrentThreadId();

	auto thread = find_thread(value);
	return ((thread == nullptr) ? 0 : thread->get_local_id());
}

std::thread::id winp::app::object::convert_local_thread_id_to_id(DWORD value){
	if (value == GetCurrentThreadId())
		return std::this_thread::get_id();

	auto thread = find_thread(value);
	return ((thread == nullptr) ? std::thread::id() : thread->get_id());
}

void winp::app::object::traverse_threads(const std::function<void(thread::object &)> &callback, DWORD context_id, int task_priority, unsigned __int64 task_id){
	std::lock_guard<std::mutex> guard(lock_);
	if (threads_.empty())
		return;

	if (auto context = ((context_id == 0) ? threads_.end() : threads_.find(context_id)); context != threads_.end()){
		for (auto &item : threads_){
			context->second->get_queue().post_task([callback, thread = item.second]{
				callback(*thread);
			}, task_priority, task_id);
		}
	}
	else{//Use current context
		for (auto &item : threads_)
			callback(*item.second);
	}
}

void winp::app::object::traverse_threads(const std::function<void(thread::object &)> &callback, thread::object &context, int task_priority, unsigned __int64 task_id){
	std::lock_guard<std::mutex> guard(lock_);
	if (threads_.empty())
		return;

	for (auto &item : threads_){
		context.get_queue().post_task([callback, thread = item.second]{
			callback(*thread);
		}, task_priority, task_id);
	}
}

std::wstring winp::app::object::class_name_;

WNDCLASSEXW winp::app::object::class_info_{};

std::unordered_map<DWORD, std::shared_ptr<winp::thread::object>> winp::app::object::threads_;

thread_local winp::thread::object *winp::app::object::current_thread_ = nullptr;

winp::app::object::app_initializer_class winp::app::object::app_initializer_;

thread_local winp::app::object::thread_scope_class winp::app::object::thread_scope_;

std::mutex winp::app::object::lock_;

winp::app::object::app_initializer_class::app_initializer_class(){
	class_info_.cbSize = sizeof(WNDCLASSEXW);
	class_info_.hInstance = GetModuleHandleW(nullptr);
	class_info_.lpfnWndProc = thread::object::get_message_entry();
	class_info_.lpszClassName = (class_name_ = WINP_CLASS_WUUID).data();
	class_info_.lpszMenuName = nullptr;
	class_info_.style = (CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS);
	class_info_.hbrBackground = nullptr;
	class_info_.hCursor = nullptr;
	class_info_.hIconSm = nullptr;
	class_info_.hIcon = nullptr;
	class_info_.cbWndExtra = 0;
	class_info_.cbClsExtra = 0;

	RegisterClassExW(&class_info_);
}

winp::app::object::thread_scope_class::thread_scope_class() = default;

winp::app::object::thread_scope_class::~thread_scope_class(){
	if (current_thread_ != nullptr){//Remove associated object
		std::lock_guard<std::mutex> guard(lock_);
		if (!threads_.empty())
			threads_.erase(current_thread_->local_id_);

		current_thread_ = nullptr;
	}
}
