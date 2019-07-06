#pragma once

#include "control_object.h"

namespace winp::control{
	class radio_group : public ui::tree{
	public:
		radio_group();

		explicit radio_group(tree &parent);

		radio_group(tree &parent, std::size_t index);

		virtual ~radio_group();

	protected:
		virtual utility::error_code insert_child_(object &child, std::size_t index) override;
	};
}
