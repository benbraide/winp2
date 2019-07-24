#pragma once

#include "menu_item.h"

namespace winp::menu{
	class tree : public ui::tree{
	public:
		using ui_tree_type = ui::tree;

		tree();

		virtual ~tree();

		virtual std::size_t get_items_count(const std::function<void(std::size_t)> &callback = nullptr) const;

		virtual std::size_t get_items_count_before(const std::function<void(std::size_t)> &callback = nullptr) const;

		virtual menu::item *find_item(UINT id, const std::function<void(menu::item *)> &callback = nullptr) const;

		virtual menu::item *get_item_at(std::size_t index, const std::function<void(menu::item *)> &callback = nullptr) const;

		virtual UINT get_states(std::size_t index, const std::function<void(UINT)> &callback = nullptr) const;

		virtual UINT get_types(std::size_t index, const std::function<void(UINT)> &callback = nullptr) const;

		virtual void traverse_items(const std::function<bool(menu::item &)> &callback, bool block) const;

		virtual void traverse_all_items(const std::function<void(menu::item &)> &callback, bool block) const;

	protected:
		friend class menu::item;
		friend class popup;

		virtual void added_event_handler_(event_manager_type &manager, event_manager_type::key_type key, unsigned __int64 id, thread::item *owner) const override;

		virtual void removed_event_handler_(event_manager_type &manager, event_manager_type::key_type key, unsigned __int64 id) const override;

		virtual void child_inserted_(ui::object &child) override;

		virtual tree *get_top_() const;

		virtual std::size_t get_items_count_() const;

		virtual std::size_t get_items_count_before_() const;

		virtual menu::item *find_item_(UINT id, menu::item *exclude, bool search_sub_menus) const;

		virtual menu::item *get_item_at_(std::size_t index) const;

		virtual UINT get_states_(std::size_t index) const;

		virtual UINT get_types_(std::size_t index) const;

		virtual UINT generate_id_(menu::item &target, std::size_t max_tries = 0xFFFFu) const;

		virtual bool id_is_reserved_(UINT id) const;

		virtual bool should_generate_id_(menu::item &target) const;

		virtual void traverse_items_(const std::function<bool(menu::item &)> &callback) const;
	};
}
