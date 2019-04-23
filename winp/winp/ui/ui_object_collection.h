#pragma once

#include "ui_object.h"

namespace winp::ui{
	enum class add_result_type{
		nil,
		confirm,
		cancel,
		dont_create,
	};

	template <class base_type>
	class object_collection : public base_type{
	public:
		template <typename... args_types>
		explicit object_collection(args_types &&... args)
			: base_type(std::forward<args_types>(args)...){}

		virtual ~object_collection() = default;

		template <typename callback_type, typename... args_types>
		auto add_object(const callback_type &callback, args_types &&... args){
			return base_type::compute_task_inside_thread_context([&]{
				return add_object_(utility::object_to_function_traits::get(callback), std::forward<args_types>(args)...);
			});
		}

		template <typename object_type, typename... args_types>
		auto add_object_direct(args_types &&... args){
			return base_type::compute_task_inside_thread_context([&]{
				return add_object_<object_type>(nullptr, std::forward<args_types>(args)...);
			});
		}

	protected:
		virtual utility::error_code do_erase_child_(ui::object &child, std::size_t index) override{
			if (auto error_code = base_type::do_erase_child_(child, index); error_code != utility::error_code::nil)
				return error_code;

			if (!objects_.empty())
				objects_.erase(&child);

			return utility::error_code::nil;
		}

		template <typename object_type, typename... args_types>
		object_type *add_object_(const std::function<void(object_type &)> &callback, args_types &&... args){
			auto object = std::make_shared<object_type>(*this, static_cast<std::size_t>(-1), std::forward<args_types>(args)...);
			if (object == nullptr)
				return nullptr;

			if (callback != nullptr){
				callback(*object);
				if (object->is_destructed())
					return nullptr;
			}

			object->create();
			objects_[object.get()] = object;

			return object.get();
		}

		std::unordered_map<thread::item *, std::shared_ptr<ui::object>> objects_;
	};
}
