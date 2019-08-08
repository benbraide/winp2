#pragma once

#include "ui_tree.h"
#include "ui_visible_surface.h"

namespace winp::ui{
	class non_window_surface : public tree, public visible_surface{
	public:
		non_window_surface();

		explicit non_window_surface(tree &parent);

		non_window_surface(tree &parent, std::size_t index);

		virtual ~non_window_surface();

		virtual utility::error_code update_handle(const std::function<void(non_window_surface &, utility::error_code)> &callback = nullptr);

		virtual HRGN get_handle(const std::function<void(HRGN)> &callback = nullptr) const;

		virtual HRGN get_outer_handle(const std::function<void(HRGN)> &callback = nullptr) const;

	protected:
		friend class surface;
		friend class thread::item_manager;

		friend class events::draw;
		friend class events::non_client_paint;

		virtual HTHEME get_theme_() const override;

		virtual std::pair<HDC, HWND> get_device_context_() const override;

		virtual utility::error_code create_() override;

		virtual utility::error_code destroy_() override;

		virtual bool is_created_() const override;

		virtual RECT get_client_padding_() const;

		virtual UINT hit_test_(int x, int y) const override;

		virtual UINT absolute_hit_test_(int x, int y) const override;

		virtual utility::error_code update_dimension_(const RECT &previous_dimension, int x, int y, int width, int height, UINT flags) override;

		virtual utility::error_code redraw_(bool non_client) const override;

		virtual utility::error_code redraw_(const RECT &region) const override;

		virtual utility::error_code redraw_(HRGN handle) const override;

		virtual utility::error_code set_visibility_(bool is_visible, bool redraw) override;

		virtual utility::error_code show_() override;

		virtual utility::error_code hide_() override;

		virtual bool is_visible_() const override;

		virtual utility::error_code redraw_non_client_() const;

		virtual utility::error_code redraw_client_(utility::rgn &handle) const;

		virtual HRGN get_handle_() const;

		virtual utility::rgn get_outer_handle_() const;

		virtual HRGN create_handle_() const;

		virtual utility::error_code update_handle_();

		virtual utility::error_code destroy_handle_();

		virtual utility::rgn prepare_handle_() const;

		virtual utility::rgn prepare_handle_(utility::rgn &handle, const POINT &offset) const;

		virtual bool intersect_ancestors_(utility::rgn &handle) const;

		virtual bool exclude_siblings_(utility::rgn &handle) const;

		virtual bool exclude_children_(utility::rgn &handle) const;

		virtual bool exclude_from_clip_(HDC device, const POINT &offset) const;

		mutable utility::rgn handle_;
		bool visible_ = true;
	};
}
