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

		virtual void traverse_children(const std::function<bool(object &)> &callback, bool block) const;

		virtual void traverse_all_children(const std::function<void(object &)> &callback, bool block) const;

		virtual void traverse_offspring(const std::function<bool(object &)> &callback, bool block) const;

		virtual void traverse_all_offspring(const std::function<void(object &)> &callback, bool block) const;

		template <typename target_type>
		void traverse_children_of(const std::function<bool(target_type &)> &callback, bool block) const{
			traverse_children([=](object &child){
				if (auto target_child = dynamic_cast<target_type *>(&child); target_child != nullptr)
					return callback(*target_child);
				return true;
			}, block);
		}

		template <typename target_type>
		void traverse_all_children_of(const std::function<void(target_type &)> &callback, bool block) const{
			traverse_all_children([=](object &child){
				if (auto target_child = dynamic_cast<target_type *>(&child); target_child != nullptr)
					callback(*target_child);
			}, block);
		}

		template <typename target_type>
		void traverse_offspring_of(const std::function<bool(target_type &)> &callback, bool block) const{
			traverse_offspring([=](object &child){
				if (auto target_child = dynamic_cast<target_type *>(&child); target_child != nullptr)
					return callback(*target_child);
				return true;
			}, block);
		}

		template <typename target_type>
		void traverse_all_offspring_of(const std::function<void(target_type &)> &callback, bool block) const{
			traverse_all_offspring([=](object &child){
				if (auto target_child = dynamic_cast<target_type *>(&child); target_child != nullptr)
					callback(*target_child);
			}, block);
		}

	protected:
		friend class object;
		friend class thread::item_manager;

		virtual utility::error_code destruct_() override;

		virtual utility::error_code insert_child_(object &child, std::size_t index);

		virtual utility::error_code do_insert_child_(object &child, std::size_t index);

		virtual void child_inserted_(object &child);

		virtual utility::error_code erase_child_(std::size_t index);

		virtual utility::error_code do_erase_child_(object &child, std::size_t index);

		virtual void child_erased_(object &child);

		virtual utility::error_code change_child_index_(std::size_t old_index, std::size_t new_index);

		virtual std::size_t find_child_(const object &child) const;

		virtual object *get_child_at_(std::size_t index) const;

		virtual void traverse_children_(const std::function<bool(object &)> &callback) const;

		virtual bool traverse_offspring_(const std::function<bool(object &)> &callback) const;

		template <typename target_type>
		void traverse_children_of_(const std::function<bool(target_type &)> &callback) const{
			traverse_children_([=](object &child){
				if (auto target_child = dynamic_cast<target_type *>(&child); target_child != nullptr)
					return callback(*target_child);
				return true;
			});
		}

		template <typename target_type>
		void traverse_offspring_of_(const std::function<bool(target_type &)> &callback) const{
			traverse_offspring_([=](object &child){
				if (auto target_child = dynamic_cast<target_type *>(&child); target_child != nullptr)
					return callback(*target_child);
				return true;
			});
		}

		std::list<object *> children_;
	};
}
