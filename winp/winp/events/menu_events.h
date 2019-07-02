#pragma once

#include "event_object.h"

namespace winp::events{
	class menu : public object_with_message{
	public:
		template <typename... args_types>
		explicit menu(args_types &&... args)
			: object_with_message(std::forward<args_types>(args)...){}

	protected:
		virtual bool should_call_default_() const override;
	};

	class menu_init : public menu{
	public:
		template <typename... args_types>
		explicit menu_init(args_types &&... args)
			: menu(std::forward<args_types>(args)...){}
	};

	class menu_uninit : public menu{
	public:
		template <typename... args_types>
		explicit menu_uninit(args_types &&... args)
			: menu(std::forward<args_types>(args)...){}
	};

	class menu_item_highlight : public menu{
	public:
		template <typename... args_types>
		explicit menu_item_highlight(args_types &&... args)
			: menu(std::forward<args_types>(args)...){}
	};

	class menu_item_select : public menu{
	public:
		template <typename... args_types>
		explicit menu_item_select(args_types &&... args)
			: menu(std::forward<args_types>(args)...){}
	};

	class menu_init_item : public menu{
	public:
		enum class enable_type{
			nil,
			enable,
			disable,
		};

		template <typename... args_types>
		explicit menu_init_item(args_types &&... args)
			: menu(std::forward<args_types>(args)...){}
	};

	class context_menu_base : public object_with_message{
	public:
		template <typename... args_types>
		explicit context_menu_base(args_types &&... args)
			: object_with_message(std::forward<args_types>(args)...){
			position_ = POINT{ GET_X_LPARAM(message_info_.lParam), GET_Y_LPARAM(message_info_.lParam) };
		}

		virtual const POINT &get_position() const;

	protected:
		virtual bool should_call_default_() const override;

		POINT position_{};
	};

	class get_context_menu_position : public context_menu_base{
	public:
		template <typename... args_types>
		explicit get_context_menu_position(args_types &&... args)
			: context_menu_base(std::forward<args_types>(args)...){}
	};

	class context_menu : public context_menu_base{
	public:
		template <typename... args_types>
		explicit context_menu(winp::menu::popup &popup, args_types &&... args)
			: context_menu_base(std::forward<args_types>(args)...), popup_(popup){}

		virtual winp::menu::popup &get_popup() const;

	protected:
		winp::menu::popup &popup_;
	};

	class modify_context_menu : public context_menu{
	public:
		template <typename... args_types>
		explicit modify_context_menu(winp::menu::popup &popup, args_types &&... args)
			: context_menu(popup, std::forward<args_types>(args)...){}
	};

	class split_button_menu : public context_menu{
	public:
		template <typename... args_types>
		explicit split_button_menu(winp::menu::popup &popup, args_types &&... args)
			: context_menu(popup, std::forward<args_types>(args)...){}
	};
}
