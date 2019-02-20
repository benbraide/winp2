#include "../app/app_collection.h"

winp::thread::item::item()
	: item(app::collection::get_main()->get_thread()){}

winp::thread::item::item(object &thread)
	: thread_(thread), id_(reinterpret_cast<std::size_t>(this)), scope_thread_id_(std::this_thread::get_id()), local_scope_thread_id_(GetCurrentThreadId()){
	thread_.get_queue().execute_task([this]{
		thread_.add_item_(*this);
	}, queue::urgent_task_priority, id_);
}

winp::thread::item::~item(){
	destruct();
}

void winp::thread::item::destruct(){
	thread_.get_queue().execute_task([this]{
		if (!destructed_){
			destructed_ = true;
			destruct_();
		}
	}, queue::urgent_task_priority, id_);
}

const winp::thread::object &winp::thread::item::get_thread() const{
	return thread_;
}

winp::thread::object &winp::thread::item::get_thread(){
	return thread_;
}

unsigned __int64 winp::thread::item::get_id() const{
	return id_;
}

std::thread::id winp::thread::item::get_scope_thread_id() const{
	return scope_thread_id_;
}

DWORD winp::thread::item::get_local_scope_thread_id() const{
	return local_scope_thread_id_;
}

bool winp::thread::item::is_cross_thread() const{
	return (local_scope_thread_id_ != thread_.local_id_);
}

void winp::thread::item::execute_task_inside_thread_context(const std::function<void()> &task){
	execute_task_inside_thread_context(task, queue::urgent_task_priority);
}

void winp::thread::item::execute_task_inside_thread_context(const std::function<void()> &task, int priority){
	thread_.get_queue().execute_task(task, priority, id_);
}

void winp::thread::item::destruct_(){
	if (!thread_.is_thread_context())
		throw utility::error_code::outside_thread_context;

	thread_.get_queue().add_id_to_black_list(id_);
	thread_.remove_item_(id_);
}
