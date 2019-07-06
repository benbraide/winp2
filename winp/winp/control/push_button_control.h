#pragma once

#include "button_control.h"

namespace winp::control{
	class push_button : public button{
	public:
		push_button();

		explicit push_button(tree &parent);

		push_button(tree &parent, std::size_t index);

		virtual ~push_button();

		virtual utility::error_code click() const;

	protected:
		virtual DWORD get_persistent_styles_(bool is_extended) const override;

		virtual LRESULT dispatch_command_(MSG &msg) const override;
	};

	class default_push_button : public push_button{
	public:
		default_push_button();

		explicit default_push_button(tree &parent);

		default_push_button(tree &parent, std::size_t index);

		virtual ~default_push_button();

	protected:
		virtual DWORD get_persistent_styles_(bool is_extended) const override;
	};
}
