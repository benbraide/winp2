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

		virtual thread::object &get_thread();

		virtual thread::item &get_target();

		virtual thread::item &get_context();

		virtual void prevent_default();

		virtual void do_default();

		virtual void stop_propagation();

		virtual unsigned int get_states() const;

		virtual bool is_thread_context() const;

		static constexpr unsigned int state_nil							= (0 << 0x0000);
		static constexpr unsigned int state_default_prevented			= (1 << 0x0000);
		static constexpr unsigned int state_propagation_stopped			= (1 << 0x0001);
		static constexpr unsigned int state_default_done				= (1 << 0x0002);

	protected:
		thread::item &target_;
		thread::item &context_;

		unsigned int states_ = state_nil;
		std::function<void(object &)> default_handler_;
	};

	class object_with_message : public object{
	public:
		template <typename... args_types>
		explicit object_with_message(MSG &message, args_types &&... args)
			: object(std::forward<args_types>(args)...), message_(message){}

		virtual ~object_with_message();

		virtual MSG &get_message();

	protected:
		MSG &message_;
	};

	template <class base_type, class value_type>
	class object_with_value : public base_type{
	public:
		using m_base_type = base_type;
		using m_value_type = value_type;

		template <typename... args_types>
		explicit object_with_value(const m_value_type &value, args_types &&... args)
			: base_type(std::forward<args_types>(args)...), value_(value){}

		template <typename... args_types>
		explicit object_with_value(args_types &&... args)
			: base_type(std::forward<args_types>(args)...){}

		virtual object_with_value &operator =(const value_type &value){
			if (!m_base_type::is_thread_context())
				throw utility::error_code::outside_thread_context;

			m_base_type::states_ |= state_result_set;
			value_ = value;

			return *this;
		}

		virtual object_with_value &operator %=(const value_type &value){
			if (!m_base_type::is_thread_context())
				throw utility::error_code::outside_thread_context;

			if ((m_base_type::states_ & state_result_set) == 0u){
				m_base_type::states_ |= state_result_set;
				value_ = value;
			}

			return *this;
		}

		virtual const m_value_type &get_value() const{
			return value_;
		}

		static constexpr unsigned int state_result_set					= (1 << 0x0002);

	protected:
		m_value_type value_ = m_value_type();
	};
}
