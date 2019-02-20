#include "thread_object.h"

winp::thread::queue::queue(object &thread)
	: thread_(thread){}

void winp::thread::queue::add_id_to_black_list(unsigned __int64 id){
	if (thread_.is_thread_context())
		black_list_[id] = '\0';
	else
		throw utility::error_code::outside_thread_context;
}

void winp::thread::queue::remove_id_from_black_list(unsigned __int64 id){
	if (thread_.is_thread_context())
		black_list_.erase(id);
	else
		throw utility::error_code::outside_thread_context;
}

bool winp::thread::queue::id_is_black_listed(unsigned __int64 id) const{
	if (!thread_.is_thread_context())
		throw utility::error_code::outside_thread_context;
	return (black_list_.empty() || black_list_.find(id) != black_list_.end());
}

void winp::thread::queue::post_task(const callback_type &task, int priority, unsigned __int64 id){
	add_([&]{
		is_executing_ = true;
		if (!id_is_black_listed(id) && task != nullptr)
			task();

		is_executing_ = false;
	}, priority);
}

void winp::thread::queue::execute_task(const callback_type &task, int priority, unsigned __int64 id, bool always_queue){
	if (!always_queue && thread_.is_thread_context()){
		is_executing_ = true;
		task();

		is_executing_ = false;
		return;
	}

	std::promise<void> promise;
	add_([&]{
		is_executing_ = true;
		if (!id_is_black_listed(id) && task != nullptr)
			task();

		is_executing_ = false;
		promise.set_value();
	}, priority);

	promise.get_future().get();
}

bool winp::thread::queue::is_thread_context() const{
	return thread_.is_thread_context();
}

winp::thread::object &winp::thread::queue::get_thread(){
	return thread_;
}

const winp::thread::object &winp::thread::queue::get_thread() const{
	return thread_;
}

void winp::thread::queue::add_(const callback_type &task, int priority){
	std::lock_guard<std::mutex> guard(lock_);
	tasks_[priority].push_back(task);
	thread_.post_message(WM_NULL);//Release loop if idle
}

bool winp::thread::queue::run_next_(){
	return run_next_(0, condition_type::nil, false);
}

bool winp::thread::queue::run_next_(int priority){
	return run_next_(priority, condition_type::equal, false);
}

bool winp::thread::queue::run_next_(int priority, condition_type condition, bool invert){
	if (is_executing_)
		return false;

	if (auto task = get_next_(priority, condition, invert); task != nullptr){
		task();
		return true;
	}

	return false;
}

void winp::thread::queue::run_all_(int priority){
	std::list<callback_type> tasks;
	get_all_(priority, tasks);
	for (auto task : tasks)
		task();
}

winp::thread::queue::callback_type winp::thread::queue::get_next_(){
	return get_next_(0, condition_type::nil, false);
}

winp::thread::queue::callback_type winp::thread::queue::get_next_(int priority){
	return get_next_(priority, condition_type::equal, false);
}

winp::thread::queue::callback_type winp::thread::queue::get_next_(int priority, condition_type condition, bool invert){
	std::lock_guard<std::mutex> guard(lock_);
	if (tasks_.empty())
		return nullptr;
	
	auto it = tasks_.begin();
	switch (condition){
	case condition_type::less:
		if ((it->first < priority) == invert)
			return nullptr;
		break;
	case condition_type::less_or_equal:
		if ((it->first <= priority) == invert)
			return nullptr;
		break;
	case condition_type::equal:
		if ((it->first == priority) == invert)
			return nullptr;
		break;
	case condition_type::greater_or_equal:
		if ((it->first >= priority) == invert)
			return nullptr;
		break;
	case condition_type::greater:
		if ((it->first > priority) == invert)
			return nullptr;
		break;
	default:
		break;
	}

	callback_type task;
	if (!it->second.empty()){
		task = *it->second.begin();
		if (it->second.size() == 1u)
			tasks_.erase(it);
		else//Multiple entries
			it->second.erase(it->second.begin());
	}
	else//Empty list
		tasks_.erase(tasks_.begin());

	return task;
}

void winp::thread::queue::get_all_(int priority, std::list<callback_type> &tasks){
	std::lock_guard<std::mutex> guard(lock_);
	if (tasks_.empty())
		return;

	auto it = tasks_.find(priority);
	if (it == tasks_.end())
		return;//List not found

	tasks = std::move(it->second);
	tasks_.erase(it);
}
