#pragma once

#include "thread_item.h"

#define WINP_WM_SEND_MESSAGE					(WM_APP + 0x00)
#define WINP_WM_POST_MESSAGE					(WM_APP + 0x01)

#define WINP_WM_GET_BACKGROUND_COLOR			(WM_APP + 0x02)

#define WINP_WM_NCMOUSEENTER					(WM_APP + 0x10)
#define WINP_WM_MOUSEENTER						(WM_APP + 0x11)

#define WINP_WM_MOUSEDRAG						(WM_APP + 0x12)
#define WINP_WM_MOUSEDRAGBEGIN					(WM_APP + 0x13)
#define WINP_WM_MOUSEDRAGEND					(WM_APP + 0x14)

#define WINP_WM_MOUSEDOWN						(WM_APP + 0x15)
#define WINP_WM_MOUSEUP							(WM_APP + 0x16)
#define WINP_WM_MOUSEDBLCLK						(WM_APP + 0x17)

namespace winp::ui{
	class interactive_surface;
	class window_surface;
}

namespace winp::thread{
	class item_manager{
	public:
		struct window_cache_info{
			HWND handle;
			ui::window_surface *object;
		};

		struct mouse_info{
			ui::interactive_surface *target;
			ui::interactive_surface *dragging;
			unsigned int button_down;
			POINT last_position;
			POINT down_position;
			SIZE drag_threshold;
		};

		explicit item_manager(object &thread);

		const object &get_thread() const;

		object &get_thread();

		bool is_thread_context() const;

		const RECT &get_update_rect() const;

		const POINT &get_last_mouse_position() const;

		const POINT &get_mouse_down_position() const;

		unsigned int get_mouse_button_down() const;

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

	private:
		friend class object;

		explicit item_manager(object &thread, DWORD thread_id);

		bool is_dialog_message_(MSG &msg) const;

		LRESULT handle_thread_message_(HWND handle, UINT message, WPARAM wparam, LPARAM lparam);

		LRESULT dispatch_message_(item &target, MSG &msg);

		LRESULT dispatch_message_(item &target, HWND handle, UINT message, WPARAM wparam, LPARAM lparam);

		LRESULT window_destroyed_(item &target, MSG &msg);

		LRESULT set_cursor_(item &target, MSG &msg);

		LRESULT erase_background_(item &context, item &target, MSG &msg);

		LRESULT paint_(item &context, item &target, MSG &msg);

		LRESULT position_change_(item &target, MSG &msg, bool changing);

		LRESULT mouse_leave_(item &context, MSG &msg, DWORD position);

		LRESULT mouse_move_(item &context, MSG &msg, DWORD position);

		LRESULT mouse_down_(item &context, MSG &msg, DWORD position, unsigned int button, bool is_non_client);

		LRESULT mouse_up_(item &context, MSG &msg, DWORD position, unsigned int button, bool is_non_client);

		LRESULT mouse_dbl_clk_(item &context, MSG &msg, DWORD position, unsigned int button, bool is_non_client);

		LRESULT mouse_wheel_(item &context, MSG &msg, DWORD position);

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

		std::unordered_map<HWND, ui::window_surface *> windows_;
		std::unordered_map<HWND, ui::window_surface *> top_level_windows_;

		window_cache_info window_cache_{};
		mouse_info mouse_{};

		RECT update_rect_{};
	};
}
