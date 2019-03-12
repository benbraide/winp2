#pragma once

#include "../thread/thread_object.h"

#define WINP_CLASS_UUID		 "{DABED3E8-D8A5-48FC-B80B-B17C167FA9B0}"
#define WINP_CLASS_WUUID	L"{DABED3E8-D8A5-48FC-B80B-B17C167FA9B0}"

namespace winp::app{
	class object{
	public:
		object();

		virtual ~object();

		virtual std::thread::id get_id() const;

		virtual DWORD get_local_id() const;

		virtual WNDPROC get_class_entry(const std::wstring &class_name) const;

		virtual const WNDCLASSEXW &get_class_info() const;

		virtual const std::wstring &get_class_name() const;

		virtual thread::object *find_thread(DWORD id) const;

		virtual thread::object *find_thread(const std::thread::id &id) const;

		virtual DWORD convert_thread_id_to_local_id(const std::thread::id &value) const;

		virtual std::thread::id convert_local_thread_id_to_id(DWORD value) const;

		virtual void traverse_threads(const std::function<void(thread::object &)> &callback, DWORD context_id = 0, int task_priority = thread::queue::urgent_task_priority, unsigned __int64 task_id = 0u);

		virtual void traverse_threads(const std::function<void(thread::object &)> &callback, thread::object &context, int task_priority = thread::queue::urgent_task_priority, unsigned __int64 task_id = 0u);

		static thread::object *get_current_thread();

	protected:
		friend class thread::object;

		virtual void add_thread_(thread::object &thread);

		virtual void remove_thread_(DWORD id);

		std::thread::id id_;
		DWORD local_id_;

		std::wstring class_name_;
		WNDCLASSEXW class_info_;

		std::unordered_map<DWORD, thread::object *> threads_;
		mutable std::unordered_map<std::wstring, WNDPROC> class_info_map_;

		mutable std::mutex lock_;

		static thread_local thread::object *current_thread_;
	};

	class main_object : public object{
	public:
		main_object();

		virtual thread::object &get_thread();

	private:
		thread::object *main_thread_ = nullptr;
		std::shared_ptr<thread::object> thread_;
	};
}
