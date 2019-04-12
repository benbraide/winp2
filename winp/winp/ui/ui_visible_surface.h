#pragma once

#include "ui_surface.h"

namespace winp::ui{
	class visible_surface : public surface{
	public:
		virtual ~visible_surface();

		virtual utility::error_code redraw(const std::function<void(visible_surface, utility::error_code)> &callback = nullptr) const;

		virtual utility::error_code redraw(const RECT &region, const std::function<void(visible_surface, utility::error_code)> &callback = nullptr) const;

		virtual utility::error_code show(const std::function<void(visible_surface, utility::error_code)> &callback = nullptr);

		virtual utility::error_code hide(const std::function<void(visible_surface, utility::error_code)> &callback = nullptr);

		virtual utility::error_code set_visibility(bool is_visible, const std::function<void(visible_surface, utility::error_code)> &callback = nullptr);

		virtual bool is_visible(const std::function<void(bool)> &callback = nullptr) const;

		virtual utility::error_code set_background_transparency(bool is_transparent, const std::function<void(visible_surface, utility::error_code)> &callback = nullptr);

		virtual bool is_transparent_background(const std::function<void(bool)> &callback = nullptr) const;

		virtual utility::error_code set_background_color(const D2D1::ColorF &value, const std::function<void(visible_surface, utility::error_code)> &callback = nullptr);

		virtual const D2D1::ColorF &get_background_color(const std::function<void(const D2D1::ColorF &)> &callback = nullptr) const;

		static COLORREF convert_rgb_to_rgba(COLORREF rgb, BYTE a = 255);

		static COLORREF convert_colorf_to_colorref(const D2D1::ColorF &value);

		static D2D1::ColorF convert_colorref_to_colorf(COLORREF value);

		static D2D1::ColorF convert_colorref_to_colorf(COLORREF value, BYTE a);

		static bool compare_colors(const D2D1::ColorF &first, const D2D1::ColorF &second);

	protected:
		virtual utility::error_code position_change_(bool is_changing) override;

		virtual utility::error_code size_change_(bool is_changing) override;

		virtual utility::error_code redraw_() const;

		virtual utility::error_code redraw_(const RECT &region) const;

		virtual utility::error_code set_visibility_(bool is_visible, bool redraw);

		virtual utility::error_code show_();

		virtual utility::error_code hide_();

		virtual bool is_visible_() const;

		virtual utility::error_code set_background_transparency_(bool is_transparent);

		virtual bool is_transparent_background_() const;

		virtual utility::error_code set_background_color_(const D2D1::ColorF &value);

		virtual const D2D1::ColorF &get_background_color_() const;

		D2D1::ColorF background_color_{ 0 };
	};
}
