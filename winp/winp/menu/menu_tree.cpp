#include "../app/app_object.h"

#include "menu_tree.h"

winp::menu::tree::tree()
	: tree(app::object::get_thread()){}

winp::menu::tree::tree(thread::object &thread)
	: ui::tree(thread){
	add_event_handler_([this](events::children_change &e){
		if (e.is_changing() && e.get_action() == events::children_change::action_type::insert && dynamic_cast<menu::item *>(&e.get_value()) == nullptr && dynamic_cast<menu::tree *>(&e.get_value()) == nullptr)
			e.prevent_default();//Menu Item or Tree required
	});
}

winp::menu::tree::~tree() = default;

std::size_t winp::menu::tree::get_items_count(const std::function<void(std::size_t)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, get_items_count_());
	}, (callback != nullptr), 0u);
}

std::size_t winp::menu::tree::get_items_count_before(const std::function<void(std::size_t)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, get_items_count_before_());
	}, (callback != nullptr), 0u);
}

winp::menu::item *winp::menu::tree::find_item(UINT id, const std::function<void(menu::item *)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, find_item_(id));
	}, (callback != nullptr), nullptr);
}

winp::menu::item *winp::menu::tree::get_item_at(std::size_t index, const std::function<void(menu::item *)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, get_item_at_(index));
	}, (callback != nullptr), nullptr);
}

UINT winp::menu::tree::get_states(std::size_t index, const std::function<void(UINT)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, get_states_(index));
	}, (callback != nullptr), 0u);
}

UINT winp::menu::tree::get_types(std::size_t index, const std::function<void(UINT)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, get_types_(index));
	}, (callback != nullptr), 0u);
}

void winp::menu::tree::traverse_items(const std::function<bool(menu::item &)> &callback, bool block) const{
	execute_or_post_task_inside_thread_context([&]{
		traverse_items_(callback);
	}, !block);
}

void winp::menu::tree::traverse_all_items(const std::function<void(menu::item &)> &callback, bool block) const{
	traverse_items([callback](menu::item &value){
		callback(value);
		return true;
	}, block);
}

std::size_t winp::menu::tree::get_items_count_() const{
	if (children_.empty())
		return 0u;

	std::size_t count = 0u;
	for (auto child : children_){
		if (auto tree_child = dynamic_cast<tree *>(child); tree_child != nullptr)
			count += tree_child->get_items_count_();
		else if (dynamic_cast<menu::item *>(child) != nullptr)
			++count;
	}

	return count;
}

std::size_t winp::menu::tree::get_items_count_before_() const{
	std::size_t count = 0u;
	if (auto parent = dynamic_cast<tree *>(parent_); parent != nullptr){
		count = parent->get_items_count_before_();
		for (auto child : parent->children_){
			if (child == this)
				break;

			if (auto tree_child = dynamic_cast<tree *>(child); tree_child != nullptr)
				count += tree_child->get_items_count_();
			else if (dynamic_cast<menu::item *>(child) != nullptr)
				++count;
		}
	}

	return count;
}

winp::menu::item *winp::menu::tree::find_item_(UINT id) const{
	if (children_.empty())
		return nullptr;

	menu::item *item = nullptr;
	for (auto child : children_){
		if (auto tree_child = dynamic_cast<tree *>(child); tree_child != nullptr && (item = tree_child->find_item_(id)) != nullptr)
			break;

		if ((item = dynamic_cast<menu::item *>(child)) != nullptr && item->id_ == id)
			break;
	}

	return ((item == nullptr || item->id_ == id) ? item : nullptr);
}

winp::menu::item *winp::menu::tree::get_item_at_(std::size_t index) const{
	menu::item *target_item = nullptr;
	traverse_items_([&](menu::item &item){
		if (index == 0u){//Target reached
			target_item = &item;
			return false;
		}

		--index;
		return true;
	});

	return target_item;
}

UINT winp::menu::tree::get_states_(std::size_t index) const{
	return 0u;
}

UINT winp::menu::tree::get_types_(std::size_t index) const{
	return 0u;
}

void winp::menu::tree::traverse_items_(const std::function<bool(menu::item &)> &callback) const{
	traverse_offspring_of_<menu::item>(callback);
}
