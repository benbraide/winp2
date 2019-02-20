#pragma once

#include <thread>
#include <functional>

#include "../utility/windows.h"
#include "../utility/error_code.h"

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

		virtual unsigned __int64 get_id() const;

		std::thread::id get_scope_thread_id() const;

		DWORD get_local_scope_thread_id() const;

		bool is_cross_thread() const;

		void execute_task_inside_thread_context(const std::function<void()> &task);

		void execute_task_inside_thread_context(const std::function<void()> &task, int priority);

	protected:
		friend class object;

		virtual void destruct_();

		object &thread_;
		unsigned __int64 id_;

		std::thread::id scope_thread_id_;
		DWORD local_scope_thread_id_;

		bool destructed_ = false;
	};
}
