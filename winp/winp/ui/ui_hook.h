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

		template <typename event_type, typename... args_types>
		std::pair<unsigned int, LRESULT> trigger_event_(args_types &&... args) const{
			return static_cast<thread::item &>(target_).trigger_event_<event_type>(std::forward<args_types>(args)...);
		}

		template <typename event_type, typename... args_types>
		std::pair<unsigned int, LRESULT> trigger_event_with_target_(item &target, args_types &&... args) const{
			return static_cast<thread::item &>(target_).trigger_event_with_target_<event_type>(target, std::forward<args_types>(args)...);
		}

		template <typename event_type, typename value_type, typename... args_types>
		std::pair<unsigned int, LRESULT> trigger_event_with_value_(const value_type &value, args_types &&... args) const{
			return static_cast<thread::item &>(target_).trigger_event_with_value_<event_type>(value, std::forward<args_types>(args)...);
		}

		template <typename event_type, typename value_type, typename... args_types>
		std::pair<unsigned int, LRESULT> trigger_event_with_target_and_value_(item &target, const value_type &value, args_types &&... args) const{
			return static_cast<thread::item &>(target_).trigger_event_with_target_and_value_<event_type>(target, value, std::forward<args_types>(args)...);
		}

		template <typename event_type, typename... args_types>
		std::pair<unsigned int, LRESULT> trigger_single_event_(unsigned __int64 id, args_types &&... args) const{
			return static_cast<thread::item &>(target_).trigger_single_event_<event_type>(id, std::forward<args_types>(args)...);
		}

		template <typename event_type, typename... args_types>
		std::pair<unsigned int, LRESULT> trigger_single_event_with_target_(unsigned __int64 id, item &target, args_types &&... args) const{
			return static_cast<thread::item &>(target_).trigger_single_event_with_target_<event_type>(id, target, std::forward<args_types>(args)...);
		}

		template <typename event_type, typename value_type, typename... args_types>
		std::pair<unsigned int, LRESULT> trigger_single_event_with_value_(unsigned __int64 id, const value_type &value, args_types &&... args) const{
			return static_cast<thread::item &>(target_).trigger_single_event_with_value_<event_type>(id, value, std::forward<args_types>(args)...);
		}

		template <typename event_type, typename value_type, typename... args_types>
		std::pair<unsigned int, LRESULT> trigger_single_event_with_target_and_value_(unsigned __int64 id, item &target, const value_type &value, args_types &&... args) const{
			return static_cast<thread::item &>(target_).trigger_single_event_with_target_and_value_<event_type>(id, target, value, std::forward<args_types>(args)...);
		}

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
