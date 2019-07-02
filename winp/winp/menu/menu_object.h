#pragma once

#include "menu_tree.h"

namespace winp::ui{
	class visible_surface;
	class window_surface;
}

namespace winp::menu{
	class link_item;
	class popup;
	class appended_popup;

	class object : public tree, public ui::surface{
	public:
		object();

		explicit object(thread::object &thread);

		virtual ~object();

		virtual HMENU get_handle(const std::function<void(HMENU)> &callback = nullptr) const;

		virtual ui::object *get_target(const std::function<void(ui::object *)> &callback = nullptr) const;

	protected:
		friend class thread::item_manager;
		friend class menu::item;
		friend class popup;

		virtual utility::error_code create_() override;

		virtual utility::error_code destroy_() override;

		virtual bool is_created_() const override;

		virtual utility::error_code set_parent_value_(ui::tree *value, bool changing) override;

		virtual std::size_t get_items_count_before_() const override;

		virtual UINT get_insertion_offset_() const;

		virtual ui::object *get_target_() const = 0;

		virtual HMENU create_handle_() = 0;

		virtual utility::error_code destroy_handle_() = 0;

		HMENU handle_ = nullptr;
	};

	class popup : public object{
	public:
		popup();

		explicit popup(thread::object &thread);

		explicit popup(link_item &owner);

		virtual ~popup();

		virtual bool is_system(const std::function<void(bool)> &callback = nullptr) const;

		virtual bool is_sub(const std::function<void(bool)> &callback = nullptr) const;

	protected:
		friend class thread::item_manager;
		friend class appended_popup;
		friend class object;

		virtual tree *get_top_() const override;

		virtual ui::object *get_target_() const override;

		virtual HMENU create_handle_() override;

		virtual utility::error_code destroy_handle_() override;

		virtual bool is_system_() const;

		virtual bool is_sub_() const;

		ui::object *owner_ = nullptr;
		ui::object *target_ = nullptr;
	};

	class wrapped_popup : public popup{
	public:
		wrapped_popup(thread::object &thread, HMENU target_handle);

		wrapped_popup(link_item &owner, HMENU target_handle);

		virtual ~wrapped_popup();

	protected:
		explicit wrapped_popup(thread::object &thread);

		virtual bool should_generate_id_(menu::item &target) const override;

		virtual utility::error_code destroy_handle_() override;

		virtual UINT get_insertion_offset_() const override;

		void wrap_();

		UINT insertion_offset_ = 0u;
		bool is_wrapping_ = false;

		std::vector<ui::object *> wrapped_objects_;
	};

	class system_popup : public wrapped_popup{
	public:
		explicit system_popup(ui::window_surface &owner);

		virtual ~system_popup();

	protected:
		virtual HMENU create_handle_() override;

		virtual tree *get_top_() const override;

		virtual bool is_system_() const override;

		virtual bool is_sub_() const override;
	};

	class appended_popup : public popup{
	public:
		explicit appended_popup(popup &popup_target);

		virtual ~appended_popup();

		virtual const popup &get_popup_target(const std::function<void(const popup &)> &callback = nullptr) const;

		virtual popup &get_popup_target(const std::function<void(popup &)> &callback = nullptr);

	protected:
		virtual utility::error_code create_() override;

		virtual utility::error_code destroy_() override;

		virtual tree *get_top_() const override;

		virtual ui::object *get_target_() const override;

		popup &popup_target_;
	};

	class bar : public object{
	public:
		explicit bar(ui::window_surface &owner);

		virtual ~bar();

		virtual const ui::window_surface &get_owner(const std::function<void(const ui::window_surface &)> &callback = nullptr) const;

		virtual ui::window_surface &get_owner(const std::function<void(ui::window_surface &)> &callback = nullptr);

	protected:
		friend class popup;

		virtual ui::object *get_target_() const override;

		virtual HMENU create_handle_() override;

		virtual utility::error_code destroy_handle_() override;

		ui::window_surface &owner_;
	};
}
