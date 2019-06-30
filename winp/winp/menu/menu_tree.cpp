#include "../app/app_object.h"

#include "menu_object.h"
#include "menu_link_item.h"

winp::menu::tree::tree()
	: tree(app::object::get_thread()){}

winp::menu::tree::tree(thread::object &thread)
	: ui_tree_type(thread){
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
		return pass_return_value_to_callback(callback, find_item_(id, nullptr, true));
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

void winp::menu::tree::child_inserted_(ui::object &child){
	ui::tree::child_inserted_(child);
	if (auto item_child = dynamic_cast<menu::item *>(&child); item_child != nullptr && should_generate_id_(*item_child))
		item_child->local_id_ = generate_id_(*item_child);
}

winp::menu::tree *winp::menu::tree::get_top_() const{
	if (auto tree_parent = dynamic_cast<tree *>(parent_); tree_parent != nullptr)
		return tree_parent->get_top_();
	return const_cast<tree *>(this);
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

winp::menu::item *winp::menu::tree::find_item_(UINT id, menu::item *exclude, bool search_sub_menus) const{
	if (children_.empty())
		return nullptr;

	menu::item *item = nullptr;
	for (auto child : children_){
		if (auto tree_child = dynamic_cast<menu::tree *>(child); tree_child != nullptr && (item = tree_child->find_item_(id, exclude, search_sub_menus)) != nullptr)
			return item;

		if ((item = dynamic_cast<menu::item *>(child)) != nullptr && item != exclude && item->local_id_ == id)
			return item;

		if (!search_sub_menus)
			continue;

		if (auto link_item_child = dynamic_cast<menu::link_item *>(child); link_item_child != nullptr && link_item_child->target_ != nullptr && (item = link_item_child->target_->find_item_(id, exclude, true)) != nullptr)
			return item;//Found in sub-menu
	}

	return nullptr;
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

UINT winp::menu::tree::generate_id_(menu::item &target, std::size_t max_tries) const{
	auto top = get_top_();
	auto is_system = (dynamic_cast<system_popup *>(get_top_()) != nullptr);

	auto id = target.local_id_;
	if (id != 0u && (!is_system || !id_is_reserved_(id)) && top->find_item_(id, &target, is_system) == nullptr)
		return id;

	for (; max_tries > 0u; --max_tries){
		if (is_system)
			id = (thread_.generate_random_integer(1u, 0xEFFFu) & 0xFF00u);
		else//Use full range
			id = thread_.generate_random_integer(1u);

		if ((!is_system || !id_is_reserved_(id)) && top->find_item_(id, &target, is_system) == nullptr)
			break;//ID is unique
	}

	return ((max_tries == 0u) ? 0u : id);
}

bool winp::menu::tree::id_is_reserved_(UINT id) const{
	if (HIWORD(id) == 1u)
		return true;

	switch (id){
	case SC_CLOSE:
	case SC_CONTEXTHELP:
	case SC_DEFAULT:
	case SC_HOTKEY:
	case SC_HSCROLL:
	case SCF_ISSECURE:
	case SC_KEYMENU:
	case SC_MAXIMIZE:
	case SC_MINIMIZE:
	case SC_MONITORPOWER:
	case SC_MOUSEMENU:
	case SC_MOVE:
	case SC_NEXTWINDOW:
	case SC_PREVWINDOW:
	case SC_RESTORE:
	case SC_SCREENSAVE:
	case SC_SIZE:
	case SC_TASKLIST:
	case SC_VSCROLL:
		return true;//Reserved for system menu items
	default:
		break;
	}

	return false;
}

bool winp::menu::tree::should_generate_id_(menu::item &target) const{
	return true;
}

void winp::menu::tree::traverse_items_(const std::function<bool(menu::item &)> &callback) const{
	traverse_offspring_of_<menu::item>(callback);
}
