#pragma once

#include "menu_tree.h"

namespace winp::ui{
	class window_surface;
}

namespace winp::menu{
	class object : public tree, public ui::surface{
	public:
		object();

		explicit object(thread::object &thread);

		virtual ~object();

		virtual HMENU get_handle(const std::function<void(HMENU)> &callback = nullptr) const;

	protected:
		virtual utility::error_code create_() override;

		virtual utility::error_code destroy_() override;

		virtual bool is_created_() const override;

		virtual utility::error_code set_parent_value_(ui::tree *value, bool changing) override;

		virtual std::size_t get_items_count_before_() const override;

		virtual HMENU create_handle_() = 0;

		virtual utility::error_code destroy_handle_() = 0;

		HMENU handle_ = nullptr;
	};

	class popup : public object{
	public:
		popup();

		explicit popup(thread::object &thread);

		virtual ~popup();

	protected:
		virtual HMENU create_handle_() override;

		virtual utility::error_code destroy_handle_() override;
	};

	class bar : public object{
	public:
		explicit bar(ui::window_surface &owner);

		virtual ~bar();

		virtual const ui::window_surface &get_owner(const std::function<void(const ui::window_surface &)> &callback = nullptr) const;

		virtual ui::window_surface &get_owner(const std::function<void(ui::window_surface &)> &callback = nullptr);

	protected:
		virtual HMENU create_handle_() override;

		virtual utility::error_code destroy_handle_() override;

		ui::window_surface &owner_;
	};
}
