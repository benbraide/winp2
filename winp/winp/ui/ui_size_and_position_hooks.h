#pragma once

#include "ui_hook.h"

namespace winp::ui{
	class parent_size_hook : public hook{
	public:
		parent_size_hook(object &target, const std::function<void(hook &)> &callback);

		parent_size_hook(object &target, const std::function<void()> &callback);

		virtual ~parent_size_hook();

	protected:
		explicit parent_size_hook(object &target);

		virtual void bind_size_event_(tree *parent, tree *previous_parent);

		std::function<void(hook &)> callback_;
	};

	class children_size_and_position_hook : public hook{
	public:
		enum class change_type{
			size,
			position,
			all,
		};

		struct event_id_info{
			unsigned __int64 position;
			unsigned __int64 creation;
		};

		children_size_and_position_hook(object &target, const std::function<void(hook &, change_type)> &callback);

		children_size_and_position_hook(object &target, const std::function<void(change_type)> &callback);

		virtual ~children_size_and_position_hook();

	protected:
		explicit children_size_and_position_hook(object &target);

		virtual void children_change_(object &child, events::children_change::action_type action);

		virtual void do_callback_(UINT flags);

		std::function<void(hook &, change_type)> callback_;
	};

	class sibling_size_and_position_hook : public hook{
	public:
		enum class sibling_type{
			previous,
			next,
		};

		enum class change_type{
			size,
			position,
			all,
		};

		sibling_size_and_position_hook(object &target, sibling_type type, const std::function<void(hook &, change_type)> &callback);

		sibling_size_and_position_hook(object &target, sibling_type type, const std::function<void(change_type)> &callback);

		virtual ~sibling_size_and_position_hook();

		virtual sibling_type get_sibling_type(const std::function<void(sibling_type)> &callback = nullptr) const;

	protected:
		sibling_size_and_position_hook(object &target, sibling_type type);

		virtual void siblings_change_();

		virtual void do_callback_(UINT flags);

		sibling_type sibling_type_;
		std::function<void(hook &, change_type)> callback_;

		object *sibling_ = nullptr;
	};

	class generic_placement_hook{
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

		enum class relative_type{
			target,
			self,
		};

		explicit generic_placement_hook(object &target, relative_type relativity);

		generic_placement_hook(object &target, alignment_type alignment, relative_type relativity);

		generic_placement_hook(object &target, alignment_type alignment, const POINT &offset, relative_type relativity);

		virtual ~generic_placement_hook();

		virtual utility::error_code set_alignment(alignment_type value, const std::function<void(generic_placement_hook &, utility::error_code)> &callback = nullptr);

		virtual alignment_type get_alignment(const std::function<void(alignment_type)> &callback = nullptr) const;

		virtual utility::error_code set_offset(const POINT &value, const std::function<void(generic_placement_hook &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code set_offset(int x, int y, const std::function<void(generic_placement_hook &, utility::error_code)> &callback = nullptr);

		virtual const POINT &get_offset(const std::function<void(const POINT &)> &callback = nullptr) const;

		virtual utility::error_code set_relativity(relative_type value, const std::function<void(generic_placement_hook &, utility::error_code)> &callback = nullptr);

		virtual relative_type get_relativity(const std::function<void(relative_type)> &callback = nullptr) const;

	protected:
		virtual utility::error_code set_alignment_(alignment_type value);

		virtual utility::error_code set_offset_(int x, int y);

		virtual utility::error_code set_relativity_(relative_type value);

		virtual bool should_react_to_relativity_() const = 0;

		virtual void update_() = 0;

		object &generic_target_;
		alignment_type alignment_;

		POINT offset_;
		relative_type relativity_;
	};

	class placement_hook : public parent_size_hook, public generic_placement_hook{
	public:
		explicit placement_hook(object &target);

		placement_hook(object &target, alignment_type alignment);

		placement_hook(object &target, alignment_type alignment, const POINT &offset);

		virtual ~placement_hook();

	protected:
		virtual bool should_react_to_relativity_() const override;

		virtual void update_() override;
	};

	class parent_fill_hook : public parent_size_hook{
	public:
		explicit parent_fill_hook(object &target);

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

	class children_contain_hook : public children_size_and_position_hook{
	public:
		explicit children_contain_hook(object &target);

		children_contain_hook(object &target, const SIZE &padding);

		virtual ~children_contain_hook();

		virtual utility::error_code set_padding(const SIZE &value, const std::function<void(children_contain_hook &, utility::error_code)> &callback = nullptr);

		virtual const SIZE &get_padding(const std::function<void(const SIZE &)> &callback = nullptr) const;

	protected:
		virtual utility::error_code set_padding_(const SIZE &value);

		virtual void update_();

		SIZE padding_{};
	};

	class sibling_placement_hook : public sibling_size_and_position_hook, public generic_placement_hook{
	public:
		sibling_placement_hook(object &target, sibling_type type, relative_type relativity = relative_type::self);

		sibling_placement_hook(object &target, sibling_type type, alignment_type alignment, relative_type relativity = relative_type::self);

		sibling_placement_hook(object &target, sibling_type type, alignment_type alignment, const POINT &offset, relative_type relativity = relative_type::self);

		virtual ~sibling_placement_hook();

	protected:
		virtual bool should_react_to_relativity_() const override;

		virtual void update_() override;
	};
}
