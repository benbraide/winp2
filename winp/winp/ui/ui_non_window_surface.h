#pragma once

#include "ui_tree.h"
#include "ui_interactive_surface.h"

namespace winp::ui{
	class non_window_surface : public ui::tree, public ui::interactive_surface{
	public:
		non_window_surface();

		explicit non_window_surface(thread::object &thread);

		explicit non_window_surface(tree &parent);

		non_window_surface(tree &parent, std::size_t index);

		virtual ~non_window_surface();

		virtual HRGN get_handle(const std::function<void(HRGN)> &callback = nullptr) const;

	protected:
		friend class thread::item_manager;

		virtual utility::error_code create_() override;

		virtual utility::error_code destroy_() override;

		virtual bool is_created_() const override;

		virtual utility::error_code redraw_() const override;

		virtual utility::error_code redraw_(const RECT &region) const override;

		virtual utility::error_code show_() override;

		virtual utility::error_code hide_() override;

		virtual bool is_visible_() const override;

		virtual HRGN get_handle_() const;

		virtual HRGN create_handle_() const = 0;

		virtual utility::error_code resize_handle_() = 0;

		HRGN handle_ = nullptr;
		bool visible_ = true;
	};
}