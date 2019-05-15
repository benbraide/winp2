#pragma once

#include "../utility/random_string_generator.h"

#include "../thread/thread_object.h"

#define WINP_CLASS_UUID		 "{DABED3E8-D8A5-48FC-B80B-B17C167FA9B0}"
#define WINP_CLASS_WUUID	L"{DABED3E8-D8A5-48FC-B80B-B17C167FA9B0}"

namespace winp::app{
	class object{
	public:
		static const WNDCLASSEXW &get_class_info();

		static const std::wstring &get_class_name();

		static int run();

		static void stop(int exit_code);

		static thread::object *find_thread(DWORD id);

		static thread::object *find_thread(const std::thread::id &id);

		static thread::object &get_thread();

		static thread::object *get_current_thread();

		static DWORD convert_thread_id_to_local_id(const std::thread::id &value);

		static std::thread::id convert_local_thread_id_to_id(DWORD value);

		static void traverse_threads(const std::function<void(thread::object &)> &callback, DWORD context_id = 0, int task_priority = thread::queue::urgent_task_priority, unsigned __int64 task_id = 0u);

		static void traverse_threads(const std::function<void(thread::object &)> &callback, thread::object &context, int task_priority = thread::queue::urgent_task_priority, unsigned __int64 task_id = 0u);

	protected:
		friend class thread::object;

		class app_initializer_class{
		public:
			app_initializer_class();
		};

		class thread_scope_class{
		public:
			thread_scope_class();

			~thread_scope_class();
		};

		static std::wstring class_name_;
		static WNDCLASSEXW class_info_;

		static std::unordered_map<DWORD, std::shared_ptr<thread::object>> threads_;
		static thread_local thread::object *current_thread_;

		static app_initializer_class app_initializer_;
		static thread_local thread_scope_class thread_scope_;

		static std::mutex lock_;
	};
}
