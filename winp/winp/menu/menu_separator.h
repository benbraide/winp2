#pragma once

#include "menu_item.h"

namespace winp::menu{
	class separator : public item{
	public:
		separator();

		explicit separator(thread::object &thread);

		explicit separator(tree &parent);

		separator(tree &parent, std::size_t index);

		virtual ~separator();

	protected:
		virtual utility::error_code create_() override;

		virtual HMENU create_handle_(menu::object &parent) override;

		virtual UINT get_filtered_states_() const override;

		virtual UINT get_types_() const override;
	};
}
