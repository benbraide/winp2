#include "app_collection.h"

winp::app::object::object()
	: id_(std::this_thread::get_id()), local_id_(GetCurrentThreadId()){
	collection::current_app_ = this;
	collection::add_(*this);

	class_info_.cbSize = sizeof(WNDCLASSEXW);
	class_info_.hInstance = GetModuleHandleW(nullptr);
	class_info_.lpfnWndProc = thread::object::get_message_entry();
	class_info_.lpszClassName = (class_name_ = collection::random_wide_string_generator_(18, 36, utility::random_string_generator_char_set::alpha)).data();
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

winp::app::object::~object(){
	collection::remove_(local_id_);
	collection::current_app_ = nullptr;
}

std::thread::id winp::app::object::get_id() const{
	return id_;
}

DWORD winp::app::object::get_local_id() const{
	return local_id_;
}

const WNDCLASSEXW &winp::app::object::get_class_info() const{
	return class_info_;
}

const std::wstring &winp::app::object::get_class_name() const{
	return class_name_;
}

winp::thread::object *winp::app::object::find_thread(DWORD id) const{
	std::lock_guard<std::mutex> guard(lock_);
	if (threads_.empty())
		return nullptr;

	auto it = threads_.find(id);
	return ((it == threads_.end()) ? nullptr : it->second);
}

winp::thread::object *winp::app::object::find_thread(const std::thread::id &id) const{
	std::lock_guard<std::mutex> guard(lock_);
	if (threads_.empty())
		return nullptr;

	for (auto &info : threads_){
		if (info.second->get_id() == id)
			return info.second;
	}

	return nullptr;
}

DWORD winp::app::object::convert_thread_id_to_local_id(const std::thread::id &value) const{
	if (value == id_)
		return local_id_;

	if (value == std::this_thread::get_id())
		return GetCurrentThreadId();

	auto thread = find_thread(value);
	return ((thread == nullptr) ? 0 : thread->get_local_id());
}

std::thread::id winp::app::object::convert_local_thread_id_to_id(DWORD value) const{
	if (value == local_id_)
		return id_;

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

winp::thread::object *winp::app::object::get_current_thread(){
	return current_thread_;
}

void winp::app::object::add_thread_(thread::object &thread){
	std::lock_guard<std::mutex> guard(lock_);
	if (threads_.find(thread.local_id_) == threads_.end())
		threads_[thread.local_id_] = &thread;
	else//Multiple threads
		throw utility::error_code::multiple_app_threads;
}

void winp::app::object::remove_thread_(DWORD id){
	std::lock_guard<std::mutex> guard(lock_);
	if (!threads_.empty())
		threads_.erase(id);
}

thread_local winp::thread::object *winp::app::object::current_thread_ = nullptr;

winp::app::main_object::main_object(){
	{//Scoped lock
		std::lock_guard<std::mutex> guard(collection::lock_);
		if (collection::main_ == nullptr)
			collection::main_ = this;
		else
			throw utility::error_code::multiple_main_apps;
	}

	if ((main_thread_ = collection::get_current_thread()) == nullptr)
		main_thread_ = (thread_ = std::make_shared<thread::object>(*this)).get();
}

winp::thread::object &winp::app::main_object::get_thread(){
	return *main_thread_;
}
