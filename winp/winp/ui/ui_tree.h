#pragma once

#include <list>

#include "ui_object.h"

namespace winp::ui{
	class tree : public object{
	public:
		tree();

		explicit tree(thread::object &thread);

		virtual ~tree();

		virtual void add_child(object &child, const std::function<void(tree &, utility::error_code)> &callback = nullptr);

		virtual void insert_child(object &child, std::size_t index, const std::function<void(tree &, utility::error_code)> &callback = nullptr);

		virtual void remove_child(object &child, const std::function<void(tree &, utility::error_code)> &callback = nullptr);

		virtual void erase_child(std::size_t index, const std::function<void(tree &, utility::error_code)> &callback = nullptr);

		virtual std::size_t find_child(const object &child, const std::function<void(std::size_t)> &callback = nullptr) const;

		virtual object *get_child_at(std::size_t index, const std::function<void(object *)> &callback = nullptr) const;

		virtual void traverse_children(const std::function<void(object &)> &callback, bool block = false) const;

	protected:
		friend class object;

		virtual void insert_child_(object &child, std::size_t index);

		virtual void erase_child_(std::size_t index);

		virtual void change_child_index_(std::size_t old_index, std::size_t new_index);

		virtual std::size_t find_child_(const object &child) const;

		virtual object *get_child_at_(std::size_t index) const;

		virtual void traverse_children_(const std::function<void(object &)> &callback) const;

		std::list<object *> children_;
	};
}
