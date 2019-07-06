#pragma once

#include "menu_tree.h"

namespace winp::menu{
	class radio_group : public tree{
	public:
		radio_group();

		explicit radio_group(ui::tree &parent);

		radio_group(ui::tree &parent, std::size_t index);

		virtual ~radio_group();
	protected:
		virtual utility::error_code insert_child_(object &child, std::size_t index) override;

		virtual UINT get_types_(std::size_t index) const override;
	};
}
