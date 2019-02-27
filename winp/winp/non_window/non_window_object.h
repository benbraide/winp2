#pragma once

#include "../ui/ui_tree.h"
#include "../ui/ui_interactive_surface.h"

namespace winp::non_window{
	class object : public ui::tree, public ui::interactive_surface{
	public:
		object();

		explicit object(thread::object &thread);

		explicit object(tree &parent);

		object(tree &parent, std::size_t index);

		virtual ~object();

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

		virtual HRGN create_handle_() const;

		virtual utility::error_code resize_handle_();

		HRGN handle_ = nullptr;
		bool visible_ = true;
	};
}
