#pragma once

#include <bitset>

#include "../utility/windows.h"
#include "../utility/error_code.h"

#include "../thread/thread_queue.h"

namespace winp::thread{
	class item;
}

namespace winp::ui{
	class object;
	class tree;

	template <class base_type>
	class object_collection;
}

namespace winp::menu{
	class popup;
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

		template <typename result_type>
		void set_result(const result_type &result){
			if (!is_thread_context())
				throw utility::error_code::outside_thread_context;

			if ((states_ & state_doing_default) == 0u)
				states_ |= state_result_set;
			else
				states_ |= state_default_result_set;

			set_result_(result);
		}

		template <typename result_type>
		void set_result_if_not_set(const result_type &result){
			if (!is_thread_context())
				throw utility::error_code::outside_thread_context;

			if ((states_ & state_result_set) == 0u){
				if ((states_ & state_doing_default) == 0u)
					states_ |= state_result_set;
				else
					states_ |= state_default_result_set;

				set_result_(result);
			}
		}

		template <typename target_type = LRESULT>
		target_type get_result() const{
			return (target_type)result_;
		}

		virtual void prevent_default();

		virtual bool do_default();

		virtual void stop_propagation();

		virtual unsigned int get_states() const;

		virtual bool is_thread_context() const;

		static constexpr unsigned int state_nil							= (0u << 0x0000);
		static constexpr unsigned int state_default_prevented			= (1u << 0x0000);
		static constexpr unsigned int state_propagation_stopped			= (1u << 0x0001);
		static constexpr unsigned int state_doing_default				= (1u << 0x0002);
		static constexpr unsigned int state_default_done				= (1u << 0x0003);
		static constexpr unsigned int state_result_set					= (1u << 0x0004);
		static constexpr unsigned int state_default_result_set			= (1u << 0x0005);

	protected:
		friend class thread::item;

		template <typename result_type>
		void set_result_(const result_type &result){
			if (!set_result_(typeid(result_type), &result))
				result_ = (LRESULT)result;
		}

		virtual bool set_result_(const std::type_info &type, const void *result){
			return false;
		}

		thread::item &target_;
		thread::item *context_;

		LRESULT result_ = 0;
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
			: object(std::forward<args_types>(args)...), message_(message), original_message_(original_message), default_callback_(default_callback){}

		virtual ~object_with_message();

		virtual bool do_default() override;

		virtual const MSG &get_message() const;

		virtual MSG &get_message();

	protected:
		virtual bool should_call_call_default_() const;

		virtual bool call_default_();

		virtual LRESULT get_called_default_value_();

		MSG &message_;
		MSG &original_message_;
		WNDPROC default_callback_;
	};
}
