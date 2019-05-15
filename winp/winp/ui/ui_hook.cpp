#include "../app/app_object.h"

#include "ui_window_surface.h"

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
	: target_(target){
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
	if (auto parent = target_.get_parent(); parent != nullptr)
		parent->events().unbind(size_event_id_);

	target_.events().unbind(tree_event_id_);
	tree_event_id_ = 0u;
}

void winp::ui::parent_size_hook::bind_size_event_(tree *parent, tree *previous_parent){
	if (previous_parent != nullptr)
		previous_parent->events().unbind(size_event_id_);

	size_event_id_ = 0u;
	if (parent != nullptr){
		size_event_id_ = parent->events().bind([this](events::position_change &e){
			if (callback_ != nullptr && !e.is_changing() && (reinterpret_cast<WINDOWPOS *>(e.get_message().lParam)->flags & SWP_NOSIZE) == 0u)
				callback_(*this);
		});
	}
}

winp::ui::children_size_and_position_hook::children_size_and_position_hook(tree &target, const std::function<void(hook &, change_type)> &callback)
	: children_size_and_position_hook(target){
	callback_ = callback;
}

winp::ui::children_size_and_position_hook::children_size_and_position_hook(tree &target, const std::function<void(change_type)> &callback)
	: children_size_and_position_hook(target){
	callback_ = [=](hook &, change_type change){
		if (callback != nullptr)
			callback(change);
	};
}

winp::ui::children_size_and_position_hook::children_size_and_position_hook(tree &target)
	: target_(target){
	target_.traverse_all_children([this](object &child){
		event_ids_[&child] = child.events().bind([this](events::position_change &e){
			if (callback_ != nullptr && !e.is_changing())
				do_callback_(reinterpret_cast<WINDOWPOS *>(e.get_message().lParam)->flags);
		});
	}, true);

	tree_event_id_ = target_.events().bind([this](events::children_change &e){
		if (!e.is_changing())
			children_change_(e.get_value(), e.get_action());
	});
}

winp::ui::children_size_and_position_hook::children_size_and_position_hook(tree &target, bool)
	: target_(target){
	tree_event_id_ = target_.events().bind([this](events::children_change &e){
		if (!e.is_changing())
			children_change_(e.get_value(), e.get_action());
	});
}

winp::ui::children_size_and_position_hook::~children_size_and_position_hook(){
	if (!event_ids_.empty()){
		for (auto &info : event_ids_)
			info.first->events().unbind(info.second);
		event_ids_.clear();
	}

	target_.events().unbind(tree_event_id_);
	tree_event_id_ = 0u;
}

void winp::ui::children_size_and_position_hook::children_change_(object &child, events::children_change::action_type action){
	if (action == events::children_change::action_type::insert){
		event_ids_[&child] = child.events().bind([this](events::position_change &e){
			if (callback_ != nullptr && !e.is_changing())
				do_callback_(reinterpret_cast<WINDOWPOS *>(e.get_message().lParam)->flags);
		});
	}
	else if (action == events::children_change::action_type::remove && !event_ids_.empty()){
		if (auto it = event_ids_.find(&child); it != event_ids_.end()){
			it->first->events().unbind(it->second);
			event_ids_.erase(it);
		}
	}
}

void winp::ui::children_size_and_position_hook::do_callback_(UINT flags){
	auto target_flags = get_target_flags_();
	if ((flags & target_flags) != target_flags){
		if ((flags & target_flags) == 0u)
			callback_(*this, change_type::both);
		else if ((flags & SWP_NOSIZE) == 0u)
			callback_(*this, change_type::size);
		else//Move
			callback_(*this, change_type::position);
	}
}

UINT winp::ui::children_size_and_position_hook::get_target_flags_() const{
	return (SWP_NOSIZE | SWP_NOMOVE);
}

winp::ui::children_size_hook::children_size_hook(tree &target, const std::function<void(hook &)> &callback)
	: children_size_and_position_hook(target){
	callback_ = [=](hook &h, change_type){
		if (callback != nullptr)
			callback(h);
	};
}

winp::ui::children_size_hook::children_size_hook(tree &target, const std::function<void()> &callback)
	: children_size_and_position_hook(target){
	callback_ = [=](hook &, change_type){
		if (callback != nullptr)
			callback();
	};
}

winp::ui::children_size_hook::~children_size_hook() = default;

UINT winp::ui::children_size_hook::get_target_flags_() const{
	return SWP_NOSIZE;
}

winp::ui::children_position_hook::children_position_hook(tree &target, const std::function<void(hook &)> &callback)
	: children_size_and_position_hook(target){
	callback_ = [=](hook &h, change_type){
		if (callback != nullptr)
			callback(h);
	};
}

winp::ui::children_position_hook::children_position_hook(tree &target, const std::function<void()> &callback)
	: children_size_and_position_hook(target){
	callback_ = [=](hook &, change_type){
		if (callback != nullptr)
			callback();
	};
}

winp::ui::children_position_hook::~children_position_hook() = default;

UINT winp::ui::children_position_hook::get_target_flags_() const{
	return SWP_NOMOVE;
}

winp::ui::placement_hook::placement_hook(object &target)
	: placement_hook(target, alignment_type::top_left, POINT{}){}

winp::ui::placement_hook::placement_hook(object &target, alignment_type alignment)
	: placement_hook(target, alignment, POINT{}){}

winp::ui::placement_hook::placement_hook(object &target, alignment_type alignment, const POINT &offset)
	: parent_size_hook(target), alignment_(alignment), offset_(offset){
	callback_ = [this](hook &){
		update_();
	};

	target_.events().bind([this](events::position_change &e){
		if (!e.is_changing() && (reinterpret_cast<WINDOWPOS *>(e.get_message().lParam)->flags & SWP_NOSIZE) == 0u)
			update_();
	});

	update_();
}

winp::ui::placement_hook::~placement_hook() = default;

winp::utility::error_code winp::ui::placement_hook::set_alignment(alignment_type value, const std::function<void(placement_hook &, utility::error_code)> &callback){
	return target_.compute_or_post_task_inside_thread_context([=]{
		return target_.pass_return_value_to_callback(callback, *this, set_alignment_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::ui::placement_hook::alignment_type winp::ui::placement_hook::get_alignment(const std::function<void(alignment_type)> &callback) const{
	return target_.compute_or_post_task_inside_thread_context([=]{
		return target_.pass_return_value_to_callback(callback, alignment_);
	}, (callback != nullptr), alignment_type::top_left);
}

winp::utility::error_code winp::ui::placement_hook::set_offset(const POINT &value, const std::function<void(placement_hook &, utility::error_code)> &callback){
	return set_offset(value.x, value.y, callback);
}

winp::utility::error_code winp::ui::placement_hook::set_offset(int x, int y, const std::function<void(placement_hook &, utility::error_code)> &callback){
	return target_.compute_or_post_task_inside_thread_context([=]{
		return target_.pass_return_value_to_callback(callback, *this, set_offset_(x, y));
	}, (callback != nullptr), utility::error_code::nil);
}

const POINT &winp::ui::placement_hook::get_offset(const std::function<void(const POINT &)> &callback) const{
	return *target_.compute_or_post_task_inside_thread_context([=]{
		return &target_.pass_return_ref_value_to_callback(callback, &offset_);
	}, (callback != nullptr), &offset_);
}

winp::utility::error_code winp::ui::placement_hook::set_alignment_(alignment_type value){
	if (alignment_ == value)
		return utility::error_code::nil;

	alignment_ = value;
	update_();

	return utility::error_code::nil;
}

winp::utility::error_code winp::ui::placement_hook::set_offset_(int x, int y){
	if (offset_.x == x && offset_.y == y)
		return utility::error_code::nil;

	offset_ = POINT{ x, y };
	update_();

	return utility::error_code::nil;
}

void winp::ui::placement_hook::update_(){
	auto surface_target = dynamic_cast<surface *>(&target_);
	if (surface_target == nullptr)
		return;

	SIZE target_size = surface_target->get_size(), parent_size{};
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

	surface_target->set_position(computed_offset);
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

	surface_target->set_size((parent_size.cx - offset.cx), (parent_size.cy - offset.cy));
}

winp::ui::first_child_contain_hook::first_child_contain_hook(tree &target)
	: first_child_contain_hook(target, SIZE{}){}

winp::ui::first_child_contain_hook::first_child_contain_hook(tree &target, const SIZE &offset)
	: children_size_and_position_hook(target, false), offset_(offset){
	target_.traverse_children([this](object &child){
		if (!event_ids_.empty())
			return false;

		event_ids_[&child] = child.events().bind([this](events::position_change &e){
			if (callback_ != nullptr && !e.is_changing())
				do_callback_(reinterpret_cast<WINDOWPOS *>(e.get_message().lParam)->flags);
		});

		return true;
	}, true);
}

winp::ui::first_child_contain_hook::~first_child_contain_hook() = default;

winp::utility::error_code winp::ui::first_child_contain_hook::set_offset(const SIZE &value, const std::function<void(first_child_contain_hook &, utility::error_code)> &callback){
	return set_offset(value.cx, value.cy, callback);
}

winp::utility::error_code winp::ui::first_child_contain_hook::set_offset(int width, int height, const std::function<void(first_child_contain_hook &, utility::error_code)> &callback){
	return target_.compute_or_post_task_inside_thread_context([=]{
		return target_.pass_return_value_to_callback(callback, *this, set_offset_(width, height));
	}, (callback != nullptr), utility::error_code::nil);
}

const SIZE &winp::ui::first_child_contain_hook::get_offset(const std::function<void(const SIZE &)> &callback) const{
	return *target_.compute_or_post_task_inside_thread_context([=]{
		return &target_.pass_return_ref_value_to_callback(callback, &offset_);
	}, (callback != nullptr), &offset_);
}

void winp::ui::first_child_contain_hook::children_change_(object &child, events::children_change::action_type action){
	if (!((action == events::children_change::action_type::insert && event_ids_.empty()) || (action == events::children_change::action_type::remove && !event_ids_.empty()))){
		children_size_and_position_hook::children_change_(child, action);
		return;
	}

	children_size_and_position_hook::children_change_(child, action);
	if (event_ids_.empty()){//Bind to first child
		target_.traverse_children([this](object &child){
			if (!event_ids_.empty())
				return false;

			event_ids_[&child] = child.events().bind([this](events::position_change &e){
				if (callback_ != nullptr && !e.is_changing())
					do_callback_(reinterpret_cast<WINDOWPOS *>(e.get_message().lParam)->flags);
			});

			return true;
		}, true);
	}
}

UINT winp::ui::first_child_contain_hook::get_target_flags_() const{
	return SWP_NOSIZE;
}

winp::utility::error_code winp::ui::first_child_contain_hook::set_offset_(int width, int height){
	if (offset_.cx == width && offset_.cy == height)
		return utility::error_code::nil;

	offset_ = SIZE{ width, height };
	update_();

	return utility::error_code::nil;
}

void winp::ui::first_child_contain_hook::update_(){
	if (auto surface_target = dynamic_cast<surface *>(&target_); surface_target != nullptr && !event_ids_.empty()){
		if (auto surface_child = dynamic_cast<surface *>(event_ids_.begin()->first); surface_child != nullptr){
			auto child_size = surface_child->get_size();
			surface_target->set_size((child_size.cx - offset_.cx), (child_size.cy - offset_.cy));
		}
	}
}
