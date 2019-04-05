#pragma once

#include "button_control.h"

namespace winp::control{
	class label : public button{
	public:
		label();

		explicit label(thread::object &thread);

		explicit label(tree &parent);

		label(tree &parent, std::size_t index);

		virtual ~label();

	protected:
		virtual DWORD get_persistent_styles_(bool is_extended) const override;
	};
}
