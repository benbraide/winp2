#pragma once

#include "thread_item.h"

#define WINP_WM_SEND_MESSAGE					(WM_APP + 0x00)
#define WINP_WM_POST_MESSAGE					(WM_APP + 0x01)

#define WINP_WM_GET_BACKGROUND_COLOR			(WM_APP + 0x02)

#define WINP_WM_MENU_ITEM_SELECT				(WM_APP + 0x03)
#define WINP_WM_MENU_ITEM_CHECK					(WM_APP + 0x04)
#define WINP_WM_MENU_ITEM_HIGHLIGHT				(WM_APP + 0x05)

namespace winp::ui{
	class interactive_surface;
	class window_surface;
}

namespace winp::menu{
	class item;
	class object;
}

namespace winp::thread{
	class item_manager{
	public:
		using menu_items_map_type = std::unordered_map<UINT, menu::item *>;

		struct window_cache_info{
			HWND handle;
			ui::window_surface *object;
		};

		explicit item_manager(object &thread);

		const object &get_thread() const;

		object &get_thread();

		bool is_thread_context() const;

		const RECT &get_update_rect() const;

		template <typename... args_types>
		HWND create_window(ui::window_surface &owner, args_types &&... args){
			if (!is_thread_context())
				throw utility::error_code::outside_thread_context;

			window_cache_.handle = nullptr;
			window_cache_.object = &owner;

			return CreateWindowExW(
				std::forward<args_types>(args)...,
				&owner
			);
		}

		HMENU create_menu(menu::object &owner, HWND target);

		bool destroy_menu(HMENU handle, HWND target);

		void add_menu(menu::object &owner);

		void remove_menu(menu::object &owner);

		UINT generate_menu_item_id(menu::item &target, bool is_system_menu, UINT id = 0u, std::size_t max_tries = 0xFFFFu);

		void add_generated_item_id(menu::item &target);

		void remove_generated_item_id(menu::item &target);

	private:
		friend class object;

		explicit item_manager(object &thread, DWORD thread_id);

		menu::item *find_menu_item_(menu::object &menu, UINT id) const;

		bool is_dialog_message_(MSG &msg) const;

		LRESULT handle_thread_message_(HWND handle, UINT message, WPARAM wparam, LPARAM lparam);

		LRESULT dispatch_message_(item &target, MSG &msg);

		LRESULT dispatch_message_(item &target, HWND handle, UINT message, WPARAM wparam, LPARAM lparam);

		LRESULT window_destroyed_(item &target, MSG &msg);

		LRESULT set_cursor_(item &target, MSG &msg);

		LRESULT erase_background_(item &context, item &target, MSG &msg);

		LRESULT paint_(item &context, item &target, MSG &msg);

		LRESULT position_change_(item &target, MSG &msg, bool changing);

		LRESULT mouse_leave_(item &target, MSG &msg);

		LRESULT mouse_move_(item &target, MSG &msg);

		template <typename window_type, typename event_type, typename app_type>
		LRESULT mouse_(item &target, MSG &msg, unsigned int button, bool is_non_client, app_type &app){
			auto window_target = dynamic_cast<window_type *>(&target);
			if (window_target == nullptr)//Window surface required
				return 0;

			return trigger_event_<event_type>(target, button, is_non_client, msg, app.get_class_entry(window_target->get_class_name())).second;
		}

		template <typename window_type, typename event_type, typename app_type>
		LRESULT key_(item &target, MSG &msg, app_type &app){
			auto window_target = dynamic_cast<window_type *>(&target);
			if (window_target == nullptr)//Window surface required
				return 0;

			return trigger_event_<event_type>(target, msg, app.get_class_entry(window_target->get_class_name())).second;
		}

		LRESULT menu_select_(item &target, MSG &msg);

		static bool menu_item_id_is_reserved_(UINT id);

		static HCURSOR get_default_cursor_(const MSG &msg);

		static void track_mouse_leave_(HWND target, UINT flags);

		static void trigger_event_(events::object &e);

		template <typename event_type, typename... args_types>
		static std::pair<unsigned int, LRESULT> trigger_event_(item &context, args_types &&... args){
			return context.trigger_event_<event_type>(args...);
		}

		template <typename event_type, typename... args_types>
		static std::pair<unsigned int, LRESULT> trigger_event_with_target_(item &context, item &target, args_types &&... args){
			return context.trigger_event_with_target_<event_type>(target, args...);
		}

		template <typename event_type, typename value_type, typename... args_types>
		static std::pair<unsigned int, LRESULT> trigger_event_with_value_(item &context, const value_type &value, args_types &&... args){
			return context.trigger_event_with_value_<event_type>(value, args...);
		}

		template <typename event_type, typename value_type, typename... args_types>
		static std::pair<unsigned int, LRESULT> trigger_event_with_target_and_value_(item &context, item &target, const value_type &value, args_types &&... args){
			return context.trigger_event_with_target_and_value_<event_type>(target, value, args...);
		}

		static bool bubble_event_(events::object &e);

		static LRESULT get_result_(const std::pair<unsigned int, LRESULT> &info, LRESULT prevented_result);

		static LRESULT CALLBACK entry_(HWND handle, UINT message, WPARAM wparam, LPARAM lparam);

		static LRESULT CALLBACK hook_entry_(int code, WPARAM wparam, LPARAM lparam);

		object &thread_;
		HHOOK hook_handle_ = nullptr;

		std::unordered_map<HMENU, menu::object *> menus_;
		std::unordered_map<HMENU, menu_items_map_type> menu_items_;

		std::unordered_map<HWND, ui::window_surface *> windows_;
		std::unordered_map<HWND, ui::window_surface *> top_level_windows_;

		window_cache_info window_cache_{};
		RECT update_rect_{};

		item *tracking_mouse_leave_ = nullptr;
	};
}
