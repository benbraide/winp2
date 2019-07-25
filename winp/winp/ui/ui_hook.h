#pragma once

#include <any>
#include <variant>

#include "../thread/thread_item.h"

namespace winp::ui{
	class object;

	class hook : public thread::item{
	public:
		explicit hook(object &target);

		virtual ~hook();

	protected:
		object &target_;
	};
}
