#pragma once

#include "menu_item.h"

namespace winp::menu{
	class popup;

	class link_item : public item{
	public:
		explicit link_item(popup &target);

		link_item(popup &target, tree &parent);

		link_item(popup &target, tree &parent, std::size_t index);

		virtual ~link_item();

		virtual const popup &get_target(const std::function<void(const popup &)> &callback = nullptr) const;

		virtual popup &get_target(const std::function<void(const popup &)> &callback = nullptr);

		virtual utility::error_code set_text(const std::wstring &value, const std::function<void(object &, utility::error_code)> &callback = nullptr);

		virtual const std::wstring &get_text(const std::function<void(const std::wstring &)> &callback = nullptr) const;

	protected:
		virtual utility::error_code create_() override;

		virtual HMENU create_handle_(menu::object &parent) override;

		virtual utility::error_code set_text_(const std::wstring &value);

		popup &target_;
		std::wstring text_;
	};
}
