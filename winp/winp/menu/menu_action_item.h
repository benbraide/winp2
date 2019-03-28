#pragma once

#include "menu_item.h"

namespace winp::menu{
	class action_item : public item{
	public:
		action_item();

		explicit action_item(thread::object &thread);

		explicit action_item(tree &parent);

		action_item(tree &parent, std::size_t index);

		virtual ~action_item();

		virtual utility::error_code set_text(const std::wstring &value, const std::function<void(action_item &, utility::error_code)> &callback = nullptr);

		virtual const std::wstring &get_text(const std::function<void(const std::wstring &)> &callback = nullptr) const;

	protected:
		virtual utility::error_code create_() override;

		virtual HMENU create_handle_(menu::object &parent) override;

		virtual utility::error_code set_text_(const std::wstring &value);

		std::wstring text_;
	};

	class system_action_item : public action_item{
	public:
		template <typename... args_types>
		explicit system_action_item(args_types &&... args)
			: action_item(std::forward<args_types>(args)...){}

		virtual ~system_action_item() = default;
	};
}
