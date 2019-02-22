#pragma once

#include <thread>
#include <functional>

#include "../utility/windows.h"
#include "../utility/error_code.h"
#include "../utility/traits.h"

#include "../events/event_manager.h"

namespace winp::ui{
	class object;
}

namespace winp::thread{
	class object;

	class item{
	public:
		item();

		explicit item(object &thread);

		virtual ~item();

		item(const item &) = delete;

		item &operator =(const item &) = delete;

		virtual void destruct();

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

		template <typename handler_type>
		unsigned __int64 bind_event(const handler_type &handler){
			return events_manager_.bind_(utility::object_to_function_traits::get(handler));
		}

		template <typename event_type>
		unsigned __int64 bind_event(const std::function<void()> &handler){
			return bind_event([handler](event_type &){
				handler();
			});
		}

		virtual void unbind_event(unsigned __int64 id);

		template <typename value_type, typename function_type>
		static value_type pass_return_value_to_callback(const function_type &callback, value_type &&value){
			if (callback)
				callback(std::forward<value_type>(value));
			return value;
		}

		template <typename value_type, typename function_type>
		static value_type &pass_return_ref_value_to_callback(const function_type &callback, value_type *value){
			if (callback)
				callback(*value);
			return *value;
		}

	protected:
		friend class object;

		virtual void destruct_();

		template <typename event_type>
		void trigger_event_with_ref_(event_type &e) const{
			events_manager_.trigger_(e);
		}

		template <typename event_type, typename... args_types>
		void trigger_event_(const std::function<void(events::object &)> &default_handler, args_types &&... args) const{
			event_type e(std::forward<args_types>(args)..., *const_cast<item *>(this), default_handler);
			trigger_event_with_ref_(e);
		}

		object &thread_;
		unsigned __int64 id_;

		std::thread::id scope_thread_id_;
		DWORD local_scope_thread_id_;

		bool destructed_ = false;
		events::manager<item> events_manager_;
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
	};
}
