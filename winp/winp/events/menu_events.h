#pragma once

#include "event_object.h"

namespace winp::events{
	class menu : public object_with_message{
	public:
		template <typename... args_types>
		explicit menu(args_types &&... args)
			: object_with_message(std::forward<args_types>(args)...){}

	protected:
		virtual bool should_call_call_default_() const override;
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
		template <typename... args_types>
		explicit menu_init_item(args_types &&... args)
			: menu(std::forward<args_types>(args)...){}
	};

	class context_menu_base : public object_with_message{
	public:
		template <typename... args_types>
		explicit context_menu_base(args_types &&... args)
			: object_with_message(std::forward<args_types>(args)...){
			position_ = POINT{ GET_X_LPARAM(original_message_.lParam), GET_Y_LPARAM(original_message_.lParam) };
		}

		virtual const POINT &get_position() const;

	protected:
		virtual bool should_call_call_default_() const override;

		POINT position_{};
	};

	class get_context_menu_position : public context_menu_base{
	public:
		template <typename... args_types>
		explicit get_context_menu_position(args_types &&... args)
			: context_menu_base(std::forward<args_types>(args)...){}
	};

	class get_context_menu_handle : public context_menu_base{
	public:
		template <typename... args_types>
		explicit get_context_menu_handle(args_types &&... args)
			: context_menu_base(std::forward<args_types>(args)...){}
	};

	class allow_context_menu : public context_menu_base{
	public:
		template <typename... args_types>
		explicit allow_context_menu(args_types &&... args)
			: context_menu_base(std::forward<args_types>(args)...){}
	};

	class context_menu : public context_menu_base{
	public:
		template <typename... args_types>
		explicit context_menu(args_types &&... args)
			: context_menu_base(std::forward<args_types>(args)...){}

		virtual ui::object_collection<winp::menu::popup> &get_popup() const;
	};
}
