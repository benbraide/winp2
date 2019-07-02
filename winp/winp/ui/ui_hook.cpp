#include "../app/app_object.h"
#include "../utility/animation_timing.h"

#include "../menu/menu_action_item.h"
#include "../menu/menu_separator.h"

#include "ui_window_surface.h"

winp::ui::hook::hook(object &target)
	: target_(target){}

winp::ui::hook::~hook() = default;

winp::ui::parent_size_hook::parent_size_hook(object &target, const std::function<void(hook &)> &callback)
	: parent_size_hook(target){
	callback_ = callback;
}

winp::ui::parent_size_hook::parent_size_hook(object &target, const std::function<void()> &callback)
	: parent_size_hook(target){
	callback_ = [=](hook &){
		if (callback != nullptr)
			callback();
	};
}

winp::ui::parent_size_hook::parent_size_hook(object &target)
	: hook(target){
	bind_size_event_(target_.get_parent(), nullptr);
	tree_event_id_ = target_.events().bind([this](events::parent_change &e){
		if (!e.is_changing()){
			bind_size_event_(e.get_value(), e.get_previous_value());
			if (callback_ != nullptr)
				callback_(*this);
		}
	});
}

winp::ui::parent_size_hook::~parent_size_hook(){
	if (auto parent = target_.get_parent(); parent != nullptr){
		parent->events().unbind(size_event_id_);
		parent->events().unbind(creation_event_id_);
	}

	target_.events().unbind(tree_event_id_);
	tree_event_id_ = size_event_id_ = creation_event_id_ = 0u;
}

void winp::ui::parent_size_hook::bind_size_event_(tree *parent, tree *previous_parent){
	if (previous_parent != nullptr){
		previous_parent->events().unbind(size_event_id_);
		previous_parent->events().unbind(creation_event_id_);
		size_event_id_ = creation_event_id_ = 0u;
	}

	if (parent != nullptr && size_event_id_ == 0u){
		size_event_id_ = parent->events().bind([this](events::position_change &e){
			if (callback_ != nullptr && !e.is_changing() && (reinterpret_cast<WINDOWPOS *>(e.get_message().lParam)->flags & SWP_NOSIZE) == 0u)
				callback_(*this);
		});

		creation_event_id_ = parent->events().bind([this](events::create &e){
			if (callback_ != nullptr && !e.is_creating())
				callback_(*this);
		});
	}
}

winp::ui::children_size_and_position_hook::children_size_and_position_hook(object &target, const std::function<void(hook &, change_type)> &callback)
	: children_size_and_position_hook(target){
	callback_ = callback;
}

winp::ui::children_size_and_position_hook::children_size_and_position_hook(object &target, const std::function<void(change_type)> &callback)
	: children_size_and_position_hook(target){
	callback_ = [=](hook &, change_type change){
		if (callback != nullptr)
			callback(change);
	};
}

winp::ui::children_size_and_position_hook::children_size_and_position_hook(object &target)
	: hook(target){
	if (auto tree_target = dynamic_cast<tree *>(&target_); tree_target != nullptr){
		tree_target->traverse_all_children([this](object &child){
			if (dynamic_cast<surface *>(&child) != nullptr && !child.has_similar_hook<parent_size_hook>()){
				auto &info = event_ids_[&child];
				info.position = child.events().bind([this](events::position_change &e){
					if (!e.is_changing())
						do_callback_(reinterpret_cast<WINDOWPOS *>(e.get_message().lParam)->flags);
				});

				info.creation = child.events().bind([this](events::create &e){
					if (!e.is_creating())
						do_callback_(0u);
				});
			}
		}, true);

		tree_event_id_ = target_.events().bind([this](events::children_change &e){
			if (!e.is_changing())
				children_change_(e.get_value(), e.get_action());
		});
	}
}

winp::ui::children_size_and_position_hook::~children_size_and_position_hook(){
	if (!event_ids_.empty()){
		for (auto &info : event_ids_){
			info.first->events().unbind(info.second.position);
			info.first->events().unbind(info.second.creation);
		}

		event_ids_.clear();
	}

	target_.events().unbind(tree_event_id_);
	tree_event_id_ = 0u;
}

void winp::ui::children_size_and_position_hook::children_change_(object &child, events::children_change::action_type action){
	if (action == events::children_change::action_type::insert){
		if (dynamic_cast<surface *>(&child) != nullptr && !child.has_similar_hook<parent_size_hook>()){
			auto &info = event_ids_[&child];
			info.position = child.events().bind([this](events::position_change &e){
				if (!e.is_changing())
					do_callback_(reinterpret_cast<WINDOWPOS *>(e.get_message().lParam)->flags);
			});

			info.creation = child.events().bind([this](events::create &e){
				if (!e.is_creating())
					do_callback_(0u);
			});

			do_callback_(0u);
		}
	}
	else if (action == events::children_change::action_type::remove && !event_ids_.empty()){
		if (auto it = event_ids_.find(&child); it != event_ids_.end()){
			it->first->events().unbind(it->second.position);
			it->first->events().unbind(it->second.creation);

			event_ids_.erase(it);
			do_callback_(0u);
		}
	}
}

void winp::ui::children_size_and_position_hook::do_callback_(UINT flags){
	if (callback_ == nullptr)
		return;

	if ((flags & (SWP_NOSIZE | SWP_NOMOVE)) == 0u)
		callback_(*this, change_type::all);
	else if ((flags & SWP_NOSIZE) == 0u)
		callback_(*this, change_type::size);
	else if ((flags & SWP_NOMOVE) == 0u)
		callback_(*this, change_type::position);
}

winp::ui::sibling_size_and_position_hook::sibling_size_and_position_hook(object &target, sibling_type type, const std::function<void(hook &, change_type)> &callback)
	: sibling_size_and_position_hook(target, type){
	callback_ = callback;
}

winp::ui::sibling_size_and_position_hook::sibling_size_and_position_hook(object &target, sibling_type type, const std::function<void(change_type)> &callback)
	: sibling_size_and_position_hook(target, type){
	callback_ = [=](hook &, change_type change){
		if (callback != nullptr)
			callback(change);
	};
}

winp::ui::sibling_size_and_position_hook::sibling_size_and_position_hook(object &target, sibling_type type)
	: hook(target), sibling_type_(type){
	if (auto object_target = dynamic_cast<object *>(&target_); object_target != nullptr){
		tree_event_id_ = target_.events().bind([this](events::parent_change &e){
			if (e.is_changing())
				return;

			if (auto previous_parent = e.get_previous_value(); previous_parent != nullptr)
				previous_parent->events().unbind(parent_tree_event_id_);//Unbind previous

			if (auto parent = e.get_value(); parent != nullptr){
				parent_tree_event_id_ = parent->events().bind([this](events::children_change &e){
					if (!e.is_changing())
						siblings_change_();
				});
			}
			else//No parent
				parent_tree_event_id_ = 0u;

			siblings_change_();
		});

		if (auto parent = object_target->get_parent(); parent != nullptr){
			parent_tree_event_id_ = parent->events().bind([this](events::children_change &e){
				if (!e.is_changing())
					siblings_change_();
			});
		}

		siblings_change_();
	}
}

winp::ui::sibling_size_and_position_hook::~sibling_size_and_position_hook(){
	if (auto object_target = dynamic_cast<object *>(&target_); object_target != nullptr){
		if (auto parent = object_target->get_parent(); parent != nullptr)
			parent->events().unbind(parent_tree_event_id_);
	}

	if (sibling_ != nullptr)
		sibling_->events().unbind(position_event_id_);

	target_.events().unbind(tree_event_id_);
	tree_event_id_ = position_event_id_ = parent_tree_event_id_ = 0u;
}

winp::ui::sibling_size_and_position_hook::sibling_type winp::ui::sibling_size_and_position_hook::get_sibling_type(const std::function<void(sibling_type)> &callback) const{
	return target_.compute_or_post_task_inside_thread_context([=]{
		return target_.pass_return_value_to_callback(callback, sibling_type_);
	}, (callback != nullptr), sibling_type::previous);
}

void winp::ui::sibling_size_and_position_hook::siblings_change_(){
	auto object_target = dynamic_cast<object *>(&target_);
	if (object_target == nullptr)
		return;

	object *target_sibling = ((sibling_type_ == sibling_type::previous) ? object_target->get_previous_sibling() : object_target->get_next_sibling());
	if (target_sibling != sibling_){
		if (sibling_ != nullptr)
			sibling_->events().unbind(position_event_id_);

		if ((sibling_ = target_sibling) != nullptr){
			position_event_id_ = sibling_->events().bind([this](events::position_change &e){
				if (!e.is_changing())
					do_callback_(reinterpret_cast<WINDOWPOS *>(e.get_message().lParam)->flags);
			});
		}
		else
			position_event_id_ = 0u;

		do_callback_(0u);
	}
}

void winp::ui::sibling_size_and_position_hook::do_callback_(UINT flags){
	if (callback_ == nullptr)
		return;

	if ((flags & (SWP_NOSIZE | SWP_NOMOVE)) == 0u)
		callback_(*this, change_type::all);
	else if ((flags & SWP_NOSIZE) == 0u)
		callback_(*this, change_type::size);
	else if ((flags & SWP_NOMOVE) == 0u)
		callback_(*this, change_type::position);
}

winp::ui::generic_placement_hook::generic_placement_hook(object &target, relative_type relativity)
	: generic_placement_hook(target, alignment_type::top_left, POINT{}, relativity){}

winp::ui::generic_placement_hook::generic_placement_hook(object &target, alignment_type alignment, relative_type relativity)
	: generic_placement_hook(target, alignment, POINT{}, relativity){}

winp::ui::generic_placement_hook::generic_placement_hook(object &target, alignment_type alignment, const POINT &offset, relative_type relativity)
	: generic_target_(target), alignment_(alignment), offset_(offset), relativity_(relativity){
	generic_size_event_id_ = generic_target_.events().bind([this](events::position_change &e){
		if (!e.is_changing() && (reinterpret_cast<WINDOWPOS *>(e.get_message().lParam)->flags & SWP_NOSIZE) == 0u)
			update_();
	});
}

winp::ui::generic_placement_hook::~generic_placement_hook(){
	generic_target_.events().unbind(generic_size_event_id_);
	generic_size_event_id_ = 0u;
}

winp::utility::error_code winp::ui::generic_placement_hook::set_alignment(alignment_type value, const std::function<void(generic_placement_hook &, utility::error_code)> &callback){
	return generic_target_.compute_or_post_task_inside_thread_context([=]{
		return generic_target_.pass_return_value_to_callback(callback, *this, set_alignment_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::ui::generic_placement_hook::alignment_type winp::ui::generic_placement_hook::get_alignment(const std::function<void(alignment_type)> &callback) const{
	return generic_target_.compute_or_post_task_inside_thread_context([=]{
		return generic_target_.pass_return_value_to_callback(callback, alignment_);
	}, (callback != nullptr), alignment_type::top_left);
}

winp::utility::error_code winp::ui::generic_placement_hook::set_offset(const POINT &value, const std::function<void(generic_placement_hook &, utility::error_code)> &callback){
	return set_offset(value.x, value.y, callback);
}

winp::utility::error_code winp::ui::generic_placement_hook::set_offset(int x, int y, const std::function<void(generic_placement_hook &, utility::error_code)> &callback){
	return generic_target_.compute_or_post_task_inside_thread_context([=]{
		return generic_target_.pass_return_value_to_callback(callback, *this, set_offset_(x, y));
	}, (callback != nullptr), utility::error_code::nil);
}

const POINT &winp::ui::generic_placement_hook::get_offset(const std::function<void(const POINT &)> &callback) const{
	return *generic_target_.compute_or_post_task_inside_thread_context([=]{
		return &generic_target_.pass_return_ref_value_to_callback(callback, &offset_);
	}, (callback != nullptr), &offset_);
}

winp::utility::error_code winp::ui::generic_placement_hook::set_relativity(relative_type value, const std::function<void(generic_placement_hook &, utility::error_code)> &callback){
	return generic_target_.compute_or_post_task_inside_thread_context([=]{
		return generic_target_.pass_return_value_to_callback(callback, *this, set_relativity_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::ui::generic_placement_hook::relative_type winp::ui::generic_placement_hook::get_relativity(const std::function<void(relative_type)> &callback) const{
	return generic_target_.compute_or_post_task_inside_thread_context([=]{
		return generic_target_.pass_return_value_to_callback(callback, relativity_);
	}, (callback != nullptr), relative_type::target);
}

winp::utility::error_code winp::ui::generic_placement_hook::set_alignment_(alignment_type value){
	if (alignment_ == value)
		return utility::error_code::nil;

	alignment_ = value;
	update_();

	return utility::error_code::nil;
}

winp::utility::error_code winp::ui::generic_placement_hook::set_offset_(int x, int y){
	if (offset_.x == x && offset_.y == y)
		return utility::error_code::nil;

	offset_ = POINT{ x, y };
	update_();

	return utility::error_code::nil;
}

winp::utility::error_code winp::ui::generic_placement_hook::set_relativity_(relative_type value){
	if (relativity_ == value || !should_react_to_relativity_())
		return utility::error_code::nil;

	relativity_ = value;
	update_();

	return utility::error_code::nil;
}

winp::ui::placement_hook::placement_hook(object &target)
	: placement_hook(target, alignment_type::top_left, POINT{}){}

winp::ui::placement_hook::placement_hook(object &target, alignment_type alignment)
	: placement_hook(target, alignment, POINT{}){}

winp::ui::placement_hook::placement_hook(object &target, alignment_type alignment, const POINT &offset)
	: parent_size_hook(target), generic_placement_hook(target, alignment, offset, relative_type::target){
	callback_ = [this](hook &){
		update_();
	};

	update_();
}

winp::ui::placement_hook::~placement_hook() = default;

void winp::ui::placement_hook::update_(){
	auto surface_target = dynamic_cast<surface *>(&target_);
	if (surface_target == nullptr)
		return;

	SIZE parent_size{};
	auto &target_size = surface_target->get_size();

	auto parent = target_.get_parent();
	if (auto surface_parent = dynamic_cast<surface *>(parent); surface_parent == nullptr){
		RECT dimension{};
		GetClientRect(GetDesktopWindow(), &dimension);
		parent_size = SIZE{ (dimension.right - dimension.left), (dimension.bottom - dimension.top) };
	}
	else//Use client size
		parent_size = surface_parent->get_client_size();

	auto computed_offset = offset_;
	switch (alignment_){
	case alignment_type::top_center:
		computed_offset.x += ((parent_size.cx - target_size.cx) / 2);
		break;
	case alignment_type::top_right:
		computed_offset.x += (parent_size.cx - target_size.cx);
		break;
	case alignment_type::center_left:
		computed_offset.y += ((parent_size.cy - target_size.cy) / 2);
		break;
	case alignment_type::center:
		computed_offset.x += ((parent_size.cx - target_size.cx) / 2);
		computed_offset.y += ((parent_size.cy - target_size.cy) / 2);
		break;
	case alignment_type::center_right:
		computed_offset.x += (parent_size.cx - target_size.cx);
		computed_offset.y += ((parent_size.cy - target_size.cy) / 2);
		break;
	case alignment_type::bottom_left:
		computed_offset.y += (parent_size.cy - target_size.cy);
		break;
	case alignment_type::bottom_center:
		computed_offset.x += ((parent_size.cx - target_size.cx) / 2);
		computed_offset.y += (parent_size.cy - target_size.cy);
		break;
	case alignment_type::bottom_right:
		computed_offset.x += (parent_size.cx - target_size.cx);
		computed_offset.y += (parent_size.cy - target_size.cy);
		break;
	default:
		break;
	}

	surface_target->set_position_(computed_offset.x, computed_offset.y, false);
}

bool winp::ui::placement_hook::should_react_to_relativity_() const{
	return false;
}

winp::ui::parent_fill_hook::parent_fill_hook(object &target)
	: parent_fill_hook(target, SIZE{}){}

winp::ui::parent_fill_hook::parent_fill_hook(object &target, const SIZE &offset)
	: parent_size_hook(target), offset_(offset){
	init_();
}

winp::ui::parent_fill_hook::parent_fill_hook(object &target, const D2D1_SIZE_F &offset)
	: parent_size_hook(target), offset_(offset){
	init_();
}

winp::ui::parent_fill_hook::~parent_fill_hook() = default;

winp::utility::error_code winp::ui::parent_fill_hook::set_offset(const SIZE &value, const std::function<void(parent_fill_hook &, utility::error_code)> &callback){
	return set_offset(value.cx, value.cy, callback);
}

winp::utility::error_code winp::ui::parent_fill_hook::set_offset(int width, int height, const std::function<void(parent_fill_hook &, utility::error_code)> &callback){
	return target_.compute_or_post_task_inside_thread_context([=]{
		return target_.pass_return_value_to_callback(callback, *this, set_offset_(width, height));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::parent_fill_hook::set_offset(const D2D1_SIZE_F &value, const std::function<void(parent_fill_hook &, utility::error_code)> &callback){
	return set_offset(value.width, value.height, callback);
}

winp::utility::error_code winp::ui::parent_fill_hook::set_offset(float width, float height, const std::function<void(parent_fill_hook &, utility::error_code)> &callback){
	return target_.compute_or_post_task_inside_thread_context([=]{
		return target_.pass_return_value_to_callback(callback, *this, set_offset_(width, height));
	}, (callback != nullptr), utility::error_code::nil);
}

const std::variant<SIZE, D2D1_SIZE_F> &winp::ui::parent_fill_hook::get_offset(const std::function<void(const std::variant<SIZE, D2D1_SIZE_F> &)> &callback) const{
	return *target_.compute_or_post_task_inside_thread_context([=]{
		return &target_.pass_return_ref_value_to_callback(callback, &offset_);
	}, (callback != nullptr), &offset_);
}

void winp::ui::parent_fill_hook::init_(){
	callback_ = [this](hook &){
		update_();
	};

	target_.events().bind([this](events::position_change &e){
		if (!e.is_changing() && (reinterpret_cast<WINDOWPOS *>(e.get_message().lParam)->flags & SWP_NOSIZE) == 0u)
			update_();
	});

	update_();
}

winp::utility::error_code winp::ui::parent_fill_hook::set_offset_(int width, int height){
	offset_ = SIZE{ width, height };
	update_();

	return utility::error_code::nil;
}

winp::utility::error_code winp::ui::parent_fill_hook::set_offset_(float width, float height){
	offset_ = D2D1_SIZE_F{ width, height };
	update_();

	return utility::error_code::nil;
}

void winp::ui::parent_fill_hook::update_(){
	auto surface_target = dynamic_cast<surface *>(&target_);
	if (surface_target == nullptr)
		return;

	SIZE parent_size{}, offset{};
	auto parent = target_.get_parent();

	if (auto surface_parent = dynamic_cast<surface *>(parent); surface_parent == nullptr){
		RECT dimension{};
		GetClientRect(GetDesktopWindow(), &dimension);
		parent_size = SIZE{ (dimension.right - dimension.left), (dimension.bottom - dimension.top) };
	}
	else//Use client size
		parent_size = surface_parent->get_client_size();

	if (std::holds_alternative<D2D1_SIZE_F>(offset_)){//Proportional offset
		auto &proportional_offset = std::get<D2D1_SIZE_F>(offset_);
		{//Compute offset
			offset.cx = static_cast<int>(parent_size.cx * proportional_offset.width);
			offset.cy = static_cast<int>(parent_size.cy * proportional_offset.height);
		}
	}
	else//Fixed size
		offset = std::get<SIZE>(offset_);

	surface_target->set_size_((parent_size.cx - offset.cx), (parent_size.cy - offset.cy), false);
}

winp::ui::children_contain_hook::children_contain_hook(object &target)
	: children_contain_hook(target, SIZE{}){}

winp::ui::children_contain_hook::children_contain_hook(object &target, const SIZE &padding)
	: children_size_and_position_hook(target), padding_(padding){
	callback_ = [this](hook &, change_type){
		update_();
	};
}

winp::ui::children_contain_hook::~children_contain_hook() = default;

winp::utility::error_code winp::ui::children_contain_hook::set_padding(const SIZE &value, const std::function<void(children_contain_hook &, utility::error_code)> &callback){
	return target_.compute_or_post_task_inside_thread_context([=]{
		return target_.pass_return_value_to_callback(callback, *this, set_padding_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

const SIZE &winp::ui::children_contain_hook::get_padding(const std::function<void(const SIZE &)> &callback) const{
	return *target_.compute_or_post_task_inside_thread_context([=]{
		return &target_.pass_return_ref_value_to_callback(callback, &padding_);
	}, (callback != nullptr), &padding_);
}

winp::utility::error_code winp::ui::children_contain_hook::set_padding_(const SIZE &value){
	if (padding_.cx != value.cx || padding_.cy != value.cy){
		padding_ = value;
		update_();
	}

	return utility::error_code::nil;
}

void winp::ui::children_contain_hook::update_(){
	auto surface_target = dynamic_cast<surface *>(&target_);
	if (surface_target == nullptr || event_ids_.empty())
		return;

	RECT union_rect{};
	for (auto &info : event_ids_){
		if (auto surface_child = dynamic_cast<surface *>(info.first); surface_child != nullptr){
			auto child_dimension = surface_child->get_dimension();
			UnionRect(&union_rect, &child_dimension, &union_rect);
		}
	}

	auto &target_size = surface_target->get_size();
	auto target_client_size = surface_target->get_client_size();

	surface_target->set_size_((union_rect.right + (target_size.cx - target_client_size.cx) + padding_.cx), (union_rect.bottom + (target_size.cy - target_client_size.cy) + padding_.cy), false);
}

winp::ui::io_hook::io_hook(object &target)
	: hook(target){}

winp::ui::io_hook::~io_hook() = default;

winp::ui::drag_hook::drag_hook(object &target)
	: hook(target){
	target_.insert_hook<io_hook>();
	drag_begin_event_id_ = target_.events().bind([](events::mouse_drag_begin &e){
		return true;
	});

	drag_event_id_ = target_.events().bind([this](events::mouse_drag &e){
		if (auto surface_target = dynamic_cast<surface *>(&target_); surface_target != nullptr){
			auto offset = e.get_offset();
			surface_target->set_position_((surface_target->position_.x + offset.x), (surface_target->position_.y + offset.y), false);
		}
	});
}

winp::ui::drag_hook::~drag_hook(){
	target_.events().unbind(drag_event_id_);
	target_.events().unbind(drag_begin_event_id_);
	drag_begin_event_id_ = drag_event_id_ = 0u;
}

winp::ui::mouse_hover_hook::mouse_hover_hook(object &target, const std::chrono::milliseconds &delay)
	: hook(target), delay_(delay){
	target_.insert_hook<io_hook>();
	move_event_id_ = target_.events().bind([this](events::mouse_move &e){
		remove_hover_();
		if (!e.is_non_client())
			bind_timer_();
	});

	leave_event_id_ = target_.events().bind([this](events::mouse_leave &e){
		remove_hover_();
	});

	down_event_id_ = target_.events().bind([this](events::mouse_down &e){
		remove_hover_();
		if (!e.is_non_client())
			bind_timer_();
	});

	up_event_id_ = target_.events().bind([this](events::mouse_up &e){
		remove_hover_();
		if (!e.is_non_client())
			bind_timer_();
	});

	wheel_event_id_ = target_.events().bind([this](events::mouse_wheel &e){
		remove_hover_();
		bind_timer_();
	});
}

winp::ui::mouse_hover_hook::~mouse_hover_hook(){
	target_.events().unbind(timer_event_id_);
	target_.events().unbind(move_event_id_);
	target_.events().unbind(leave_event_id_);

	target_.events().unbind(down_event_id_);
	target_.events().unbind(up_event_id_);
	target_.events().unbind(wheel_event_id_);

	timer_event_id_ = move_event_id_ = leave_event_id_ = down_event_id_ = up_event_id_ = wheel_event_id_ = 0u;
}

winp::utility::error_code winp::ui::mouse_hover_hook::set_delay(const std::chrono::milliseconds &value, const std::function<void(mouse_hover_hook &, utility::error_code)> &callback){
	return target_.compute_or_post_task_inside_thread_context([=]{
		return target_.pass_return_value_to_callback(callback, *this, set_delay_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

const std::chrono::milliseconds &winp::ui::mouse_hover_hook::get_delay(const std::function<void(const std::chrono::milliseconds &)> &callback) const{
	return *target_.compute_or_post_task_inside_thread_context([=]{
		return &target_.pass_return_ref_value_to_callback(callback, &delay_);
	}, (callback != nullptr), &delay_);
}

winp::utility::error_code winp::ui::mouse_hover_hook::set_delay_(const std::chrono::milliseconds &value){
	delay_ = value;
	return utility::error_code::nil;
}

void winp::ui::mouse_hover_hook::bind_timer_(){
	auto state = ++state_;
	timer_event_id_ = target_.events().bind([=](events::timer &e){
		if (state_ != state){
			e.prevent_default();
			return 0ll;
		}

		if (e.needs_duration())
			return delay_.count();

		timer_event_id_ = 0u;
		if (!is_hovered_)
			target_.trigger_event_<events::mouse_hover>(is_hovered_ = true);

		return 0ll;
	});
}

void winp::ui::mouse_hover_hook::remove_hover_(){
	target_.events().unbind(timer_event_id_);
	++state_;

	if (is_hovered_)
		target_.trigger_event_<events::mouse_hover>(is_hovered_ = false);
}

winp::ui::auto_hide_cursor_hook::auto_hide_cursor_hook(object &target, const std::chrono::milliseconds &delay)
	: hook(target), delay_(delay){
	if (auto hk = target_.insert_hook<mouse_hover_hook>(); hk != nullptr)
		hk->set_delay(delay_);
	
	event_id_ = target_.events().bind([this](events::mouse_hover &e){
		if (e.is_hovered() && !is_hidden_){
			ShowCursor(FALSE);
			is_hidden_ = true;
		}
		else if (!e.is_hovered() && is_hidden_){
			ShowCursor(TRUE);
			is_hidden_ = false;
		}
	});
}

winp::ui::auto_hide_cursor_hook::~auto_hide_cursor_hook(){
	target_.events().unbind(event_id_);
	event_id_ = 0u;

	if (is_hidden_){
		ShowCursor(TRUE);
		is_hidden_ = false;
	}
}

winp::utility::error_code winp::ui::auto_hide_cursor_hook::set_delay(const std::chrono::milliseconds &value, const std::function<void(auto_hide_cursor_hook &, utility::error_code)> &callback){
	return target_.compute_or_post_task_inside_thread_context([=]{
		return target_.pass_return_value_to_callback(callback, *this, set_delay_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

const std::chrono::milliseconds &winp::ui::auto_hide_cursor_hook::get_delay(const std::function<void(const std::chrono::milliseconds &)> &callback) const{
	return *target_.compute_or_post_task_inside_thread_context([=]{
		return &target_.pass_return_ref_value_to_callback(callback, &delay_);
	}, (callback != nullptr), &delay_);
}

winp::utility::error_code winp::ui::auto_hide_cursor_hook::set_delay_(const std::chrono::milliseconds &value){
	delay_ = value;
	if (auto hk = target_.find_hook<mouse_hover_hook>(); hk != nullptr)
		hk->set_delay(delay_);

	return utility::error_code::nil;
}

winp::ui::fullscreen_hook::fullscreen_hook(object &target)
	: hook(target){
	target_.insert_hook<io_hook>();
	if (dynamic_cast<window_surface *>(&target_) != nullptr){
		append_menu_event_id_ = target_.events().bind([this](events::appended_context_menu &e){
			if (!dynamic_cast<menu::appended_popup &>(e.get_popup()).get_popup_target().is_system())
				return;

			if (is_fullscreen_){
				e.get_popup().add_object([this](menu::separator &item){});

				e.get_popup().add_object([this](menu::action_item &item){
					item.set_text(L"Exit Full Screen");
					item.events().bind([this](events::menu_item_select &e){
						escape_fullscreen_();
					});
				});
			}
			else{//Normal window
				e.get_popup().add_object([this](menu::separator &item){});

				e.get_popup().add_object([this](menu::action_item &item){
					item.set_text(L"Enter Full Screen");
					item.events().bind([this](events::menu_item_select &e){
						enter_fullscreen_();
					});
				});
			}
		});

		dbl_click_event_id_ = target_.events().bind([this](events::mouse_dbl_clk &e){
			toggle_fullscreen_();
		});

		key_down_event_id_ = target_.events().bind([this](events::key_down &e){
			switch (e.get_virtual_code()){
			case VK_ESCAPE:
				escape_fullscreen_();
				break;
			case 'F':
				toggle_fullscreen_();
				break;
			default:
				break;
			}
		});

		key_up_event_id_ = target_.events().bind([this](events::key_up &e){
			switch (e.get_virtual_code()){
			case VK_ESCAPE:
				escape_fullscreen_();
				break;
			default:
				break;
			}
		});
	}
}

winp::ui::fullscreen_hook::~fullscreen_hook(){
	target_.events().unbind(append_menu_event_id_);
	target_.events().unbind(dbl_click_event_id_);
	target_.events().unbind(key_down_event_id_);
	target_.events().unbind(key_up_event_id_);

	key_up_event_id_ = key_down_event_id_ = dbl_click_event_id_ = append_menu_event_id_ = 0u;
	escape_fullscreen_();
}

void winp::ui::fullscreen_hook::enter_fullscreen_(){
	auto window_target = dynamic_cast<window_surface *>(&target_);
	if (is_fullscreen_ || window_target == nullptr || !window_target->is_created())
		return;

	if ((saved_.is_maximized = window_target->is_maximized()))
		window_target->toggle_maximized();

	saved_.dimension = window_target->get_dimension();
	saved_.styles = window_target->get_styles(false);
	saved_.extended_styles = window_target->get_styles(true);

	auto hmon = MonitorFromWindow(window_target->get_handle(), MONITOR_DEFAULTTONEAREST);
	MONITORINFO info{ sizeof(MONITORINFO) };

	if (GetMonitorInfoW(hmon, &info) == FALSE)
		return;

	window_target->set_styles((WS_POPUP | WS_VISIBLE), false);
	window_target->set_styles(0, true);

	window_target->set_absolute_position(info.rcMonitor.left, info.rcMonitor.top);
	window_target->set_size((info.rcMonitor.right - info.rcMonitor.left), (info.rcMonitor.bottom - info.rcMonitor.top));

	is_fullscreen_ = true;
}

void winp::ui::fullscreen_hook::escape_fullscreen_(){
	auto window_target = dynamic_cast<window_surface *>(&target_);
	if (!is_fullscreen_ || window_target == nullptr)
		return;

	window_target->set_styles((saved_.styles | WS_VISIBLE), false);
	window_target->set_styles(saved_.extended_styles, true);

	window_target->set_position(saved_.dimension.left, saved_.dimension.top);
	window_target->set_size((saved_.dimension.right - saved_.dimension.left), (saved_.dimension.bottom - saved_.dimension.top));

	if (saved_.is_maximized)
		window_target->maximize();

	is_fullscreen_ = false;
}

void winp::ui::fullscreen_hook::toggle_fullscreen_(){
	if (is_fullscreen_)
		escape_fullscreen_();
	else
		enter_fullscreen_();
}

winp::ui::sibling_placement_hook::sibling_placement_hook(object &target, sibling_type type, relative_type relativity)
	: sibling_placement_hook(target, type, alignment_type::top_left, POINT{}, relativity){}

winp::ui::sibling_placement_hook::sibling_placement_hook(object &target, sibling_type type, alignment_type alignment, relative_type relativity)
	: sibling_placement_hook(target, type, alignment, POINT{}, relativity){}

winp::ui::sibling_placement_hook::sibling_placement_hook(object &target, sibling_type type, alignment_type alignment, const POINT &offset, relative_type relativity)
	: sibling_size_and_position_hook(target, type), generic_placement_hook(target, alignment, offset, relativity){
	callback_ = [this](hook &, change_type){
		update_();
	};

	update_();
}

winp::ui::sibling_placement_hook::~sibling_placement_hook() = default;

bool winp::ui::sibling_placement_hook::should_react_to_relativity_() const{
	return true;
}

void winp::ui::sibling_placement_hook::update_(){
	auto surface_target = dynamic_cast<surface *>(&target_);
	if (surface_target == nullptr || sibling_ == nullptr)
		return;

	auto surface_sibling = dynamic_cast<surface *>(sibling_);
	if (surface_sibling == nullptr)
		return;

	auto &sibling_position = surface_sibling->get_position_();
	auto &target_size = surface_target->get_size_();
	auto sibling_size = surface_sibling->get_client_size();

	switch (alignment_){
	case alignment_type::top_left:
		surface_target->set_position_(
			((relativity_ == relative_type::target) ? (sibling_position.x + offset_.x) : ((sibling_position.x - target_size.cx) + offset_.x)),
			((relativity_ == relative_type::target) ? (sibling_position.y + offset_.y) : ((sibling_position.y - target_size.cy) + offset_.y)),
			false
		);
		break;
	case alignment_type::top_center:
		surface_target->set_position_(
			(sibling_position.x + ((sibling_size.cx - target_size.cx) / 2) + offset_.x),
			((relativity_ == relative_type::target) ? (sibling_position.y + offset_.y) : ((sibling_position.y - target_size.cy) + offset_.y)),
			false
		);
		break;
	case alignment_type::top_right:
		surface_target->set_position_(
			((relativity_ == relative_type::target) ? ((sibling_position.x + (sibling_size.cx - target_size.cx)) + offset_.x) : ((sibling_position.x + sibling_size.cx) + offset_.x)),
			((relativity_ == relative_type::target) ? (sibling_position.y + offset_.y) : ((sibling_position.y - target_size.cy) + offset_.y)),
			false
		);
		break;
	case alignment_type::center_left:
		surface_target->set_position_(
			((relativity_ == relative_type::target) ? (sibling_position.x + offset_.x) : ((sibling_position.x - target_size.cx) + offset_.x)),
			(sibling_position.y + ((sibling_size.cy - target_size.cy) / 2) + offset_.y),
			false
		);
		break;
	case alignment_type::center:
		surface_target->set_position_(
			(sibling_position.x + ((sibling_size.cx - target_size.cx) / 2) + offset_.x),
			(sibling_position.y + ((sibling_size.cy - target_size.cy) / 2) + offset_.y),
			false
		);
		break;
	case alignment_type::center_right:
		surface_target->set_position_(
			((relativity_ == relative_type::target) ? ((sibling_position.x + (sibling_size.cx - target_size.cx)) + offset_.x) : ((sibling_position.x + sibling_size.cx) + offset_.x)),
			(sibling_position.y + ((sibling_size.cy - target_size.cy) / 2) + offset_.y),
			false
		);
		break;
	case alignment_type::bottom_left:
		surface_target->set_position_(
			((relativity_ == relative_type::target) ? (sibling_position.x + offset_.x) : ((sibling_position.x - target_size.cx) + offset_.x)),
			((relativity_ == relative_type::target) ? ((sibling_position.y + (sibling_size.cy - target_size.cy)) + offset_.y) : ((sibling_position.y + sibling_size.cy) + offset_.y)),
			false
		);
		break;
	case alignment_type::bottom_center:
		surface_target->set_position_(
			(sibling_position.x + ((sibling_size.cx - target_size.cx) / 2) + offset_.x),
			((relativity_ == relative_type::target) ? ((sibling_position.y + (sibling_size.cy - target_size.cy)) + offset_.y) : ((sibling_position.y + sibling_size.cy) + offset_.y)),
			false
		);
		break;
	case alignment_type::bottom_right:
		surface_target->set_position_(
			((relativity_ == relative_type::target) ? ((sibling_position.x + (sibling_size.cx - target_size.cx)) + offset_.x) : ((sibling_position.x + sibling_size.cx) + offset_.x)),
			((relativity_ == relative_type::target) ? ((sibling_position.y + (sibling_size.cy - target_size.cy)) + offset_.y) : ((sibling_position.y + sibling_size.cy) + offset_.y)),
			false
		);
		break;
	default:
		break;
	}
}

winp::ui::auto_create_hook::auto_create_hook(object &target)
	: hook(target){}

winp::ui::auto_create_hook::~auto_create_hook() = default;

winp::ui::animation_hook::animation_hook(object &target)
	: animation_hook(target, utility::animation_timing::linear::ease, std::chrono::milliseconds(500)){}

winp::ui::animation_hook::animation_hook(object &target, const easing_type &easing)
	: animation_hook(target, easing, std::chrono::milliseconds(500)){}

winp::ui::animation_hook::animation_hook(object &target, const std::chrono::microseconds &duration)
	: animation_hook(target, utility::animation_timing::linear::ease, duration){}

winp::ui::animation_hook::animation_hook(object &target, const easing_type &easing, const std::chrono::microseconds &duration)
	: hook(target), easing_(easing), duration_(duration){}

winp::ui::animation_hook::~animation_hook() = default;

winp::utility::error_code winp::ui::animation_hook::set_easing(const easing_type &value, const std::function<void(animation_hook &, utility::error_code)> &callback){
	return target_.compute_or_post_task_inside_thread_context([=]{
		return target_.pass_return_value_to_callback(callback, *this, set_easing_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::animation_hook::set_easing(key_type key, const easing_type &value, const std::function<void(animation_hook &, utility::error_code)> &callback){
	return target_.compute_or_post_task_inside_thread_context([=]{
		return target_.pass_return_value_to_callback(callback, *this, set_easing_(key, value));
	}, (callback != nullptr), utility::error_code::nil);
}

const winp::ui::animation_hook::easing_type &winp::ui::animation_hook::get_easing(const std::function<void(const easing_type &)> &callback) const{
	return *target_.compute_or_post_task_inside_thread_context([=]{
		return &target_.pass_return_ref_value_to_callback(callback, &easing_);
	}, (callback != nullptr), &easing_);
}

const winp::ui::animation_hook::easing_type &winp::ui::animation_hook::get_easing(key_type key, const std::function<void(const easing_type &)> &callback) const{
	return *target_.compute_or_post_task_inside_thread_context([=]{
		return &target_.pass_return_ref_value_to_callback(callback, &get_easing(key));
	}, (callback != nullptr), &easing_);
}

winp::utility::error_code winp::ui::animation_hook::set_duration(const std::chrono::microseconds &value, const std::function<void(animation_hook &, utility::error_code)> &callback){
	return target_.compute_or_post_task_inside_thread_context([=]{
		return target_.pass_return_value_to_callback(callback, *this, set_duration_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::animation_hook::set_duration(key_type key, const std::chrono::microseconds &value, const std::function<void(animation_hook &, utility::error_code)> &callback){
	return target_.compute_or_post_task_inside_thread_context([=]{
		return target_.pass_return_value_to_callback(callback, *this, set_duration_(key, value));
	}, (callback != nullptr), utility::error_code::nil);
}

const std::chrono::microseconds &winp::ui::animation_hook::get_duration(const std::function<void(const std::chrono::microseconds &)> &callback) const{
	return *target_.compute_or_post_task_inside_thread_context([=]{
		return &target_.pass_return_ref_value_to_callback(callback, &duration_);
	}, (callback != nullptr), &duration_);
}

const std::chrono::microseconds &winp::ui::animation_hook::get_duration(key_type key, const std::function<void(const std::chrono::microseconds &)> &callback) const{
	return *target_.compute_or_post_task_inside_thread_context([=]{
		return &target_.pass_return_ref_value_to_callback(callback, &get_duration_(key));
	}, (callback != nullptr), &duration_);
}

void winp::ui::animation_hook::set_allowed_state(bool is_allowed){
	target_.execute_task_inside_thread_context([=]{
		is_allowed_ = is_allowed;
	});
}

void winp::ui::animation_hook::set_allowed_state(key_type key, bool is_allowed){
	target_.execute_task_inside_thread_context([=]{
		if (!info_list_.empty()){
			if (auto it = info_list_.find(key); it != info_list_.end())
				it->second.is_allowed = is_allowed;
		}
	});
}

bool winp::ui::animation_hook::is_allowed() const{
	return target_.compute_task_inside_thread_context([=]{
		return is_allowed_;
	});
}

bool winp::ui::animation_hook::is_allowed(key_type key) const{
	return target_.compute_task_inside_thread_context([=]{
		if (info_list_.empty())
			return is_allowed_;

		if (auto it = info_list_.find(key); it != info_list_.end())
			return it->second.is_allowed;

		return is_allowed_;
	});
}

bool winp::ui::animation_hook::is_active(key_type key) const{
	return target_.compute_task_inside_thread_context([=]{
		if (info_list_.empty())
			return false;

		if (auto it = info_list_.find(key); it != info_list_.end())
			return it->second.is_active;

		return false;
	});
}

bool winp::ui::animation_hook::is_active() const{
	return target_.compute_task_inside_thread_context([=]{
		if (info_list_.empty())
			return false;

		for (auto &info : info_list_){
			if (info.second.is_active)
				return true;
		}

		return false;
	});
}

winp::utility::error_code winp::ui::animation_hook::set_easing_(const easing_type &value){
	easing_ = value;
	return utility::error_code::nil;
}

winp::utility::error_code winp::ui::animation_hook::set_easing_(key_type key, const easing_type &value){
	if (auto it = info_list_.find(key); it != info_list_.end())
		it->second.easing = value;
	else
		info_list_[key] = key_info{ true, false, value, duration_, std::any(), 0u };

	return utility::error_code::nil;
}

const winp::ui::animation_hook::easing_type &winp::ui::animation_hook::get_easing_(key_type key) const{
	if (info_list_.empty())
		return easing_;

	if (auto it = info_list_.find(key); it != info_list_.end())
		return it->second.easing;

	return easing_;
}

winp::utility::error_code winp::ui::animation_hook::set_duration_(const std::chrono::microseconds &value){
	duration_ = value;
	return utility::error_code::nil;
}

winp::utility::error_code winp::ui::animation_hook::set_duration_(key_type key, const std::chrono::microseconds &value){
	if (auto it = info_list_.find(key); it != info_list_.end())
		it->second.duration = value;
	else
		info_list_[key] = key_info{ true, false, easing_, value, std::any(), 0u };

	return utility::error_code::nil;
}

const std::chrono::microseconds &winp::ui::animation_hook::get_duration_(key_type key) const{
	if (info_list_.empty())
		return duration_;

	if (auto it = info_list_.find(key); it != info_list_.end())
		return it->second.duration;

	return duration_;
}

winp::ui::animation_hook::key_info &winp::ui::animation_hook::get_(key_type key) const{
	if (auto it = info_list_.find(key); it != info_list_.end())
		return it->second;

	return (info_list_[key] = key_info{ is_allowed_, false, easing_, duration_, std::any(), 0u });
}

winp::ui::animation_hook::key_info *winp::ui::animation_hook::get_existing_(key_type key) const{
	if (info_list_.empty())
		return nullptr;

	if (auto it = info_list_.find(key); it != info_list_.end())
		return &it->second;

	return nullptr;
}

winp::ui::animation_suppression_hook::animation_suppression_hook(object &target, bool once)
	: hook(target), once_(once){}

winp::ui::animation_suppression_hook::~animation_suppression_hook() = default;

bool winp::ui::animation_suppression_hook::is_once(const std::function<void(bool)> &callback) const{
	return target_.compute_or_post_task_inside_thread_context([=]{
		return target_.pass_return_value_to_callback(callback, once_);
	}, (callback != nullptr), false);
}

void winp::ui::animation_suppression_hook::suppress_type(key_type key){
	target_.execute_task_inside_thread_context([=]{
		suppressed_list_[key] = true;
	});
}

void winp::ui::animation_suppression_hook::unsuppress_type(key_type key){
	target_.execute_task_inside_thread_context([=]{
		if (!suppressed_list_.empty())
			suppressed_list_.erase(key);
	});
}

bool winp::ui::animation_suppression_hook::type_is_suppressed(key_type key) const{
	return target_.compute_task_inside_thread_context([=]{
		if (suppressed_list_.empty())
			return true;

		auto it = suppressed_list_.find(key);
		return (it != suppressed_list_.end() && it->second);
	});
}
