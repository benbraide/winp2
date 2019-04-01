#pragma once

#include "push_button_control.h"

namespace winp::control{
	class command_link : public push_button{
	public:
		command_link();

		explicit command_link(thread::object &thread);

		explicit command_link(tree &parent);

		command_link(tree &parent, std::size_t index);

		virtual ~command_link();

		virtual utility::error_code set_note(const std::wstring &value, const std::function<void(command_link &, utility::error_code)> &callback = nullptr);

		virtual const std::wstring &get_note(const std::function<void(const std::wstring &)> &callback = nullptr) const;

	protected:
		virtual utility::error_code create_() override;

		virtual DWORD get_persistent_styles_(bool is_extended) const override;

		virtual SIZE compute_additional_size_() const override;

		virtual utility::error_code set_note_(const std::wstring &value);

		std::wstring note_;
	};
}