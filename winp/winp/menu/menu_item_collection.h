#pragma once

#include "../ui/ui_object_collection.h"

#include "menu_separator.h"
#include "menu_check_item.h"
#include "menu_link_item_with_popup.h"

namespace winp::menu{
	template <class base_type>
	class item_collection : public ui::object_collection<base_type>{
	public:
		using m_base_type = ui::object_collection<base_type>;

		template <typename... args_types>
		explicit item_collection(args_types &&... args)
			: m_base_type(std::forward<args_types>(args)...){}

		virtual ~item_collection() = default;

		using m_base_type::add_object;

		template <typename callback_type, typename... args_types>
		auto add_object(const callback_type &callback, args_types &&... args){
			return base_type::compute_task_inside_thread_context([&]{
				return add_object_(utility::object_to_function_traits::get(callback), std::forward<args_types>(args)...);
			});
		}

/*

		template <typename... args_types>
		action_item *add_action_item(const std::function<ui::add_result_type(action_item &)> &callback, args_types &&... args){
			return m_base_type::compute_task_inside_thread_context([&]{
				return add_action_item_(callback, std::forward<args_types>(args)...);
			});
		}

		template <typename... args_types>
		check_item *add_check_item(const std::function<ui::add_result_type(check_item &)> &callback, args_types &&... args){
			return m_base_type::compute_task_inside_thread_context([&]{
				return add_check_item_(callback, std::forward<args_types>(args)...);
			});
		}

		template <typename... args_types>
		link_item *add_link_item(const std::function<ui::add_result_type(link_item &)> &callback, args_types &&... args){
			return m_base_type::compute_task_inside_thread_context([&]{
				return add_link_item_(callback, std::forward<args_types>(args)...);
			});
		}

		template <typename... args_types>
		link_item_with_popup<item_collection> *add_link_item_with_popup(const std::function<ui::add_result_type(link_item_with_popup<item_collection> &)> &callback, args_types &&... args){
			return m_base_type::compute_task_inside_thread_context([&]{
				return add_link_item_with_popup_(callback, std::forward<args_types>(args)...);
			});
		}

		template <typename... args_types>
		separator *add_separator(const std::function<ui::add_result_type(separator &)> &callback, args_types &&... args){
			return m_base_type::compute_task_inside_thread_context([&]{
				return add_separator_(callback, std::forward<args_types>(args)...);
			});
		}
*/

	protected:
		using m_base_type::add_object_;

		template <typename... args_types>
		action_item *add_object_(const std::function<ui::add_result_type(action_item &)> &callback, args_types &&... args){
			if (dynamic_cast<menu::system_object *>(this) == nullptr)
				return m_base_type::add_object_(callback, std::forward<args_types>(args)...);

			return m_base_type::template add_object_<system_action_item>([&](system_action_item &item){
				return callback(item);
			}, std::forward<args_types>(args)...);
		}

		template <typename... args_types>
		check_item *add_object_(const std::function<ui::add_result_type(check_item &)> &callback, args_types &&... args){
			if (dynamic_cast<menu::system_object *>(this) == nullptr)
				return m_base_type::add_object_(callback, std::forward<args_types>(args)...);

			return m_base_type::template add_object_<system_check_item>([&](system_check_item &item){
				return callback(item);
			}, std::forward<args_types>(args)...);
		}

		template <typename... args_types>
		link_item *add_object_(const std::function<ui::add_result_type(link_item &)> &callback, args_types &&... args){
			if (dynamic_cast<menu::system_object *>(this) == nullptr)
				return m_base_type::add_object_(callback, std::forward<args_types>(args)...);

			return m_base_type::template add_object_<system_link_item>([&](system_link_item &item){
				return callback(item);
			}, std::forward<args_types>(args)...);
		}

		template <template<class> class collection_type, typename... args_types>
		link_item_with_popup<collection_type> *add_object_(const std::function<ui::add_result_type(link_item_with_popup<collection_type> &)> &callback, args_types &&... args){
			if (dynamic_cast<menu::system_object *>(this) == nullptr)
				return m_base_type::add_object_(callback, std::forward<args_types>(args)...);

			return m_base_type::template add_object_<system_link_item_with_popup<collection_type>>([&](system_link_item_with_popup<collection_type> &item){
				return callback(item);
			}, std::forward<args_types>(args)...);
		}

		template <typename... args_types>
		separator *add_object_(const std::function<ui::add_result_type(separator &)> &callback, args_types &&... args){
			if (dynamic_cast<menu::system_object *>(this) == nullptr)
				return m_base_type::add_object_(callback, std::forward<args_types>(args)...);

			return m_base_type::template add_object_<system_separator>([&](system_separator &item){
				return callback(item);
			}, std::forward<args_types>(args)...);
		}
	};
}
