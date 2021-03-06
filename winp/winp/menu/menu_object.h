#pragma once

#include "menu_tree.h"

namespace winp::ui{
	class visible_surface;
	class window_surface;
}

namespace winp::menu{
	class link_item;
	class popup;

	class object : public tree, public ui::surface{
	public:
		object();

		virtual ~object();

		virtual HMENU get_handle(const std::function<void(HMENU)> &callback = nullptr) const;

		virtual ui::object *get_target(const std::function<void(ui::object *)> &callback = nullptr) const;

	protected:
		friend class thread::item_manager;
		friend class menu::item;
		friend class popup;

		virtual HTHEME get_theme_() const override;

		virtual std::pair<HDC, HWND> get_device_context_() const override;

		virtual utility::error_code create_() override;

		virtual utility::error_code destroy_() override;

		virtual bool is_created_() const override;

		virtual utility::error_code set_parent_value_(ui::tree *value, bool changing) override;

		virtual std::size_t get_items_count_before_() const override;

		virtual ui::object *get_target_() const = 0;

		virtual HMENU create_handle_() = 0;

		virtual utility::error_code destroy_handle_() = 0;

		HMENU handle_ = nullptr;
		HWND frontend_handle_ = nullptr;
	};

	class popup : public object{
	public:
		popup();

		explicit popup(link_item &owner);

		virtual ~popup();

		virtual bool is_system(const std::function<void(bool)> &callback = nullptr) const;

		virtual bool is_sub(const std::function<void(bool)> &callback = nullptr) const;

	protected:
		friend class thread::item_manager;
		friend class menu::item;
		friend class object;
		friend class tree;

		virtual void child_inserted_(ui::object &child) override;

		virtual void child_erased_(ui::object &child) override;

		virtual tree *get_top_() const override;

		virtual ui::object *get_target_() const override;

		virtual HMENU create_handle_() override;

		virtual utility::error_code destroy_handle_() override;

		virtual bool is_system_() const;

		virtual bool is_sub_() const;

		virtual void clear_modified_list_();

		ui::object *owner_ = nullptr;
		ui::object *target_ = nullptr;

		bool is_modifying_ = false;
		std::unordered_map<ui::object *, bool> modified_list_;
	};

	class wrapped_popup : public popup{
	public:
		explicit wrapped_popup(HMENU target_handle);

		wrapped_popup(link_item &owner, HMENU target_handle);

		virtual ~wrapped_popup();

	protected:
		wrapped_popup();

		virtual bool should_generate_id_(menu::item &target) const override;

		virtual utility::error_code destroy_handle_() override;

		void wrap_();

		bool is_wrapping_ = false;
		std::vector<ui::object *> wrapped_objects_;
	};

	class system_popup : public wrapped_popup{
	public:
		virtual ~system_popup();

	protected:
		friend class ui::window_surface;

		explicit system_popup(ui::window_surface &owner);

		virtual HMENU create_handle_() override;

		virtual tree *get_top_() const override;

		virtual bool is_system_() const override;

		virtual bool is_sub_() const override;
	};

	class bar : public object{
	public:
		virtual ~bar();

		virtual ui::window_surface *get_owner(const std::function<void(ui::window_surface *)> &callback = nullptr) const;

	protected:
		friend class popup;
		friend class ui::window_surface;

		explicit bar(ui::window_surface &owner);

		virtual void child_inserted_(ui::object &child) override;

		virtual void child_erased_(ui::object &child) override;

		virtual ui::object *get_target_() const override;

		virtual HMENU create_handle_() override;

		virtual utility::error_code destroy_handle_() override;

		ui::window_surface *owner_ = nullptr;
	};
}
