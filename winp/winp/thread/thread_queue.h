#pragma once

#include <map>
#include <list>
#include <mutex>
#include <thread>
#include <memory>
#include <future>
#include <variant>
#include <functional>
#include <unordered_map>

namespace winp::thread{
	class object;

	class queue{
	public:
		using compare_type = std::greater<>;
		using callback_type = std::function<void()>;

		enum class condition_type{
			nil,
			less,
			less_or_equal,
			equal,
			greater_or_equal,
			greater,
		};

		void post_task(const callback_type &task, int priority, unsigned __int64 id);

		void execute_task(const callback_type &task, int priority, unsigned __int64 id);

		template <typename function_type>
		auto compute_task(const function_type &task, int priority, unsigned __int64 id){
			using return_type = decltype(task());
			if (is_thread_context()){
				is_executing_ = true;
				auto result = task();

				is_executing_ = false;
				return result;
			}

			std::promise<return_type> promise;
			add_([&]{
				is_executing_ = true;
				if (!is_black_listed_(id))
					promise.set_value(task());
				else
					promise.set_value(return_type());

				is_executing_ = false;
			}, priority);

			return promise.get_future().get();
		}

		bool is_thread_context() const;

		object &get_thread();

		static const int default_task_priority			= 0;
		static const int urgent_task_priority			= std::numeric_limits<int>::max();
		static const int elective_task_priority			= std::numeric_limits<int>::min();

		static const unsigned int default_task_id		= 0u;

	private:
		friend class object;

		explicit queue(object &thread);

		void add_to_black_list_(unsigned __int64 id);

		void remove_from_black_list_(unsigned __int64 id);

		bool is_black_listed_(unsigned __int64 id) const;

		void add_(const callback_type &task, int priority);

		bool run_next_();

		bool run_next_(int priority);

		bool run_next_(int priority, condition_type condition, bool invert);

		void run_all_(int priority);

		callback_type get_next_();

		callback_type get_next_(int priority);

		callback_type get_next_(int priority, condition_type condition, bool invert);

		void get_all_(int priority, std::list<callback_type> &tasks);

		object &thread_;
		std::mutex lock_;

		bool is_executing_ = false;
		std::map<int, std::list<callback_type>, compare_type> tasks_;
		std::unordered_map<unsigned __int64, char> black_list_;
	};
}
