#pragma once

#include <list>

#include "ui_object.h"

namespace winp::ui{
	class tree : public object{
	public:
		tree();

		explicit tree(thread::object &thread);

		virtual ~tree();

		virtual utility::error_code add_child(object &child, const std::function<void(tree &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code insert_child(object &child, std::size_t index, const std::function<void(tree &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code remove_child(object &child, const std::function<void(tree &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code erase_child(std::size_t index, const std::function<void(tree &, utility::error_code)> &callback = nullptr);

		virtual std::size_t find_child(const object &child, const std::function<void(std::size_t)> &callback = nullptr) const;

		virtual object *get_child_at(std::size_t index, const std::function<void(object *)> &callback = nullptr) const;

		virtual const std::list<object *> &get_children(const std::function<void(const std::list<object *> &)> &callback = nullptr) const;

		virtual void traverse_children(const std::function<bool(object &)> &callback, bool block = false) const;

		virtual void traverse_all_children(const std::function<void(object &)> &callback, bool block = false) const;

	protected:
		friend class object;

		virtual utility::error_code destruct_() override;

		virtual utility::error_code insert_child_(object &child, std::size_t index);

		virtual utility::error_code do_insert_child_(object &child, std::size_t index);

		virtual utility::error_code erase_child_(std::size_t index);

		virtual utility::error_code do_erase_child_(object &child, std::size_t index);

		virtual utility::error_code change_child_index_(std::size_t old_index, std::size_t new_index);

		virtual std::size_t find_child_(const object &child) const;

		virtual object *get_child_at_(std::size_t index) const;

		virtual void traverse_children_(const std::function<bool(object &)> &callback) const;

		std::list<object *> children_;
	};
}
