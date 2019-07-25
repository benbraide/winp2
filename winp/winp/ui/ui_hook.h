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
		friend class object;

		virtual bool setup_();

		object &target_;
	};

	template <class target_type>
	class typed_hook : public hook{
	public:
		explicit typed_hook(object &target)
			: hook(target), typed_target_(dynamic_cast<target_type *>(&target)){}

		virtual ~typed_hook() = default;

	protected:
		virtual bool setup_() override{
			return (hook::setup_() && typed_target_ != nullptr);
		}

		target_type *typed_target_;
	};
}
