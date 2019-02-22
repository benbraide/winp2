#pragma once

#include "../utility/random_string_generator.h"

#include "app_object.h"

namespace winp::app{
	class collection{
	public:
		static main_object *get_main();

		static object *find_app(DWORD id);

		static object *find_app(const std::thread::id &id);

		static thread::object *find_thread(DWORD id);

		static thread::object *find_thread(const std::thread::id &id);

		static object *get_current_thread_app();

		static thread::object *get_current_thread();

		static DWORD convert_thread_id_to_local_id(const std::thread::id &value);

		static std::thread::id convert_local_thread_id_to_id(DWORD value);

		static void traverse_objects(const std::function<void(object &)> &callback, DWORD context_id = 0, int task_priority = thread::queue::urgent_task_priority, unsigned __int64 task_id = 0u);

		static void traverse_objects(const std::function<void(object &)> &callback, thread::object &context, int task_priority = thread::queue::urgent_task_priority, unsigned __int64 task_id = 0u);

		static void traverse_threads(const std::function<void(thread::object &)> &callback, DWORD context_id = 0, int task_priority = thread::queue::urgent_task_priority, unsigned __int64 task_id = 0u);

		static void traverse_threads(const std::function<void(thread::object &)> &callback, thread::object &context, int task_priority = thread::queue::urgent_task_priority, unsigned __int64 task_id = 0u);

		static void enable_automatic_thread_creation();

	private:
		friend class object;
		friend class main_object;

		class automatic_thread_creation_class{
		public:
			automatic_thread_creation_class();

			~automatic_thread_creation_class();

		private:
			DWORD thread_id_;
		};

		static void add_(object &item);

		static void remove_(DWORD id);

		static thread::object *unsafe_find_thread_(DWORD id);

		static DWORD create_thread_();

		static void destroy_thread_(DWORD id);

		static std::unordered_map<DWORD, std::shared_ptr<thread::object>> created_threads_;
		static std::shared_ptr<main_object> created_main_object_;

		static std::unordered_map<DWORD, object *> apps_;
		static utility::random_wide_string_generator random_wide_string_generator_;

		static main_object *main_;
		static std::mutex lock_;

		static bool automatic_thread_creation_enabled_;
		static thread_local automatic_thread_creation_class automatic_thread_creation_;
	};
}
