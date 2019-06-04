#pragma once

#include <variant>

#include "../thread/thread_item.h"

namespace winp::ui{
	class object;

	class hook{
	public:
		explicit hook(object &target);

		virtual ~hook();

	protected:
		object &target_;
	};

	class parent_size_hook : public hook{
	public:
		parent_size_hook(object &target, const std::function<void(hook &)> &callback);

		parent_size_hook(object &target, const std::function<void()> &callback);

		virtual ~parent_size_hook();

	protected:
		explicit parent_size_hook(object &target);

		virtual void bind_size_event_(tree *parent, tree *previous_parent);

		std::function<void(hook &)> callback_;
		unsigned __int64 size_event_id_ = 0u;
		unsigned __int64 tree_event_id_ = 0u;
		unsigned __int64 creation_event_id_ = 0u;
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
		std::unordered_map<object *, event_id_info> event_ids_;
		unsigned __int64 tree_event_id_ = 0u;
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
		unsigned __int64 position_event_id_ = 0u;
		unsigned __int64 tree_event_id_ = 0u;
		unsigned __int64 parent_tree_event_id_ = 0u;
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

		unsigned __int64 generic_size_event_id_ = 0u;
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

	class io_hook : public hook{
	public:
		explicit io_hook(object &target);

		virtual ~io_hook();
	};

	class drag_hook : public hook{
	public:
		explicit drag_hook(object &target);

		virtual ~drag_hook();

	protected:
		unsigned __int64 drag_begin_event_id_ = 0u;
		unsigned __int64 drag_event_id_ = 0u;
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

	class auto_create_hook : public hook{
	public:
		explicit auto_create_hook(object &target);

		virtual ~auto_create_hook();
	};

	class animation_hook : public hook{
	public:
		using easing_type = std::function<float(float)>;
		using key_type = thread::item::event_manager_type::key_type;

		explicit animation_hook(object &target);

		animation_hook(object &target, const easing_type &easing);

		animation_hook(object &target, const std::chrono::microseconds &duration);

		animation_hook(object &target, const easing_type &easing, const std::chrono::microseconds &duration);

		virtual ~animation_hook();

		virtual utility::error_code set_easing(const easing_type &value, const std::function<void(animation_hook &, utility::error_code)> &callback = nullptr);

		virtual const easing_type &get_easing(const std::function<void(const easing_type &)> &callback = nullptr) const;

		virtual utility::error_code set_duration(const std::chrono::microseconds &value, const std::function<void(animation_hook &, utility::error_code)> &callback = nullptr);

		virtual const std::chrono::microseconds &get_duration(const std::function<void(const std::chrono::microseconds &)> &callback = nullptr) const;

		template <typename target_type>
		void allow_type(){
			allow_type(thread::item::event_manager_type::get_key<target_type>());
		}

		virtual void allow_type(key_type key);

		template <typename target_type>
		void disallow_type(){
			disallow_type(thread::item::event_manager_type::get_key<target_type>());
		}

		virtual void disallow_type(key_type key);

		template <typename target_type>
		bool type_is_allowed() const{
			return type_is_allowed(thread::item::event_manager_type::get_key<target_type>());
		}

		virtual bool type_is_allowed(key_type key) const;

	protected:
		virtual utility::error_code set_easing_(const easing_type &value);

		virtual utility::error_code set_duration_(const std::chrono::microseconds &value);

		easing_type easing_;
		std::chrono::microseconds duration_;
		std::unordered_map<key_type, bool> allowed_list_;
	};

	class animation_suppression_hook : public hook{
	public:
		using key_type = thread::item::event_manager_type::key_type;

		explicit animation_suppression_hook(object &target, bool once = true);

		virtual ~animation_suppression_hook();

		virtual bool is_once(const std::function<void(bool)> &callback = nullptr) const;

		template <typename target_type>
		void suppress_type(){
			suppress_type(thread::item::event_manager_type::get_key<target_type>());
		}

		virtual void suppress_type(key_type key);

		template <typename target_type>
		void unsuppress_type(){
			unsuppress_type(thread::item::event_manager_type::get_key<target_type>());
		}

		virtual void unsuppress_type(key_type key);

		template <typename target_type>
		bool type_is_suppressed() const{
			return type_is_suppressed(thread::item::event_manager_type::get_key<target_type>());
		}

		virtual bool type_is_suppressed(key_type key) const;

	protected:
		bool once_;
		std::unordered_map<key_type, bool> suppressed_list_;
	};
}
