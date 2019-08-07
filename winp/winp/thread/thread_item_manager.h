#pragma once

#include "../ui/ui_visible_surface.h"

#define WINP_WM_SEND_MESSAGE					(WM_APP + 0x00)
#define WINP_WM_POST_MESSAGE					(WM_APP + 0x01)

#define WINP_WM_GET_BACKGROUND_BRUSH			(WM_APP + 0x02)
#define WINP_WM_GET_BACKGROUND_COLOR			(WM_APP + 0x03)

#define WINP_WM_MENU_ITEM_SELECT				(WM_APP + 0x04)
#define WINP_WM_ITEM_CHECK						(WM_APP + 0x05)
#define WINP_WM_MENU_ITEM_HIGHLIGHT				(WM_APP + 0x06)

#define WINP_WM_GET_CONTEXT_MENU_POSITION		(WM_APP + 0x07)
#define WINP_WM_GET_CONTEXT_MENU_HANDLE			(WM_APP + 0x08)
#define WINP_WM_BLOCK_CONTEXT_MENU				(WM_APP + 0x09)

#define WINP_WM_INIT_MENU_ITEM					(WM_APP + 0x0A)
#define WINP_WM_SPLIT_BUTTON_DROPDOWN			(WM_APP + 0x0B)

#define WINP_WM_TOOLTIP_SHOW					(WM_APP + 0x10)
#define WINP_WM_TOOLTIP_HIDE					(WM_APP + 0x11)

#define WINP_WM_NCMOUSEENTER					(WM_APP + 0x20)
#define WINP_WM_MOUSEENTER						(WM_APP + 0x21)

#define WINP_WM_MOUSEDRAG						(WM_APP + 0x22)
#define WINP_WM_MOUSEDRAGBEGIN					(WM_APP + 0x23)
#define WINP_WM_MOUSEDRAGEND					(WM_APP + 0x24)

#define WINP_WM_MOUSEDOWN						(WM_APP + 0x25)
#define WINP_WM_MOUSEUP							(WM_APP + 0x26)
#define WINP_WM_MOUSEDBLCLK						(WM_APP + 0x27)

namespace winp::ui{
	class window_surface;
}

namespace winp::menu{
	class item;
	class object;
	class popup;
	class wrapped_popup;
}

namespace winp::thread{
	class item_manager{
	public:
		using menu_items_map_type = std::unordered_map<UINT, menu::item *>;

		struct window_cache_info{
			HWND handle;
			ui::window_surface *object;
		};

		struct mouse_info{
			bool full_feature_enabled;
			unsigned int button_down;

			ui::object *target;
			ui::object *dragging;
			ui::object *tracking_leave;
			ui::object *pressed;

			POINT last_position;
			POINT down_position;
			SIZE drag_threshold;
		};

		explicit item_manager(object &thread);

		const object &get_thread() const;

		object &get_thread();

		bool is_thread_context() const;

		HRGN get_update_rgn() const;

		RECT get_update_rect() const;

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

		const mouse_info &get_mouse_state() const;

		static ui::object *find_deepest_mouse_target(ui::object &target, const POINT &mouse_position);

	private:
		friend class object;
		friend class menu::object;
		friend class menu::wrapped_popup;

		class clear_black_listed_menus_on_exit{
		public:
			explicit clear_black_listed_menus_on_exit(item_manager &manager)
				: manager_(manager){}

			~clear_black_listed_menus_on_exit(){
				manager_.clear_black_listed_modified_menus_();
				if (!manager_.black_listed_menus_.empty())
					manager_.black_listed_menus_.clear();
			}

		private:
			item_manager &manager_;
		};

		explicit item_manager(object &thread, DWORD thread_id);

		void add_menu_(menu::object &owner);

		void remove_menu_(menu::object &owner);

		void add_timer_(const std::chrono::milliseconds &duration, const std::function<void()> &callback, unsigned __int64 id);

		void remove_timer_(unsigned __int64 id);

		ui::window_surface *find_window_(HWND handle, bool cache) const;

		bool is_dialog_message_(MSG &msg) const;

		LRESULT handle_thread_message_(HWND handle, UINT message, WPARAM wparam, LPARAM lparam);

		LRESULT dispatch_message_(item &target, MSG &msg);

		LRESULT dispatch_message_(item &target, HWND handle, UINT message, WPARAM wparam, LPARAM lparam);

		LRESULT window_destroyed_(item &target, MSG &msg);

		LRESULT set_cursor_(item &target, MSG &msg);

		LRESULT erase_background_(item &context, item &target, MSG &msg);

		LRESULT paint_(item &context, item &target, MSG &msg);

		LRESULT draw_item_(item &target, MSG &msg);

		LRESULT measure_item_(item &target, MSG &msg);

		LRESULT position_change_(item &target, MSG &msg, bool changing);

		LRESULT style_changing_(item &target, MSG &msg);

		template <typename window_type, typename event_type, typename thread_type>
		LRESULT mouse_message_(item &target, MSG &msg, unsigned int button, bool is_non_client, thread_type &thread){
			if (auto window_target = dynamic_cast<window_type *>(&target); window_target != nullptr)//Window surface required
				return trigger_event_<event_type>(target, button, is_non_client, msg, thread.get_class_entry_(window_target->get_class_name())).second;
			return 0;
		}

		LRESULT mouse_leave_(item &target, MSG &msg, DWORD position);

		LRESULT mouse_move_(item &target, MSG &msg, DWORD position);

		template <typename window_type, typename event_type, typename thread_type>
		LRESULT mouse_button_(item &target, MSG &msg, DWORD position, unsigned int button, bool is_non_client, thread_type &thread, const std::function<void()> &callback){
			auto window_target = dynamic_cast<window_type *>(&target);
			if (window_target == nullptr)//Window surface required
				return trigger_event_<events::unhandled>(target, msg, nullptr).second;

			if (callback != nullptr)
				callback();

			LRESULT result = 0;
			std::pair<unsigned int, LRESULT> result_info;

			auto bubbled_to_target = false;
			for (auto mouse_target = mouse_.target; mouse_target != nullptr; mouse_target = mouse_target->get_parent_()){
				if (dynamic_cast<ui::visible_surface *>(mouse_target) == nullptr || !mouse_target->has_hook_<ui::io_hook>())
					continue;

				if (mouse_target == &target){
					result = (result_info = trigger_event_with_target_<event_type>(*mouse_target, *mouse_.target, button, is_non_client, msg, thread.get_class_entry_(window_target->get_class_name()))).second;
					bubbled_to_target = true;
				}
				else//Ignore result
					result_info = trigger_event_with_target_<event_type>(*mouse_target, *mouse_.target, button, is_non_client, msg, nullptr);

				if ((result_info.first & events::object::state_propagation_stopped) != 0u)
					break;//Propagation stopped
			}

			return (bubbled_to_target ? result : CallWindowProcW(thread.get_class_entry_(window_target->get_class_name()), msg.hwnd, msg.message, msg.wParam, msg.lParam));
		}

		LRESULT mouse_down_(item &target, MSG &msg, DWORD position, unsigned int button, bool is_non_client);

		LRESULT mouse_up_(item &target, MSG &msg, DWORD position, unsigned int button, bool is_non_client);

		LRESULT mouse_dbl_clk_(item &target, MSG &msg, DWORD position, unsigned int button, bool is_non_client);

		LRESULT mouse_wheel_(item &target, MSG &msg, DWORD position);

		LRESULT key_(item &target, MSG &msg);

		template <typename window_type, typename thread_type>
		std::pair<unsigned int, LRESULT> trigger_key_event_(item &context, item &target, bool is_original_target, MSG &msg, thread_type &thread){
			auto window_target = dynamic_cast<window_type *>(&target);
			switch (msg.message){
			case WM_KEYUP:
				return trigger_event_with_target_<events::key_up>(context, target, msg, ((!is_original_target || window_target == nullptr) ? nullptr : thread.get_class_entry_(window_target->get_class_name())));
			case WM_CHAR:
				return trigger_event_with_target_<events::key_press>(context, target, msg, ((!is_original_target || window_target == nullptr) ? nullptr : thread.get_class_entry_(window_target->get_class_name())));
			default:
				break;
			}

			return trigger_event_with_target_<events::key_down>(context, target, msg, ((!is_original_target || window_target == nullptr) ? nullptr : thread.get_class_entry_(window_target->get_class_name())));
		}

		LRESULT set_focus_(item &target, MSG &msg);

		LRESULT kill_focus_(item &target, MSG &msg);

		LRESULT menu_select_(item &target, MSG &msg);

		LRESULT menu_command_(item &target, MSG &msg);

		LRESULT system_command_(item &target, MSG &msg);

		LRESULT context_menu_(item &target, MSG &msg);

		LRESULT menu_init_(item &target, MSG &msg);

		LRESULT menu_uninit_(item &target, MSG &msg);

		LRESULT command_(item &target, MSG &msg);

		LRESULT notify_(item &target, MSG &msg);

		void clear_black_listed_modified_menus_();

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

		static LRESULT get_result_(const std::pair<unsigned int, LRESULT> &info, LRESULT prevented_result);

		static LRESULT CALLBACK entry_(HWND handle, UINT message, WPARAM wparam, LPARAM lparam);

		static LRESULT CALLBACK hook_entry_(int code, WPARAM wparam, LPARAM lparam);

		static void CALLBACK timer_entry_(HWND handle, UINT message, UINT_PTR id, DWORD time);

		object &thread_;
		HHOOK hook_handle_ = nullptr;

		std::shared_ptr<menu::popup> active_context_menu_;
		std::unordered_map<HMENU, menu::object *> menus_;
		std::unordered_map<HMENU, std::shared_ptr<menu::popup>> wrapped_menus_;

		std::list<std::shared_ptr<menu::popup>> black_listed_menus_;
		std::list<menu::popup *> black_listed_modified_menus_;

		std::unordered_map<HWND, ui::window_surface *> windows_;
		std::unordered_map<HWND, ui::window_surface *> top_level_windows_;

		mutable window_cache_info window_cache_{};
		mouse_info mouse_{};

		HDC paint_device_ = nullptr;
		HRGN update_rgn_ = nullptr;

		ui::object *focused_object_ = nullptr;
		std::unordered_map<unsigned __int64, std::function<void()>> timers_;
	};
}
