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

		virtual void create(const std::function<void(object &, utility::error_code)> &callback = nullptr);

		virtual void destroy(const std::function<void(object &, utility::error_code)> &callback = nullptr);

		virtual bool is_created(const std::function<void(bool)> &callback = nullptr) const;

		virtual void set_parent(tree *value, const std::function<void(object &, utility::error_code)> &callback = nullptr);

		virtual void set_parent(tree *value, std::size_t index, const std::function<void(object &, utility::error_code)> &callback = nullptr);

		virtual tree *get_parent(const std::function<void(tree *)> &callback = nullptr) const;

		virtual bool is_ancestor(const tree &target, const std::function<void(bool)> &callback = nullptr) const;

		virtual void set_index(std::size_t value, const std::function<void(object &, utility::error_code)> &callback = nullptr);

		virtual std::size_t get_index(const std::function<void(std::size_t)> &callback = nullptr) const;

		virtual void traverse_ancestors(const std::function<void(tree &)> &callback, bool block = false) const;

		virtual void traverse_siblings(const std::function<void(object &)> &callback, bool block = false) const;

	protected:
		friend class tree;

		virtual void destruct_() override;

		virtual void create_();

		virtual void destroy_();

		virtual bool is_created_() const;

		virtual void set_parent_(tree *value, std::size_t index);

		virtual tree *get_parent_() const;

		virtual bool is_ancestor_(const tree &target) const;

		virtual void set_index_(std::size_t value);

		virtual std::size_t get_index_() const;

		virtual void traverse_ancestors_(const std::function<void(tree &)> &callback) const;

		virtual void traverse_siblings_(const std::function<void(object &)> &callback) const;

		tree *parent_ = nullptr;
		std::size_t index_ = static_cast<std::size_t>(-1);
	};
}
