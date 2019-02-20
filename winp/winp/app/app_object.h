#pragma once

#include "../thread/thread_object.h"

namespace winp::app{
	class object{
	public:
		object();

		virtual ~object();

		virtual void traverse_threads(const std::function<void(thread::object &)> &callback);

	protected:
		friend class thread::object;

		virtual void add_thread_(thread::object &thread);

		virtual void remove_thread_(DWORD id);

		std::unordered_map<DWORD, thread::object *> threads_;
		std::mutex lock_;
	};

	class main_object : public object{
	public:
		main_object();

		virtual thread::object &get_thread();

	private:
		thread::object thread_;
	};
}
