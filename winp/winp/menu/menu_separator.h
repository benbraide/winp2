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
		virtual HMENU create_handle_(menu::object &parent) override;

		virtual utility::error_code generate_id_() override;

		virtual UINT get_filtered_states_() const override;

		virtual UINT get_types_() const override;
	};

	class system_separator : public separator{
	public:
		template <typename... args_types>
		explicit system_separator(args_types &&... args)
			: separator(std::forward<args_types>(args)...){}

		virtual ~system_separator() = default;
	};
}
