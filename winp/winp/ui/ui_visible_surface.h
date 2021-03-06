#pragma once

#include "ui_surface.h"

namespace winp::ui{
	class visible_surface : public surface{
	public:
		visible_surface();

		virtual ~visible_surface();

		virtual utility::error_code redraw(bool non_client, const std::function<void(visible_surface, utility::error_code)> &callback = nullptr) const;

		virtual utility::error_code redraw(const RECT &region, const std::function<void(visible_surface, utility::error_code)> &callback = nullptr) const;

		virtual utility::error_code redraw(HRGN rgn, const std::function<void(visible_surface, utility::error_code)> &callback = nullptr) const;

		virtual utility::error_code show(const std::function<void(visible_surface, utility::error_code)> &callback = nullptr);

		virtual utility::error_code hide(const std::function<void(visible_surface, utility::error_code)> &callback = nullptr);

		virtual utility::error_code set_visibility(bool is_visible, const std::function<void(visible_surface, utility::error_code)> &callback = nullptr);

		virtual bool is_visible(const std::function<void(bool)> &callback = nullptr) const;

		virtual utility::error_code set_background_brush(ID2D1Brush *value, const std::function<void(visible_surface, utility::error_code)> &callback = nullptr);

		virtual ID2D1Brush *get_background_brush(const std::function<void(bool)> &callback = nullptr) const;

		virtual utility::error_code set_background_color(const D2D1_COLOR_F &value, const std::function<void(visible_surface, utility::error_code)> &callback = nullptr);

		virtual utility::error_code set_background_color(const D2D1::ColorF &value, const std::function<void(visible_surface, utility::error_code)> &callback = nullptr);

		virtual const D2D1_COLOR_F &get_background_color(const std::function<void(const D2D1_COLOR_F &)> &callback = nullptr) const;

		static COLORREF convert_rgb_to_rgba(COLORREF rgb, BYTE a = 255);

		static COLORREF convert_colorf_to_colorref(const D2D1_COLOR_F &value);

		static COLORREF convert_colorf_to_colorref(const D2D1::ColorF &value);

		static D2D1_COLOR_F convert_colorref_to_colorf(COLORREF value);

		static D2D1_COLOR_F convert_colorref_to_colorf(COLORREF value, BYTE a);

		static bool compare_colors(const D2D1_COLOR_F &first, const D2D1_COLOR_F &second);

		static bool compare_colors(const D2D1::ColorF &first, const D2D1::ColorF &second);

	protected:
		friend class thread::item_manager;
		friend class events::background_color;
		friend class non_window_surface;

		virtual utility::error_code redraw_(bool non_client) const;

		virtual utility::error_code redraw_(const RECT &region) const;

		virtual utility::error_code redraw_(HRGN rgn) const;

		virtual utility::error_code set_visibility_(bool is_visible, bool redraw);

		virtual utility::error_code show_();

		virtual utility::error_code hide_();

		virtual bool is_visible_() const;

		virtual utility::error_code set_background_brush_(ID2D1Brush *value);

		virtual utility::error_code set_background_color_(const D2D1_COLOR_F &value, bool allow_animation);

		virtual utility::error_code animate_background_color_(object &object_self, animation_hook &hk, const D2D1_COLOR_F &value);

		virtual utility::error_code update_background_color_(const D2D1_COLOR_F &value);

		virtual const D2D1_COLOR_F &get_background_color_() const;

		virtual const D2D1_COLOR_F &get_current_background_color_() const;

		D2D1_COLOR_F background_color_{};
		ID2D1Brush *background_brush_ = nullptr;
	};
}
