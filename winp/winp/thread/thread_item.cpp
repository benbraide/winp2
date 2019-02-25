#include "../ui/ui_tree.h"
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

winp::utility::error_code winp::thread::item::destruct(const std::function<void(item &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		if (is_destructed_)
			return utility::error_code::nil;

		is_destructed_ = true;
		auto error_code = destruct_();

		if (error_code == utility::error_code::nil)
			trigger_event_<events::destruct>();

		return pass_return_value_to_callback(callback, *this, error_code);
	}, (callback != nullptr), utility::error_code::nil);
}

bool winp::thread::item::is_destructed(const std::function<void(bool)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, is_destructed_);
	}, (callback != nullptr), false);
}

const winp::thread::object &winp::thread::item::get_thread() const{
	return thread_;
}

winp::thread::object &winp::thread::item::get_thread(){
	return thread_;
}

const winp::thread::queue &winp::thread::item::get_thread_queue() const{
	return thread_.get_queue();
}

winp::thread::queue &winp::thread::item::get_thread_queue(){
	return thread_.get_queue();
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

void winp::thread::item::execute_task_inside_thread_context(const std::function<void()> &task) const{
	execute_task_inside_thread_context(task, queue::urgent_task_priority);
}

void winp::thread::item::execute_task_inside_thread_context(const std::function<void()> &task, int priority) const{
	thread_.get_queue().execute_task(task, priority, id_);
}

void winp::thread::item::execute_or_post_task_inside_thread_context(const std::function<void()> &task, bool post) const{
	execute_or_post_task_inside_thread_context(task, post, thread::queue::urgent_task_priority);
}

void winp::thread::item::execute_or_post_task_inside_thread_context(const std::function<void()> &task, bool post, int priority) const{
	if (post)
		thread_.get_queue().post_task(task, priority, id_);
	else//Execute
		thread_.get_queue().execute_task(task, priority, id_);
}

const winp::events::manager<winp::thread::item> &winp::thread::item::events() const{
	return events_manager_;
}

winp::events::manager<winp::thread::item> &winp::thread::item::events(){
	return events_manager_;
}

void winp::thread::item::unbind_event(unsigned __int64 id){
	events_manager_.unbind(id);
}

winp::utility::error_code winp::thread::item::destruct_(){
	if (!thread_.is_thread_context())
		return utility::error_code::outside_thread_context;

	thread_.get_queue().add_id_to_black_list(id_);
	thread_.remove_item_(id_);

	return utility::error_code::nil;
}

void winp::thread::item::trigger_event_handler_(events::object &e) const{
	e.states_ |= events::object::state_default_done;
	event_handlers_.trigger_(e);
}

void winp::thread::item::trigger_event_(events::object &e) const{
	events_manager_.trigger_(e);
	e.do_default();
}

bool winp::thread::item::bubble_event_(events::object &e){
	auto object_context = dynamic_cast<ui::object *>(e.context_);
	if (object_context == nullptr)
		return false;

	if (auto parent = object_context->get_parent(); parent != nullptr){
		e.context_ = parent;
		e.states_ = events::object::state_nil;
		return true;
	}

	return false;
}

winp::thread::synchronized_item::~synchronized_item() = default;

void winp::thread::synchronized_item::synchronized_item_execute_task_inside_thread_context(const std::function<void()> &task) const{
	if (auto item_self = dynamic_cast<const item *>(this); item_self != nullptr)
		item_self->execute_task_inside_thread_context(task);
	else//This is not an instance of thread::item
		task();
}

void winp::thread::synchronized_item::synchronized_item_execute_task_inside_thread_context(const std::function<void()> &task, int priority) const{
	if (auto item_self = dynamic_cast<const item *>(this); item_self != nullptr)
		item_self->execute_task_inside_thread_context(task, priority);
	else//This is not an instance of thread::item
		task();
}

void winp::thread::synchronized_item::synchronized_item_execute_or_post_task_inside_thread_context(const std::function<void()> &task, bool post) const{
	if (auto item_self = dynamic_cast<const item *>(this); item_self != nullptr)
		item_self->execute_or_post_task_inside_thread_context(task, post);
	else//This is not an instance of thread::item
		task();
}

void winp::thread::synchronized_item::synchronized_item_execute_or_post_task_inside_thread_context(const std::function<void()> &task, bool post, int priority) const{
	if (auto item_self = dynamic_cast<const item *>(this); item_self != nullptr)
		item_self->execute_or_post_task_inside_thread_context(task, post, priority);
	else//This is not an instance of thread::item
		task();
}

void winp::thread::synchronized_item::synchronized_item_trigger_event_(events::object &e) const{
	if (auto item_self = dynamic_cast<const item *>(this); item_self != nullptr)
		item_self->trigger_event_(e);
}
