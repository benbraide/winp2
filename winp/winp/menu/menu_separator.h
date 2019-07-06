#pragma once

#include "menu_item.h"

namespace winp::menu{
	class separator : public item{
	public:
		separator();

		explicit separator(ui::tree &parent);

		separator(ui::tree &parent, std::size_t index);

		virtual ~separator();

	protected:
		friend class popup;

		virtual utility::error_code fill_info_(MENUITEMINFOW &info) override;

		virtual UINT get_filtered_states_() const override;

		virtual UINT get_types_() const override;
	};

	class wrapped_separator : public separator{
	public:
		virtual ~wrapped_separator();

	protected:
		friend class wrapped_popup;

		wrapped_separator(menu::object &parent, std::size_t index);

		virtual utility::error_code create_() override;

		virtual utility::error_code destroy_() override;
	};
}
