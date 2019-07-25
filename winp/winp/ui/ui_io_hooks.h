#pragma once

#include "ui_hook.h"

namespace winp::ui{
	class io_hook : public hook{
	public:
		explicit io_hook(object &target);

		virtual ~io_hook();
	};

	class drag_hook : public hook{
	public:
		explicit drag_hook(object &target);

		virtual ~drag_hook();
	};

	class no_drag_position_updated_hook : public hook{
	public:
		explicit no_drag_position_updated_hook(object &target);

		virtual ~no_drag_position_updated_hook();

	protected:
		bool is_dragging_ = false;
	};

	class mouse_hover_hook : public hook{
	public:
		explicit mouse_hover_hook(object &target, const std::chrono::milliseconds &delay = std::chrono::seconds(3));

		virtual ~mouse_hover_hook();

		virtual utility::error_code set_delay(const std::chrono::milliseconds &value, const std::function<void(mouse_hover_hook &, utility::error_code)> &callback = nullptr);

		virtual const std::chrono::milliseconds &get_delay(const std::function<void(const std::chrono::milliseconds &)> &callback = nullptr) const;

	protected:
		virtual utility::error_code set_delay_(const std::chrono::milliseconds &value);

		virtual void bind_timer_();

		virtual void remove_hover_();

		std::chrono::milliseconds delay_;
		unsigned __int64 state_ = 0u;
		bool is_hovered_ = false;
	};

	class auto_hide_cursor_hook : public hook{
	public:
		explicit auto_hide_cursor_hook(object &target, const std::chrono::milliseconds &delay = std::chrono::seconds(3));

		virtual ~auto_hide_cursor_hook();

		virtual utility::error_code set_delay(const std::chrono::milliseconds &value, const std::function<void(auto_hide_cursor_hook &, utility::error_code)> &callback = nullptr);

		virtual const std::chrono::milliseconds &get_delay(const std::function<void(const std::chrono::milliseconds &)> &callback = nullptr) const;

	protected:
		virtual utility::error_code set_delay_(const std::chrono::milliseconds &value);

		std::chrono::milliseconds delay_;
		bool is_hidden_ = false;
	};

	class fullscreen_hook : public hook{
	public:
		struct saved_info{
			RECT dimension;
			DWORD styles;
			DWORD extended_styles;
			bool is_maximized;
		};

		explicit fullscreen_hook(object &target);

		virtual ~fullscreen_hook();

	protected:
		virtual void enter_fullscreen_();

		virtual void escape_fullscreen_();

		virtual void toggle_fullscreen_();

		bool is_fullscreen_ = false;
		saved_info saved_{};
	};

	class system_menu_as_context_menu : public hook{
	public:
		explicit system_menu_as_context_menu(object &target);

		virtual ~system_menu_as_context_menu();
	};
}