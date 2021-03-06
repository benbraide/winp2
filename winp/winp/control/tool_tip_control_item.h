#pragma once

#include "../ui/ui_window_surface.h"

namespace ui{
	class window_surface;
}

namespace winp::control{
	class tool_tip;

	class tool_tip_item : public ui::object, public ui::surface{
	public:
		tool_tip_item();

		explicit tool_tip_item(ui::tree &parent);

		tool_tip_item(ui::tree &parent, std::size_t index);

		virtual ~tool_tip_item();

		virtual utility::error_code set_target(ui::object &value, const std::function<void(tool_tip_item &, utility::error_code)> &callback = nullptr);

		virtual ui::object *get_target(const std::function<void(ui::object *)> &callback = nullptr) const;

		virtual utility::error_code set_max_width(int value, const std::function<void(tool_tip_item &, utility::error_code)> &callback = nullptr);

		virtual int get_max_width(const std::function<void(int)> &callback = nullptr) const;

		virtual utility::error_code set_title(const std::wstring &value, const std::function<void(tool_tip_item &, utility::error_code)> &callback = nullptr);

		virtual const std::wstring &get_title(const std::function<void(const std::wstring &)> &callback = nullptr) const;

		virtual utility::error_code set_text(const std::wstring &value, const std::function<void(tool_tip_item &, utility::error_code)> &callback = nullptr);

		virtual const std::wstring &get_text(const std::function<void(const std::wstring &)> &callback = nullptr) const;

		virtual utility::error_code set_font(HFONT value, const std::function<void(tool_tip_item &, utility::error_code)> &callback = nullptr);

		virtual HFONT get_font(const std::function<void(HFONT)> &callback = nullptr) const;

	protected:
		friend class tool_tip;

		virtual utility::error_code create_() override;

		virtual utility::error_code destroy_() override;

		virtual bool is_created_() const override;

		virtual utility::error_code set_parent_value_(ui::tree *value, bool changing) override;

		virtual utility::error_code set_dimension_(int x, int y, int width, int height, UINT flags, bool allow_animation) override;

		virtual utility::error_code set_target_(ui::object &value);

		virtual utility::error_code set_max_width_(int value);

		virtual int get_max_width_() const;

		virtual utility::error_code set_title_(const std::wstring &value);

		virtual utility::error_code set_text_(const std::wstring &value);

		virtual utility::error_code set_font_(HFONT value);

		virtual HFONT get_font_() const;

		virtual LRESULT need_text_(NMTTDISPINFOW &info) const;

		virtual LRESULT showing_() const;

		virtual LRESULT popping_() const;

		virtual LRESULT link_clicked_() const;

		virtual ui::window_surface *get_target_window_ancestor_(POINT &offset) const;

		virtual void compute_values_(bool target_changed);

		HWND handle_ = nullptr;
		UINT_PTR local_id_ = 0u;

		ui::object *target_ = nullptr;
		RECT initial_dimension_{};

		ui::window_surface *computed_target_ = nullptr;
		RECT computed_dimension_{};
		POINT computed_offset_{};

		int max_width_ = 0;
		std::wstring title_;
		std::wstring text_;

		HFONT font_ = nullptr;
		HGDIOBJ image_ = nullptr;
	};

	class inplace_tool_tip_item : public tool_tip_item{
	public:
		inplace_tool_tip_item();

		explicit inplace_tool_tip_item(ui::tree &parent);

		inplace_tool_tip_item(ui::tree &parent, std::size_t index);

		virtual ~inplace_tool_tip_item();
	};
}
