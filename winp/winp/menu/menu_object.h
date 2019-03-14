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

		virtual utility::error_code set_system_state(bool is_system, const std::function<void(object &, utility::error_code)> &callback = nullptr);

		virtual bool is_system(const std::function<void(bool)> &callback = nullptr) const;

		virtual HMENU get_handle(const std::function<void(HMENU)> &callback = nullptr) const;

	protected:
		virtual utility::error_code create_() override;

		virtual utility::error_code destroy_() override;

		virtual bool is_created_() const override;

		virtual utility::error_code set_parent_value_(ui::tree *value, bool changing) override;

		virtual std::size_t get_items_count_before_() const override;

		virtual utility::error_code set_system_state_(bool is_system);

		virtual bool is_system_() const;

		virtual HMENU create_handle_() = 0;

		virtual utility::error_code destroy_handle_() = 0;

		HMENU handle_ = nullptr;
	};

	class popup : public object{
	public:
		popup();

		explicit popup(bool is_system);

		explicit popup(thread::object &thread);

		popup(thread::object &thread, bool is_system);

		virtual ~popup();

	protected:
		virtual utility::error_code set_system_state_(bool is_system) override;

		virtual bool is_system_() const override;

		virtual HMENU create_handle_() override;

		virtual utility::error_code destroy_handle_() override;

		bool is_system_value_ = false;
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
