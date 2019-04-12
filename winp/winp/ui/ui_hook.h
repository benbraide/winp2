#pragma once

#include <variant>

#include "../thread/thread_item.h"

namespace winp::ui{
	class object;
	class tree;

	class hook{
	public:
		virtual ~hook() = default;
	};

	class parent_size_hook : public hook{
	public:
		parent_size_hook(object &target, const std::function<void(hook &)> &callback);

		parent_size_hook(object &target, const std::function<void()> &callback);

		virtual ~parent_size_hook();

	protected:
		parent_size_hook(object &target);

		virtual void bind_size_event_(tree *parent, tree *previous_parent);

		object &target_;
		std::function<void(hook &)> callback_;
		unsigned __int64 size_event_id_ = 0u;
		unsigned __int64 tree_event_id_ = 0u;
	};

	class children_size_and_position_hook : public hook{
	public:
		enum class change_type{
			size,
			position,
			both,
		};

		children_size_and_position_hook(tree &target, const std::function<void(hook &, change_type)> &callback);

		children_size_and_position_hook(tree &target, const std::function<void(change_type)> &callback);

		virtual ~children_size_and_position_hook();

	protected:
		children_size_and_position_hook(tree &target);

		children_size_and_position_hook(tree &target, bool);

		virtual void children_change_(object &child, events::children_change::action_type action);

		virtual void do_callback_(UINT flags);

		virtual UINT get_target_flags_() const;

		tree &target_;
		std::function<void(hook &, change_type)> callback_;
		std::unordered_map<object *, unsigned __int64> event_ids_;
		unsigned __int64 tree_event_id_ = 0u;
	};

	class children_size_hook : public children_size_and_position_hook{
	public:
		children_size_hook(tree &target, const std::function<void(hook &)> &callback);

		children_size_hook(tree &target, const std::function<void()> &callback);

		virtual ~children_size_hook();

	protected:
		virtual UINT get_target_flags_() const override;
	};

	class children_position_hook : public children_size_and_position_hook{
	public:
		children_position_hook(tree &target, const std::function<void(hook &)> &callback);

		children_position_hook(tree &target, const std::function<void()> &callback);

		virtual ~children_position_hook();

	protected:
		virtual UINT get_target_flags_() const override;
	};

	class placement_hook : public parent_size_hook{
	public:
		enum class alignment_type{
			top_left,
			top_center,
			top_right,
			center_left,
			center,
			center_right,
			bottom_left,
			bottom_center,
			bottom_right,
		};

		placement_hook(object &target);

		placement_hook(object &target, alignment_type alignment);

		placement_hook(object &target, alignment_type alignment, const POINT &offset);

		virtual ~placement_hook();

		virtual utility::error_code set_alignment(alignment_type value, const std::function<void(placement_hook &, utility::error_code)> &callback = nullptr);

		virtual alignment_type get_alignment(const std::function<void(alignment_type)> &callback = nullptr) const;

		virtual utility::error_code set_offset(const POINT &value, const std::function<void(placement_hook &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code set_offset(int x, int y, const std::function<void(placement_hook &, utility::error_code)> &callback = nullptr);

		virtual const POINT &get_offset(const std::function<void(const POINT &)> &callback = nullptr) const;

	protected:
		virtual utility::error_code set_alignment_(alignment_type value);

		virtual utility::error_code set_offset_(int x, int y);

		virtual void update_();

		alignment_type alignment_ = alignment_type::top_left;
		POINT offset_{};
	};

	class parent_fill_hook : public parent_size_hook{
	public:
		parent_fill_hook(object &target);

		parent_fill_hook(object &target, const SIZE &offset);

		parent_fill_hook(object &target, const D2D1_SIZE_F &offset);

		virtual ~parent_fill_hook();

		virtual utility::error_code set_offset(const SIZE &value, const std::function<void(parent_fill_hook &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code set_offset(int width, int height, const std::function<void(parent_fill_hook &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code set_offset(const D2D1_SIZE_F &value, const std::function<void(parent_fill_hook &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code set_offset(float width, float height, const std::function<void(parent_fill_hook &, utility::error_code)> &callback = nullptr);

		virtual const std::variant<SIZE, D2D1_SIZE_F> &get_offset(const std::function<void(const std::variant<SIZE, D2D1_SIZE_F> &)> &callback = nullptr) const;

	protected:
		void init_();

		virtual utility::error_code set_offset_(int width, int height);

		virtual utility::error_code set_offset_(float width, float height);

		virtual void update_();

		std::variant<SIZE, D2D1_SIZE_F> offset_ = SIZE{};
	};

	class first_child_contain_hook : public children_size_and_position_hook{
	public:
		first_child_contain_hook(tree &target);

		first_child_contain_hook(tree &target, const SIZE &offset);

		virtual ~first_child_contain_hook();

		virtual utility::error_code set_offset(const SIZE &value, const std::function<void(first_child_contain_hook &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code set_offset(int width, int height, const std::function<void(first_child_contain_hook &, utility::error_code)> &callback = nullptr);

		virtual const SIZE &get_offset(const std::function<void(const SIZE &)> &callback = nullptr) const;

	protected:
		virtual void children_change_(object &child, events::children_change::action_type action) override;

		virtual UINT get_target_flags_() const override;

		virtual utility::error_code set_offset_(int width, int height);

		virtual void update_();

		SIZE offset_{};
	};
}
