#pragma once

#include "ui_hook.h"

namespace winp::ui{
	class tree;

	class object : public thread::item{
	public:
		using hook_ptr_type = std::shared_ptr<hook>;
		using hook_list_type = std::list<hook_ptr_type>;

		object();

		explicit object(thread::object &thread);

		explicit object(tree &parent);

		object(tree &parent, std::size_t index);

		virtual ~object();

		virtual utility::error_code create(const std::function<void(object &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code auto_create(const std::function<void(object &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code destroy(const std::function<void(object &, utility::error_code)> &callback = nullptr);

		virtual bool is_created(const std::function<void(bool)> &callback = nullptr) const;

		virtual utility::error_code set_auto_create_state(bool state, const std::function<void(object &, utility::error_code)> &callback = nullptr);

		virtual bool is_auto_createable(const std::function<void(bool)> &callback = nullptr) const;

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

		virtual object *get_previous_sibling(const std::function<void(object *)> &callback = nullptr) const;

		virtual object *get_next_sibling(const std::function<void(object *)> &callback = nullptr) const;

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

		template <typename hook_type, typename... args_types>
		hook_type *insert_hook(args_types &&... args){
			return compute_task_inside_thread_context([&]() -> hook_type *{
				auto hook = std::make_shared<hook_type>(*this, std::forward<args_types>(args)...);
				if (hook == nullptr)//Failed to create object
					return nullptr;

				auto key = event_manager_type::template get_key<hook_type>();
				if (auto max_allowed = hook->get_max_allowed(); max_allowed != 0u && !hooks_.empty()){
					if (auto it = hooks_.find(key); it != hooks_.end() && max_allowed <= it->second.size())
						return nullptr;//Max allowed reached
				}

				hooks_[key].push_back(hook);
				return hook.get();
			});
		}

		virtual void remove_hook(hook &target);

		template <typename hook_type>
		bool has_hook(const std::function<void(bool)> &callback = nullptr) const{
			return compute_or_post_task_inside_thread_context([=]{
				return pass_return_value_to_callback(callback, has_hook_<hook_type>());
			}, (callback != nullptr), false);
		}

		template <typename hook_type>
		bool has_similar_hook(const std::function<void(bool)> &callback = nullptr) const{
			return compute_or_post_task_inside_thread_context([=]{
				return pass_return_value_to_callback(callback, has_similar_hook_<hook_type>());
			}, (callback != nullptr), false);
		}

		virtual void traverse_hooks(const std::function<bool(hook &)> &callback, bool unique_only, bool block) const;

		virtual void traverse_all_hooks(const std::function<void(hook &)> &callback, bool unique_only, bool block) const;

		virtual bool is_dialog_message(MSG &msg) const;

	protected:
		friend class tree;
		friend class thread::item_manager;

		virtual utility::error_code destruct_() override;

		virtual utility::error_code create_();

		virtual utility::error_code auto_create_();

		virtual utility::error_code destroy_();

		virtual bool is_created_() const;

		virtual utility::error_code set_auto_create_state_(bool state);

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

		virtual object *get_sibling_(bool is_previous) const;

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

		template <typename hook_type>
		bool has_hook_() const{
			if (hooks_.empty())
				return false;

			if (auto it = hooks_.find(event_manager_type::template get_key<hook_type>()); it == hooks_.end() || it->second.empty())
				return false;

			return true;
		}

		template <typename hook_type>
		bool has_similar_hook_() const{
			auto has_similar_hook = false;
			traverse_hooks_([&](hook &hk){
				return !(has_similar_hook = (dynamic_cast<hook_type *>(&hk) != nullptr));
			}, true);

			return has_similar_hook;
		}

		virtual void traverse_hooks_(const std::function<bool(hook &)> &callback, bool unique_only) const;

		virtual bool is_dialog_message_(MSG &msg) const;

		tree *parent_ = nullptr;
		std::size_t index_ = static_cast<std::size_t>(-1);
		std::unordered_map<event_manager_type::key_type, hook_list_type> hooks_;
		bool is_auto_createable_ = true;
	};
}
