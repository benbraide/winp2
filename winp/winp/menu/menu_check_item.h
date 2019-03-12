#pragma once

#include "menu_action_item.h"

namespace winp::menu{
	class check_item : public action_item{
	public:
		check_item();

		explicit check_item(thread::object &thread);

		explicit check_item(tree &parent);

		check_item(tree &parent, std::size_t index);

		virtual ~check_item();

		virtual utility::error_code set_checked_bitmap(HBITMAP value, const std::function<void(item &, utility::error_code)> &callback = nullptr);

		virtual HBITMAP get_checked_bitmap(const std::function<void(HBITMAP)> &callback = nullptr) const;

		virtual utility::error_code set_unchecked_bitmap(HBITMAP value, const std::function<void(item &, utility::error_code)> &callback = nullptr);

		virtual HBITMAP get_unchecked_bitmap(const std::function<void(HBITMAP)> &callback = nullptr) const;

		virtual utility::error_code set_checked_state(bool is_checked, const std::function<void(item &, utility::error_code)> &callback = nullptr);

		virtual bool is_checked(const std::function<void(bool)> &callback = nullptr) const;

	protected:
		virtual UINT get_filtered_states_() const override;

		virtual UINT get_types_() const override;

		virtual utility::error_code set_checked_bitmap_(HBITMAP value);

		virtual utility::error_code set_unchecked_bitmap_(HBITMAP value);

		virtual utility::error_code set_checked_state_(bool is_checked);

		HBITMAP checked_bitmap_ = nullptr;
		HBITMAP unchecked_bitmap_ = nullptr;
	};
}
