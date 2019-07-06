#pragma once

#include "push_button_control.h"

namespace winp::control{
	class check : public push_button{
	public:
		check();

		explicit check(tree &parent);

		check(tree &parent, std::size_t index);

		virtual ~check();

		virtual utility::error_code set_checked_state(bool is_checked, const std::function<void(check &, utility::error_code)> &callback = nullptr);

		virtual bool is_checked(const std::function<void(bool)> &callback = nullptr) const;

		virtual bool is_radio(const std::function<void(bool)> &callback = nullptr) const;

	protected:
		virtual DWORD get_persistent_styles_(bool is_extended) const override;

		virtual SIZE compute_additional_size_() const override;

		virtual utility::error_code set_checked_state_(bool is_checked);

		virtual bool is_radio_() const;

		bool is_checked_ = false;
	};

	class three_state_check : public push_button{
	public:
		enum class checked_state_type{
			unchecked,
			checked,
			indeterminate,
		};

		three_state_check();

		explicit three_state_check(tree &parent);

		three_state_check(tree &parent, std::size_t index);

		virtual ~three_state_check();

		virtual utility::error_code set_checked_state(checked_state_type state, const std::function<void(three_state_check &, utility::error_code)> &callback = nullptr);

		virtual checked_state_type get_checked_state(const std::function<void(checked_state_type)> &callback = nullptr) const;

		static checked_state_type get_next_checked_state(checked_state_type state);

	protected:
		virtual DWORD get_persistent_styles_(bool is_extended) const override;

		virtual SIZE compute_additional_size_() const override;

		virtual utility::error_code set_checked_state_(checked_state_type state);

		checked_state_type checked_state_ = checked_state_type::unchecked;
	};
}
