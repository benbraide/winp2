#pragma once

#include <thread>
#include <functional>

#include "../utility/windows.h"
#include "../utility/error_code.h"

#include "../events/event_manager.h"

namespace winp::ui{
	class object;
}

namespace winp::thread{
	class object;
	class synchronized_item;
	class item_manager;

	class item{
	public:
		using event_manager_type = events::manager<item>;

		item();

		explicit item(object &thread);

		virtual ~item();

		item(const item &) = delete;

		item &operator =(const item &) = delete;

		virtual utility::error_code destruct(const std::function<void(item &, utility::error_code)> &callback = nullptr);

		virtual bool is_destructed(const std::function<void(bool)> &callback = nullptr) const;

		virtual const object &get_thread() const;

		virtual object &get_thread();

		virtual const queue &get_thread_queue() const;

		virtual queue &get_thread_queue();

		virtual unsigned __int64 get_id() const;

		virtual std::thread::id get_scope_thread_id() const;

		virtual DWORD get_local_scope_thread_id() const;

		virtual bool is_cross_thread() const;

		virtual void execute_task_inside_thread_context(const std::function<void()> &task) const;

		virtual void execute_task_inside_thread_context(const std::function<void()> &task, int priority) const;

		virtual void execute_or_post_task_inside_thread_context(const std::function<void()> &task, bool post) const;

		virtual void execute_or_post_task_inside_thread_context(const std::function<void()> &task, bool post, int priority) const;

		template <typename function_type>
		auto compute_task_inside_thread_context(const function_type &task) const{
			return get_thread_queue().compute_task(task, queue::urgent_task_priority, id_);
		}

		template <typename function_type>
		auto compute_or_post_task_inside_thread_context(const function_type &task, bool post, decltype(task()) default_value) const{
			if (!post)
				return get_thread_queue().compute_task([&]{ return task(); }, thread::queue::urgent_task_priority, id_);
			
			get_thread_queue().post_task(task, thread::queue::urgent_task_priority, id_);
			return default_value;
		}

		virtual const event_manager_type &events() const;

		virtual event_manager_type &events();

		template <typename value_type, typename function_type>
		static value_type pass_return_value_to_callback(const function_type &callback, value_type &&value){
			if (callback != nullptr)
				callback(std::forward<value_type>(value));
			return value;
		}

		template <typename value_type, typename function_type>
		static value_type &pass_return_ref_value_to_callback(const function_type &callback, value_type *value){
			if (callback != nullptr)
				callback(*value);
			return *value;
		}

		template <typename value_type, typename function_type, typename object_type>
		static value_type pass_return_value_to_callback(const function_type &callback, object_type &&object, value_type &&value){
			if (callback != nullptr)
				callback(std::forward<object_type>(object), std::forward<value_type>(value));
			return value;
		}

		template <typename value_type, typename function_type, typename object_type>
		static value_type &pass_return_ref_value_to_callback(const function_type &callback, object_type &&object, value_type *value){
			if (callback != nullptr)
				callback(std::forward<object_type>(object), *value);
			return *value;
		}

	protected:
		friend class object;
		friend class item_manager;
		friend class synchronized_item;
		friend class events::object;
		friend class events::manager<item>;

		virtual utility::error_code destruct_();

		virtual bool event_is_supported_(event_manager_type::key_type key) const;

		template <typename event_type, typename... others>
		void set_event_state_(unsigned int state, bool combine = true){
			events_manager_.set_state_<event_type, others...>(state, combine);
		}

		template <typename event_type, typename... others>
		void remove_event_state_(unsigned int state){
			events_manager_.remove_state_<event_type, others...>(state);
		}

		virtual bool adding_event_handler_(event_manager_type &manager, event_manager_type::key_type key) const;

		virtual void added_event_handler_(event_manager_type &manager, event_manager_type::key_type key, unsigned __int64 id) const;

		virtual void removed_event_handler_(event_manager_type &manager, event_manager_type::key_type key, unsigned __int64 id) const;

		template <typename handler_type>
		unsigned __int64 add_event_handler_(const handler_type &handler){
			return event_handlers_.bind(handler);
		}

		template <typename handler_type>
		unsigned __int64 add_event_handler_(item &target, const handler_type &handler){
			return target.add_event_handler_(handler);
		}

		virtual void trigger_event_handler_(events::object &e) const;

		virtual void trigger_event_(events::object &e, unsigned __int64 id = 0u) const;

		template <typename event_type, typename... args_types>
		std::pair<unsigned int, LRESULT> trigger_event_(args_types &&... args) const{
			return trigger_event_with_target_and_value_<event_type>(const_cast<item &>(*this), 0, args...);
		}

		template <typename event_type, typename... args_types>
		std::pair<unsigned int, LRESULT> trigger_event_with_target_(item &target, args_types &&... args) const{
			return trigger_event_with_target_and_value_<event_type>(target, 0, args...);
		}

		template <typename event_type, typename value_type, typename... args_types>
		std::pair<unsigned int, LRESULT> trigger_event_with_value_(const value_type &value, args_types &&... args) const{
			return trigger_event_with_target_and_value_<event_type>(const_cast<item &>(*this), value, args...);
		}

		template <typename event_type, typename value_type, typename... args_types>
		std::pair<unsigned int, LRESULT> trigger_event_with_target_and_value_(item &target, const value_type &value, args_types &&... args) const{
			event_type e(std::forward<args_types>(args)..., target, const_cast<item &>(*this));
			trigger_event_(e);

			if ((e.states_ & (events::object::state_result_set | events::object::state_default_result_set)) == 0u)
				return std::make_pair(e.states_, (LRESULT)value);

			return std::make_pair(e.states_, e.result_);
		}

		template <typename event_type, typename... args_types>
		std::pair<unsigned int, LRESULT> trigger_single_event_(unsigned __int64 id, args_types &&... args) const{
			return trigger_single_event_with_target_and_value_<event_type>(id, const_cast<item &>(*this), 0, args...);
		}

		template <typename event_type, typename... args_types>
		std::pair<unsigned int, LRESULT> trigger_single_event_with_target_(unsigned __int64 id, item &target, args_types &&... args) const{
			return trigger_single_event_with_target_and_value_<event_type>(id, target, 0, args...);
		}

		template <typename event_type, typename value_type, typename... args_types>
		std::pair<unsigned int, LRESULT> trigger_single_event_with_value_(unsigned __int64 id, const value_type &value, args_types &&... args) const{
			return trigger_single_event_with_target_and_value_<event_type>(id, const_cast<item &>(*this), value, args...);
		}

		template <typename event_type, typename value_type, typename... args_types>
		std::pair<unsigned int, LRESULT> trigger_single_event_with_target_and_value_(unsigned __int64 id, item &target, const value_type &value, args_types &&... args) const{
			event_type e(std::forward<args_types>(args)..., target, const_cast<item &>(*this));
			trigger_event_(e, id);

			if ((e.states_ & (events::object::state_result_set | events::object::state_default_result_set)) == 0u)
				return std::make_pair(e.states_, (LRESULT)value);

			return std::make_pair(e.states_, e.result_);
		}

		static bool bubble_event_(events::object &e);

		object &thread_;
		unsigned __int64 id_;

		std::thread::id scope_thread_id_;
		DWORD local_scope_thread_id_;

		bool is_destructed_ = false;
		event_manager_type events_manager_{ *this };
		event_manager_type event_handlers_{ *this };
	};

	class synchronized_item{
	public:
		virtual ~synchronized_item();

		virtual void synchronized_item_execute_task_inside_thread_context(const std::function<void()> &task) const;

		virtual void synchronized_item_execute_task_inside_thread_context(const std::function<void()> &task, int priority) const;

		virtual void synchronized_item_execute_or_post_task_inside_thread_context(const std::function<void()> &task, bool post) const;

		virtual void synchronized_item_execute_or_post_task_inside_thread_context(const std::function<void()> &task, bool post, int priority) const;

		template <typename function_type>
		auto synchronized_item_compute_task_inside_thread_context(const function_type &task) const{
			if (auto item_self = dynamic_cast<const item *>(this); item_self != nullptr)
				return item_self->compute_task_inside_thread_context(task);
			return task();
		}

		template <typename function_type>
		auto synchronized_item_compute_or_post_task_inside_thread_context(const function_type &task, bool post, decltype(task()) default_value) const{
			if (auto item_self = dynamic_cast<const item *>(this); item_self != nullptr)
				return item_self->compute_or_post_task_inside_thread_context(task, post, default_value);
			return task();
		}

		template <typename value_type, typename function_type>
		static value_type synchronized_item_pass_return_value_to_callback(const function_type &callback, const value_type &value){
			return item::pass_return_value_to_callback(callback, value);
		}

		template <typename value_type, typename function_type>
		static value_type &synchronized_item_pass_return_ref_value_to_callback(const function_type &callback, value_type *value){
			return item::pass_return_ref_value_to_callback(callback, value);
		}

	protected:
		virtual void synchronized_item_trigger_event_(events::object &e) const;

		template <typename event_type, typename... args_types>
		std::pair<unsigned int, LRESULT> synchronized_item_trigger_event_(args_types &&... args) const{
			if (auto item_self = dynamic_cast<const item *>(this); item_self != nullptr)
				return item_self->trigger_event_<event_type>(args...);
			return std::make_pair<unsigned int, LRESULT>(0u, 0);
		}

		template <typename event_type, typename... args_types>
		std::pair<unsigned int, LRESULT> synchronized_item_trigger_event_with_target_(item &target, args_types &&... args) const{
			if (auto item_self = dynamic_cast<const item *>(this); item_self != nullptr)
				return item_self->trigger_event_with_target_<event_type>(target, args...);
			return std::make_pair<unsigned int, LRESULT>(0u, 0);
		}

		template <typename event_type, typename value_type, typename... args_types>
		std::pair<unsigned int, LRESULT> synchronized_item_trigger_event_with_value_(const value_type &value, args_types &&... args) const{
			if (auto item_self = dynamic_cast<const item *>(this); item_self != nullptr)
				return item_self->trigger_event_with_value_<event_type>(value, args...);
			return std::make_pair<unsigned int, LRESULT>(0u, 0);
		}

		template <typename event_type, typename value_type, typename... args_types>
		std::pair<unsigned int, LRESULT> synchronized_item_trigger_event_with_target_and_value_(item &target, const value_type &value, args_types &&... args) const{
			if (auto item_self = dynamic_cast<const item *>(this); item_self != nullptr)
				return item_self->trigger_event_with_target_and_value_<event_type>(target, value, args...);
			return std::make_pair<unsigned int, LRESULT>(0u, 0);
		}
	};
}
