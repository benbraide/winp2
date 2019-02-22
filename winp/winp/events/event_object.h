#pragma once

#include "../thread/thread_queue.h"

namespace winp::thread{
	class item;
}

namespace winp::events{
	class object{
	public:
		explicit object(thread::item &target, const std::function<void(object &)> &default_handler = nullptr);

		object(thread::item &target, thread::item &context, const std::function<void(object &)> &default_handler = nullptr);

		virtual ~object();

		object(const object &) = delete;

		object &operator =(const object &) = delete;

		virtual thread::object &get_thread();

		virtual thread::item &get_target();

		virtual thread::item &get_context();

		virtual void prevent_default();

		virtual bool do_default();

		virtual void stop_propagation();

		virtual unsigned int get_states() const;

		virtual bool is_thread_context() const;

		static constexpr unsigned int state_nil							= (0 << 0x0000);
		static constexpr unsigned int state_default_prevented			= (1 << 0x0000);
		static constexpr unsigned int state_propagation_stopped			= (1 << 0x0001);
		static constexpr unsigned int state_default_done				= (1 << 0x0002);
		static constexpr unsigned int state_result_set					= (1 << 0x0003);

	protected:
		thread::item &target_;
		thread::item &context_;

		unsigned int states_ = state_nil;
		std::function<void(object &)> default_handler_;
	};

	class object_with_message : public object{
	public:
		template <typename... args_types>
		explicit object_with_message(MSG &message, WNDPROC default_callback, args_types &&... args)
			: object_with_message(message, message, default_callback, args...){}

		template <typename... args_types>
		explicit object_with_message(MSG &message, MSG &original_message, WNDPROC default_callback, args_types &&... args)
			: object(std::forward<args_types>(args)...), message_(message), original_message_(original_message_), default_callback_(default_callback){}

		virtual ~object_with_message();

		virtual bool do_default() override;

		template <typename value_type>
		object_with_message &operator =(const value_type &value){
			if (!is_thread_context())
				throw utility::error_code::outside_thread_context;

			states_ |= state_result_set;
			value_ = value;

			return *this;
		}

		template <typename value_type>
		object_with_message &operator %=(const value_type &value){
			if (!is_thread_context())
				throw utility::error_code::outside_thread_context;

			if ((states_ & state_result_set) == 0u){
				states_ |= state_result_set;
				value_ = value;
			}

			return *this;
		}

		template <typename target_type = LRESULT>
		target_type get_value() const{
			return (target_type)value_;
		}

		virtual const MSG &get_message() const;

		virtual MSG &get_message();

	protected:
		MSG &message_;
		MSG &original_message_;

		WNDPROC default_callback_;
		LRESULT value_ = 0;
	};

	class unhandled_event : public object_with_message{
	public:
		template <typename... args_types>
		explicit unhandled_event(args_types &&... args)
			: object_with_message(std::forward<args_types>(args)...){}
	};
}
