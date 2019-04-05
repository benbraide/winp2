#pragma once

#include "../thread/thread_item.h"

namespace winp::ui{
	class tree;

	class object : public thread::item{
	public:
		object();

		explicit object(thread::object &thread);

		explicit object(tree &parent);

		object(tree &parent, std::size_t index);

		virtual ~object();

		virtual utility::error_code create(const std::function<void(object &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code destroy(const std::function<void(object &, utility::error_code)> &callback = nullptr);

		virtual bool is_created(const std::function<void(bool)> &callback = nullptr) const;

		virtual utility::error_code set_parent(tree *value, const std::function<void(object &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code set_parent(tree *value, std::size_t index, const std::function<void(object &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code set_parent(tree &value, const std::function<void(object &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code set_parent(tree &value, std::size_t index, const std::function<void(object &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code remove_from_parent(const std::function<void(object &, utility::error_code)> &callback = nullptr);

		virtual tree *get_parent(const std::function<void(tree *)> &callback = nullptr) const;

		virtual tree *get_top_ancestor(const std::function<void(tree *)> &callback = nullptr) const;

		template <typename target_type>
		target_type *get_first_ancestor_of(const std::function<void(target_type *)> &callback = nullptr) const{
			return get_first_ancestor_of<target_type>(nullptr, callback);
		}

		template <typename target_type>
		target_type *get_first_ancestor_of(const std::function<bool(tree &)> &not_type_callback, const std::function<bool(target_type *)> &callback = nullptr) const{
			return compute_or_post_task_inside_thread_context([=]{
				return pass_return_value_to_callback(callback, get_first_ancestor_of_<target_type>(not_type_callback));
			}, (callback != nullptr), nullptr);
		}

		virtual bool is_ancestor(const tree &target, const std::function<void(bool)> &callback = nullptr) const;

		virtual utility::error_code set_index(std::size_t value, const std::function<void(object &, utility::error_code)> &callback = nullptr);

		virtual std::size_t get_index(const std::function<void(std::size_t)> &callback = nullptr) const;

		virtual void traverse_ancestors(const std::function<bool(tree &)> &callback, bool block) const;

		virtual void traverse_all_ancestors(const std::function<void(tree &)> &callback, bool block) const;

		virtual void traverse_siblings(const std::function<bool(object &)> &callback, bool block) const;

		virtual void traverse_all_siblings(const std::function<void(object &)> &callback, bool block) const;

		template <typename target_type>
		void traverse_ancestors_of(const std::function<bool(target_type &)> &callback, bool block) const{
			traverse_ancestors([=](tree &ancestor){
				if (auto target_ancestor = dynamic_cast<target_type *>(&ancestor); target_ancestor != nullptr)
					return callback(*target_ancestor);
				return true;
			}, block);
		}

		template <typename target_type>
		void traverse_all_ancestors_of(const std::function<void(target_type &)> &callback, bool block) const{
			traverse_all_ancestors([=](tree &ancestor){
				if (auto target_ancestor = dynamic_cast<target_type *>(&ancestor); target_ancestor != nullptr)
					callback(*target_ancestor);
			}, block);
		}

		template <typename target_type>
		void traverse_siblings_of(const std::function<bool(target_type &)> &callback, bool block) const{
			traverse_siblings([=](object &sibling){
				if (auto target_sibling = dynamic_cast<target_type *>(&sibling); target_sibling != nullptr)
					return callback(*target_sibling);
				return true;
			}, block);
		}

		template <typename target_type>
		void traverse_all_siblings_of(const std::function<void(target_type &)> &callback, bool block) const{
			traverse_all_siblings([=](object &sibling){
				if (auto target_sibling = dynamic_cast<target_type *>(&sibling); target_sibling != nullptr)
					callback(*target_sibling);
			}, block);
		}

	protected:
		friend class tree;

		virtual utility::error_code destruct_() override;

		virtual utility::error_code create_();

		virtual utility::error_code destroy_();

		virtual bool is_created_() const;

		virtual utility::error_code set_parent_(tree *value, std::size_t index);

		virtual utility::error_code set_parent_value_(tree *value, bool changing);

		virtual tree *get_parent_() const;

		virtual tree *get_top_ancestor_() const;

		template <typename target_type>
		target_type *get_first_ancestor_of_(const std::function<bool(tree &)> &not_type_callback) const{
			for (auto ancestor = parent_; ancestor != nullptr; ancestor = dynamic_cast<object *>(ancestor)->parent_){
				if (auto target_ancestor = dynamic_cast<target_type *>(ancestor); target_ancestor != nullptr)
					return target_ancestor;

				if (not_type_callback != nullptr && !not_type_callback(*ancestor))
					break;
			}

			return nullptr;
		}

		virtual bool is_ancestor_(const tree &target) const;

		virtual utility::error_code set_index_(std::size_t value);

		virtual utility::error_code set_index_value_(std::size_t value, bool changing);

		virtual std::size_t get_index_() const;

		virtual void traverse_ancestors_(const std::function<bool(tree &)> &callback) const;

		virtual void traverse_siblings_(const std::function<bool(object &)> &callback) const;

		template <typename target_type>
		void traverse_ancestors_of_(const std::function<bool(target_type &)> &callback) const{
			traverse_ancestors_([=](tree &ancestor){
				if (auto target_ancestor = dynamic_cast<target_type *>(&ancestor); target_ancestor != nullptr)
					return callback(*target_ancestor);
				return true;
			});
		}

		template <typename target_type>
		void traverse_siblings_of_(const std::function<bool(target_type &)> &callback) const{
			traverse_siblings_([=](object &sibling){
				if (auto target_sibling = dynamic_cast<target_type *>(&sibling); target_sibling != nullptr)
					return callback(*target_sibling);
				return true;
			});
		}

		tree *parent_ = nullptr;
		std::size_t index_ = static_cast<std::size_t>(-1);
	};
}
