#pragma once

#include "ui_tree.h"
#include "ui_visible_surface.h"

namespace winp::ui{
	class non_window_surface : public tree, public visible_surface{
	public:
		non_window_surface();

		explicit non_window_surface(thread::object &thread);

		explicit non_window_surface(tree &parent);

		non_window_surface(tree &parent, std::size_t index);

		virtual ~non_window_surface();

		virtual HRGN get_handle(const std::function<void(HRGN)> &callback = nullptr) const;

	protected:
		friend class thread::item_manager;

		non_window_surface(thread::object &thread, bool init_grid);

		virtual utility::error_code create_() override;

		virtual utility::error_code destroy_() override;

		virtual bool is_created_() const override;

		virtual utility::error_code redraw_() const override;

		virtual utility::error_code redraw_(const RECT &region) const override;

		virtual utility::error_code set_visibility_(bool is_visible, bool redraw) override;

		virtual utility::error_code show_() override;

		virtual utility::error_code hide_() override;

		virtual bool is_visible_() const override;

		virtual HRGN get_handle_() const;

		virtual HRGN create_handle_() const;

		virtual utility::error_code update_handle_();

		HRGN handle_ = nullptr;
		bool visible_ = true;
	};
}
