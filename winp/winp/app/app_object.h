#pragma once

#include "../thread/thread_object.h"

namespace winp::app{
	class main_object;

	class object{
	public:
		static main_object *get_main();

	protected:
		friend class thread::object;

		virtual void add_thread_(thread::object &thread);

		virtual void remove_thread_(DWORD id);

		std::unordered_map<DWORD, thread::object *> threads_;
		static main_object *main_;
	};

	class main_object : public object{
	public:
		main_object();

		virtual thread::object &get_thread();

	private:
		thread::object thread_;
	};
}
