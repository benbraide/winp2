#pragma once

#include "ui_surface.h"

namespace winp::ui{
	class visible_surface : public surface{
	public:
		virtual ~visible_surface();

		virtual void show(const std::function<void(visible_surface, utility::error_code)> &callback = nullptr);

		virtual void hide(const std::function<void(visible_surface, utility::error_code)> &callback = nullptr);

		virtual void set_visibility(bool is_visible, const std::function<void(visible_surface, utility::error_code)> &callback = nullptr);

		virtual bool is_visible(const std::function<void(bool)> &callback = nullptr) const;

		virtual void set_background_transparency(bool is_transparent, const std::function<void(visible_surface, utility::error_code)> &callback = nullptr);

		virtual bool is_transparent_background(const std::function<void(bool)> &callback = nullptr) const;

		virtual void set_background_color(const D2D1::ColorF &value, const std::function<void(visible_surface, utility::error_code)> &callback = nullptr);

		virtual const D2D1::ColorF &get_background_color(const std::function<void(const D2D1::ColorF &)> &callback = nullptr) const;

		static COLORREF convert_rgb_to_rgba(COLORREF rgb, BYTE a = 255);

		static COLORREF convert_colorf_to_colorref(const D2D1::ColorF &value);

		static D2D1::ColorF convert_colorref_to_colorf(COLORREF value);

		static D2D1::ColorF convert_colorref_to_colorf(COLORREF value, BYTE a);

		static bool compare_colors(const D2D1::ColorF &first, const D2D1::ColorF &second);

	protected:
		virtual void show_();

		virtual void hide_();

		virtual bool is_visible_() const;

		virtual void set_background_transparency_(bool is_transparent);

		virtual bool is_transparent_background_() const;

		virtual void set_background_color_(const D2D1::ColorF &value);

		virtual const D2D1::ColorF &get_background_color_() const;

		D2D1::ColorF background_color_{ 0 };
	};
}
