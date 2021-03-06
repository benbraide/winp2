#pragma once

#include "../ui/ui_window_surface.h"

namespace winp::control{
	class tab;

	class tab_page : public ui::window_surface{
	public:
		tab_page();

		explicit tab_page(ui::tree &parent);

		tab_page(ui::tree &parent, std::size_t index);

		virtual ~tab_page();

		virtual utility::error_code set_title(const std::wstring &value, const std::function<void(tab_page &, utility::error_code)> &callback = nullptr);

		virtual const std::wstring &get_title(const std::function<void(const std::wstring &)> &callback = nullptr) const;

	protected:
		friend class tab;

		virtual utility::error_code set_parent_value_(ui::tree *value, bool changing) override;

		virtual utility::error_code set_index_value_(std::size_t value, bool changing) override;

		virtual DWORD get_filtered_styles_(bool is_extended) const override;

		virtual utility::error_code insert_into_tab_(tab &tab_parent);

		virtual utility::error_code set_title_(const std::wstring &value);

		virtual LRESULT deactivate_();

		virtual LRESULT activate_();

		virtual int get_insertion_index_(HWND tab_handle) const;

		std::wstring title_;
	};
}
