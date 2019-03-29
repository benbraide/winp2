#pragma once

#include "push_button_control.h"

namespace winp::control{
	class split_button : public push_button{
	public:
		split_button();

		explicit split_button(thread::object &thread);

		explicit split_button(tree &parent);

		split_button(tree &parent, std::size_t index);

		virtual ~split_button();

		virtual LRESULT dispatch_notification(MSG &msg) const override;

	protected:
		virtual DWORD get_persistent_styles_(bool is_extended) const override;

		virtual HMENU get_context_menu_handle_(events::get_context_menu_handle &e) const override;

		virtual POINT get_context_menu_position_() const override;

		virtual SIZE compute_additional_size_() const override;
	};

	class default_split_button : public split_button{
	public:
		default_split_button();

		explicit default_split_button(thread::object &thread);

		explicit default_split_button(tree &parent);

		default_split_button(tree &parent, std::size_t index);

		virtual ~default_split_button();

	protected:
		virtual DWORD get_persistent_styles_(bool is_extended) const override;
	};
}
