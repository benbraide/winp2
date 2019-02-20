#pragma once

#include "app_object.h"

namespace winp::app{
	class collection{
	public:
		static main_object *get_main();

		static void traverse_objects(const std::function<void(object &)> &callback);

	private:
		friend class object;
		friend class main_object;

		static void add_(object &item);

		static void remove_(object &item);

		static std::unordered_map<object *, char> list_;
		static main_object *main_;
		static std::mutex lock_;
	};
}
