#pragma once

#include "menu_item.h"

namespace winp::menu{
	class action_item : public item{
	public:
		action_item();

		explicit action_item(ui::tree &parent);

		action_item(ui::tree &parent, std::size_t index);

		virtual ~action_item();

		virtual utility::error_code set_text(const std::wstring &value, const std::function<void(action_item &, utility::error_code)> &callback = nullptr);

		virtual const std::wstring &get_text(const std::function<void(const std::wstring &)> &callback = nullptr) const;

	protected:
		friend class popup;

		virtual utility::error_code create_() override;

		virtual utility::error_code fill_info_(MENUITEMINFOW &info) override;

		virtual utility::error_code set_text_(const std::wstring &value);

		std::wstring text_;
	};

	class wrapped_action_item : public action_item{
	public:
		virtual ~wrapped_action_item();

	protected:
		friend class wrapped_popup;

		wrapped_action_item(menu::object &parent, std::size_t index);

		virtual utility::error_code create_() override;

		virtual utility::error_code destroy_() override;
	};
}
