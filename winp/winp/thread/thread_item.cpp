#include "../ui/ui_tree.h"
#include "../app/app_object.h"

winp::thread::item::item()
	: thread_(app::object::get_thread()), id_(reinterpret_cast<unsigned __int64>(this)){
	thread_.add_item_(*this);
}

winp::thread::item::~item(){
	destruct_();
}

winp::utility::error_code winp::thread::item::destruct(const std::function<void(item &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, destruct_());
	}, (callback != nullptr), utility::error_code::nil);
}

bool winp::thread::item::is_destructed(const std::function<void(bool)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, is_destructed_());
	}, (callback != nullptr), false);
}

const winp::thread::object &winp::thread::item::get_thread() const{
	return thread_;
}

winp::thread::object &winp::thread::item::get_thread(){
	return thread_;
}

const winp::thread::queue &winp::thread::item::get_thread_queue() const{
	return thread_.queue_;
}

winp::thread::queue &winp::thread::item::get_thread_queue(){
	return thread_.queue_;
}

unsigned __int64 winp::thread::item::get_id(const std::function<void(unsigned __int64)> &callback) const{
	return id_;
}

HTHEME winp::thread::item::get_theme(const std::function<void(HTHEME)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, get_theme_());
	}, (callback != nullptr), nullptr);
}

std::pair<HDC, HWND> winp::thread::item::get_device_context(const std::function<void(const std::pair<HDC, HWND> &)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, get_device_context_());
	}, (callback != nullptr), std::make_pair<HDC, HWND>(nullptr, nullptr));
}

bool winp::thread::item::is_thread_context() const{
	return thread_.is_thread_context();
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
	if (id_ == 0u)
		return utility::error_code::nil;

	before_destruct_();
	trigger_event_<events::destruct>();

	thread_.queue_.add_id_to_black_list(id_);
	thread_.remove_item_(*this);

	id_ = 0u;
	after_destruct_();

	return utility::error_code::nil;
}

void winp::thread::item::before_destruct_(){}

void winp::thread::item::after_destruct_(){}

bool winp::thread::item::is_destructed_() const{
	return (id_ == 0u);
}

HTHEME winp::thread::item::get_theme_() const{
	return nullptr;
}

std::pair<HDC, HWND> winp::thread::item::get_device_context_() const{
	return std::make_pair<HDC, HWND>(nullptr, nullptr);
}

bool winp::thread::item::event_is_supported_(event_manager_type::key_type key) const{
	return true;
}

void winp::thread::item::unbind_outbound_events_(item *target){
	unbind_outbound_events_(target, static_cast<event_manager_type::key_type>(0));
}

void winp::thread::item::unbind_outbound_events_(item *target, event_manager_type::key_type key){
	if (thread_.items_.empty())
		return;

	auto it = thread_.items_.find(id_);
	if (it == thread_.items_.end())
		return;

	if (target != nullptr || key != static_cast<event_manager_type::key_type>(0)){//Unbind events bound to target or specific to key type
		std::list<unsigned __int64> event_ids;
		for (auto &out_info : it->second.outbound_events_){
			if ((target == nullptr || out_info.second.target == target) && (key == static_cast<event_manager_type::key_type>(0) || key == out_info.second.key))
				event_ids.push_back(out_info.first);
		}

		for (auto event_id : event_ids)
			target->events_manager_.unbind_(event_id);
	}
	else{//Unbind all events
		auto outbound_events = std::move(it->second.outbound_events_);
		for (auto &info : outbound_events)//Unbind all outgoing events
			info.second.target->events_manager_.unbind_(info.first);
	}
}

bool winp::thread::item::adding_event_handler_(event_manager_type &manager, event_manager_type::key_type key, item *owner) const{
	return true;
}

void winp::thread::item::added_event_handler_(event_manager_type &manager, event_manager_type::key_type key, unsigned __int64 id, item *owner) const{
	if (owner != nullptr && &manager == &events_manager_){
		if (auto it = thread_.items_.find(owner->id_); it != thread_.items_.end())
			it->second.outbound_events_[id] = object::outbound_event_info{ const_cast<item *>(this), key };
	}

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
		std::pair<unsigned int, LRESULT> result_info{};
		if (key == event_manager_type::get_key<events::timer>())
			result_info = trigger_single_event_<events::timer>(id, false);
		else//Interval
			result_info = trigger_single_event_<events::interval>(id, false);

		if ((result_info.first & events::object::state_unbind_on_exit) != 0u)
			thread_.remove_timer_(id);
	}, id);
}

void winp::thread::item::removed_event_handler_(event_manager_type &manager, event_manager_type::key_type key, unsigned __int64 id) const{
	if (key == event_manager_type::get_key<events::timer>() || key == event_manager_type::get_key<events::interval>())
		thread_.remove_timer_(id);

	if (&manager != &events_manager_ || thread_.items_.empty())
		return;

	for (auto &item_info : thread_.items_){//Erase incoming event reference for ID
		auto out_it = item_info.second.outbound_events_.find(id);
		if (out_it != item_info.second.outbound_events_.end()){
			item_info.second.outbound_events_.erase(out_it);
			break;
		}
	}
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
