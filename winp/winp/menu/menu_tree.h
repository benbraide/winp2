#pragma once

#include "../ui/ui_tree.h"

#include "menu_item.h"

namespace winp::menu{
	class tree : public ui::tree{
	public:
		tree();

		explicit tree(thread::object &thread);

		virtual ~tree();

		virtual std::size_t get_items_count(const std::function<void(std::size_t)> &callback = nullptr) const;

		virtual std::size_t get_items_count_before(const std::function<void(std::size_t)> &callback = nullptr) const;

		virtual menu::item *find_item(UINT id, const std::function<void(menu::item *)> &callback = nullptr) const;

		virtual menu::item *get_item_at(std::size_t index, const std::function<void(menu::item *)> &callback = nullptr) const;

		virtual UINT get_states(std::size_t index, const std::function<void(UINT)> &callback = nullptr) const;

		virtual UINT get_types(std::size_t index, const std::function<void(UINT)> &callback = nullptr) const;

		virtual void traverse_items(const std::function<bool(menu::item &)> &callback, bool block = false) const;

		virtual void traverse_all_items(const std::function<void(menu::item &)> &callback, bool block = false) const;

	protected:
		virtual std::size_t get_items_count_() const;

		virtual std::size_t get_items_count_before_() const;

		virtual menu::item *find_item_(UINT id) const;

		virtual menu::item *get_item_at_(std::size_t index) const;

		virtual UINT get_states_(std::size_t index) const;

		virtual UINT get_types_(std::size_t index) const;

		virtual bool traverse_items_(const std::function<bool(menu::item &)> &callback) const;
	};
}
