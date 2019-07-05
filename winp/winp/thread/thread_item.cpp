#include "../ui/ui_tree.h"
#include "../app/app_object.h"

winp::thread::item::item()
	: item(app::object::get_thread()){}

winp::thread::item::item(object &thread)
	: thread_(thread), id_(reinterpret_cast<unsigned __int64>(this)), scope_thread_id_(std::this_thread::get_id()), local_scope_thread_id_(GetCurrentThreadId()){
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

const winp::thread::item::event_manager_type &winp::thread::item::events() const{
	return events_manager_;
}

winp::thread::item::event_manager_type &winp::thread::item::events(){
	return events_manager_;
}

winp::utility::error_code winp::thread::item::destruct_(){
	if (!thread_.is_thread_context())
		return utility::error_code::outside_thread_context;

	thread_.get_queue().add_id_to_black_list(id_);
	thread_.remove_item_(id_);

	return utility::error_code::nil;
}

bool winp::thread::item::event_is_supported_(event_manager_type::key_type key) const{
	return true;
}

bool winp::thread::item::adding_event_handler_(event_manager_type &manager, event_manager_type::key_type key) const{
	return true;
}

void winp::thread::item::added_event_handler_(event_manager_type &manager, event_manager_type::key_type key, unsigned __int64 id) const{
	std::pair<unsigned int, LRESULT> result_info{};
	if (key == event_manager_type::get_key<events::timer>())
		result_info = trigger_single_event_<events::timer>(id, true);
	else if (key == event_manager_type::get_key<events::interval>())
		result_info = trigger_single_event_<events::interval>(id, true);
	else
		return;

	if ((result_info.first & events::object::state_default_prevented) != 0u)
		return;

	thread_.add_timer_(std::chrono::milliseconds(result_info.second), [=]{
		if (key == event_manager_type::get_key<events::timer>()){
			trigger_single_event_<events::timer>(id, false);
			thread_.remove_timer_(id);
		}
		else
			trigger_single_event_<events::interval>(id, false);
	}, id);
}

void winp::thread::item::removed_event_handler_(event_manager_type &manager, event_manager_type::key_type key, unsigned __int64 id) const{
	if (key == event_manager_type::get_key<events::timer>() || key == event_manager_type::get_key<events::interval>())
		thread_.remove_timer_(id);
}

void winp::thread::item::trigger_event_handler_(events::object &e) const{
	e.states_ |= events::object::state_default_done;
	event_handlers_.trigger_(e, 0u);
}

void winp::thread::item::trigger_event_(events::object &e, unsigned __int64 id) const{
	events_manager_.trigger_(e, id);
	e.do_default();
}

void winp::thread::item::trigger_event_of_(item &owner, events::object &e, unsigned __int64 id) const{
	owner.trigger_event_(e, id);
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
