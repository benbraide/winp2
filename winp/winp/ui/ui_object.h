#pragma once

#include "ui_general_hooks.h"

namespace winp::ui{
	class tree;
	class surface;
	class visible_surface;

	class object : public thread::item{
	public:
		using hook_ptr_type = std::shared_ptr<hook>;

		object();

		explicit object(tree &parent);

		object(tree &parent, std::size_t index);

		virtual ~object();

		virtual utility::error_code create(const std::function<void(object &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code auto_create(const std::function<void(object &, utility::error_code)> &callback = nullptr);

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
		target_type *get_first_ancestor_of(const std::function<bool(tree &)> &not_type_callback, const std::function<void(target_type *)> &callback = nullptr) const{
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

		virtual void traverse_siblings(const std::function<bool(object &, bool)> &callback, bool block) const;

		virtual void traverse_all_siblings(const std::function<void(object &, bool)> &callback, bool block) const;

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
		void traverse_siblings_of(const std::function<bool(target_type &, bool)> &callback, bool block) const{
			traverse_siblings([=](object &sibling, bool is_before){
				if (auto target_sibling = dynamic_cast<target_type *>(&sibling); target_sibling != nullptr)
					return callback(*target_sibling, is_before);
				return true;
			}, block);
		}

		template <typename target_type>
		void traverse_all_siblings_of(const std::function<void(target_type &, bool)> &callback, bool block) const{
			traverse_all_siblings([=](object &sibling, bool is_before){
				if (auto target_sibling = dynamic_cast<target_type *>(&sibling); target_sibling != nullptr)
					callback(*target_sibling, is_before);
			}, block);
		}

		template <typename hook_type, typename... args_types>
		hook_type *insert_hook(args_types &&... args){
			return compute_task_inside_thread_context([&]{
				return insert_hook_<hook_type>(std::forward<args_types>(args)...);
			});
		}

		template <typename hook_type>
		void remove_hook(){
			execute_task_inside_thread_context([&]{
				remove_hook_<hook_type>();
			});
		}

		template <typename hook_type>
		void remove_similar_hooks(){
			execute_task_inside_thread_context([&]{
				remove_similar_hooks_<hook_type>();
			});
		}

		template <typename hook_type>
		hook_type *find_hook(const std::function<void(hook_type *)> &callback = nullptr) const{
			return compute_or_post_task_inside_thread_context([=]{
				return pass_return_value_to_callback(callback, find_hook_<hook_type>());
			}, (callback != nullptr), false);
		}

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

		virtual void traverse_hooks(const std::function<bool(hook &)> &callback, bool block) const;

		virtual void traverse_all_hooks(const std::function<void(hook &)> &callback, bool block) const;

		virtual bool is_dialog_message(MSG &msg) const;

	protected:
		friend class tree;
		friend class surface;
		friend class visible_surface;
		friend class thread::item_manager;

		virtual void before_destruct_() override;

		virtual utility::error_code create_();

		virtual utility::error_code auto_create_();

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

		virtual object *get_sibling_(bool is_previous) const;

		virtual void traverse_ancestors_(const std::function<bool(tree &)> &callback) const;

		virtual void traverse_siblings_(const std::function<bool(object &, bool)> &callback) const;

		template <typename target_type>
		void traverse_ancestors_of_(const std::function<bool(target_type &)> &callback) const{
			traverse_ancestors_([=](tree &ancestor){
				if (auto target_ancestor = dynamic_cast<target_type *>(&ancestor); target_ancestor != nullptr)
					return callback(*target_ancestor);
				return true;
			});
		}

		template <typename target_type>
		void traverse_siblings_of_(const std::function<bool(target_type &, bool)> &callback) const{
			traverse_siblings_([=](object &sibling, bool is_before){
				if (auto target_sibling = dynamic_cast<target_type *>(&sibling); target_sibling != nullptr)
					return callback(*target_sibling, is_before);
				return true;
			});
		}

		template <typename hook_type, typename... args_types>
		hook_type *insert_hook_(args_types &&... args){
			auto it = hooks_.find(event_manager_type::template get_key<hook_type>());
			if (it != hooks_.end())
				return dynamic_cast<hook_type *>(it->second.get());

			auto hook = std::make_shared<hook_type>(*this, std::forward<args_types>(args)...);
			if (hook == nullptr || !static_cast<ui::hook *>(hook.get())->setup_())
				return nullptr;

			hooks_[event_manager_type::template get_key<hook_type>()] = hook;
			return hook.get();
		}

		template <typename hook_type>
		void remove_hook_(){
			if (!hooks_.empty())
				hooks_.erase(event_manager_type::template get_key<hook_type>());
		}

		template <typename hook_type>
		void remove_similar_hooks_(){
			if (hooks_.empty())
				return;

			std::size_t index = 0u;
			std::list<std::size_t> index_list;

			for (auto &info : hooks_){
				if (dynamic_cast<hook_type *>(info.second.get()) != nullptr)
					index_list.push_front(index);
				++index;
			}

			for (auto index : index_list)
				hooks_.erase(std::next(hooks_.begin(), index));
		}

		template <typename hook_type>
		hook_type *find_hook_() const{
			if (hooks_.empty())
				return nullptr;

			if (auto it = hooks_.find(event_manager_type::template get_key<hook_type>()); it != hooks_.end())
				return dynamic_cast<hook_type *>(it->second.get());

			return nullptr;
		}

		template <typename hook_type>
		bool has_hook_() const{
			return (!hooks_.empty() && hooks_.find(event_manager_type::template get_key<hook_type>()) != hooks_.end());
		}

		template <typename hook_type>
		bool has_similar_hook_() const{
			if (hooks_.empty())
				return false;

			for (auto &info : hooks_){
				if (dynamic_cast<hook_type *>(info.second.get()) != nullptr)
					return true;
			}

			return false;
		}

		virtual void traverse_hooks_(const std::function<bool(hook &)> &callback) const;

		virtual bool is_dialog_message_(MSG &msg) const;

		template <typename target_type>
		void cancel_animation_(){
			cancel_animation_(thread::item::event_manager_type::get_key<target_type>());
		}

		void cancel_animation_(animation_hook::key_type key);

		template <typename target_type>
		void cancel_animation_(animation_hook &hk){
			hk.cancel_<target_type>();
		}

		void cancel_animation_(animation_hook &hk, animation_hook::key_type key);

		template <typename target_type>
		animation_hook::key_info &get_animation_info_(animation_hook &hk) const{
			return hk.get_<target_type>();
		}

		virtual animation_hook::key_info &get_animation_info_(animation_hook &hk, animation_hook::key_type key) const;

		template <typename target_type>
		animation_hook::key_info *get_existing_animation_info_(animation_hook &hk) const{
			return hk.get_existing_<target_type>();
		}

		virtual animation_hook::key_info *get_existing_animation_info_(animation_hook &hk, animation_hook::key_type key) const;

		tree *parent_ = nullptr;
		std::size_t index_ = static_cast<std::size_t>(-1);
		std::unordered_map<event_manager_type::key_type, hook_ptr_type> hooks_;
	};
}
