#include "../app/app_object.h"
#include "../utility/animation_timing.h"

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

	surface_target->set_size((parent_size.cx - offset.cx), (parent_size.cy - offset.cy));
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

	surface_target->set_size((union_rect.right + (target_size.cx - target_client_size.cx) + padding_.cx), (union_rect.bottom + (target_size.cy - target_client_size.cy) + padding_.cy));
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
			if (auto shk = target_.insert_hook<animation_suppression_hook>(); shk != nullptr){
				shk->suppress_type<POINT>();
				surface_target->offset_position(e.get_offset());
			}
		}
	});
}

winp::ui::drag_hook::~drag_hook(){
	target_.events().unbind(drag_event_id_);
	target_.events().unbind(drag_begin_event_id_);
	drag_begin_event_id_ = drag_event_id_ = 0u;
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

	auto sibling_position = surface_sibling->get_position();
	auto target_size = surface_target->get_size(), sibling_size = surface_sibling->get_client_size();

	switch (alignment_){
	case alignment_type::top_left:
		surface_target->set_position(
			((relativity_ == relative_type::target) ? (sibling_position.x + offset_.x) : ((sibling_position.x - target_size.cx) + offset_.x)),
			((relativity_ == relative_type::target) ? (sibling_position.y + offset_.y) : ((sibling_position.y - target_size.cy) + offset_.y))
		);
		break;
	case alignment_type::top_center:
		surface_target->set_position(
			(sibling_position.x + ((sibling_size.cx - target_size.cx) / 2) + offset_.x),
			((relativity_ == relative_type::target) ? (sibling_position.y + offset_.y) : ((sibling_position.y - target_size.cy) + offset_.y))
		);
		break;
	case alignment_type::top_right:
		surface_target->set_position(
			((relativity_ == relative_type::target) ? ((sibling_position.x + (sibling_size.cx - target_size.cx)) + offset_.x) : ((sibling_position.x + sibling_size.cx) + offset_.x)),
			((relativity_ == relative_type::target) ? (sibling_position.y + offset_.y) : ((sibling_position.y - target_size.cy) + offset_.y))
		);
		break;
	case alignment_type::center_left:
		surface_target->set_position(
			((relativity_ == relative_type::target) ? (sibling_position.x + offset_.x) : ((sibling_position.x - target_size.cx) + offset_.x)),
			(sibling_position.y + ((sibling_size.cy - target_size.cy) / 2) + offset_.y)
		);
		break;
	case alignment_type::center:
		surface_target->set_position(
			(sibling_position.x + ((sibling_size.cx - target_size.cx) / 2) + offset_.x),
			(sibling_position.y + ((sibling_size.cy - target_size.cy) / 2) + offset_.y)
		);
		break;
	case alignment_type::center_right:
		surface_target->set_position(
			((relativity_ == relative_type::target) ? ((sibling_position.x + (sibling_size.cx - target_size.cx)) + offset_.x) : ((sibling_position.x + sibling_size.cx) + offset_.x)),
			(sibling_position.y + ((sibling_size.cy - target_size.cy) / 2) + offset_.y)
		);
		break;
	case alignment_type::bottom_left:
		surface_target->set_position(
			((relativity_ == relative_type::target) ? (sibling_position.x + offset_.x) : ((sibling_position.x - target_size.cx) + offset_.x)),
			((relativity_ == relative_type::target) ? ((sibling_position.y + (sibling_size.cy - target_size.cy)) + offset_.y) : ((sibling_position.y + sibling_size.cy) + offset_.y))
		);
		break;
	case alignment_type::bottom_center:
		surface_target->set_position(
			(sibling_position.x + ((sibling_size.cx - target_size.cx) / 2) + offset_.x),
			((relativity_ == relative_type::target) ? ((sibling_position.y + (sibling_size.cy - target_size.cy)) + offset_.y) : ((sibling_position.y + sibling_size.cy) + offset_.y))
		);
		break;
	case alignment_type::bottom_right:
		surface_target->set_position(
			((relativity_ == relative_type::target) ? ((sibling_position.x + (sibling_size.cx - target_size.cx)) + offset_.x) : ((sibling_position.x + sibling_size.cx) + offset_.x)),
			((relativity_ == relative_type::target) ? ((sibling_position.y + (sibling_size.cy - target_size.cy)) + offset_.y) : ((sibling_position.y + sibling_size.cy) + offset_.y))
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

const winp::ui::animation_hook::easing_type &winp::ui::animation_hook::get_easing(const std::function<void(const easing_type &)> &callback) const{
	return *target_.compute_or_post_task_inside_thread_context([=]{
		return &target_.pass_return_ref_value_to_callback(callback, &easing_);
	}, (callback != nullptr), &easing_);
}

winp::utility::error_code winp::ui::animation_hook::set_duration(const std::chrono::microseconds &value, const std::function<void(animation_hook &, utility::error_code)> &callback){
	return target_.compute_or_post_task_inside_thread_context([=]{
		return target_.pass_return_value_to_callback(callback, *this, set_duration_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

const std::chrono::microseconds &winp::ui::animation_hook::get_duration(const std::function<void(const std::chrono::microseconds &)> &callback) const{
	return *target_.compute_or_post_task_inside_thread_context([=]{
		return &target_.pass_return_ref_value_to_callback(callback, &duration_);
	}, (callback != nullptr), &duration_);
}

void winp::ui::animation_hook::allow_type(key_type key){
	target_.execute_task_inside_thread_context([=]{
		allowed_list_[key] = true;
	});
}

void winp::ui::animation_hook::disallow_type(key_type key){
	target_.execute_task_inside_thread_context([=]{
		if (!allowed_list_.empty()){
			if (auto it = allowed_list_.find(key); it != allowed_list_.end())
				it->second = false;
		}
	});
}

bool winp::ui::animation_hook::type_is_allowed(key_type key) const{
	return target_.compute_task_inside_thread_context([=]{
		if (allowed_list_.empty())
			return true;

		auto it = allowed_list_.find(key);
		return (it != allowed_list_.end() && it->second);
	});
}

winp::utility::error_code winp::ui::animation_hook::set_easing_(const easing_type &value){
	easing_ = value;
	return utility::error_code::nil;
}

winp::utility::error_code winp::ui::animation_hook::set_duration_(const std::chrono::microseconds &value){
	duration_ = value;
	return utility::error_code::nil;
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
		if (!suppressed_list_.empty()){
			if (auto it = suppressed_list_.find(key); it != suppressed_list_.end())
				it->second = false;
		}
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
