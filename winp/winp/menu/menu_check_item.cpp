#include "../app/app_collection.h"

#include "menu_tree.h"
#include "menu_check_item.h"

winp::menu::check_item::check_item()
	: check_item(app::collection::get_main()->get_thread()){}

winp::menu::check_item::check_item(thread::object &thread)
	: action_item(thread){
	add_event_handler_([this](events::menu_item_select &e){
		if ((get_types_() & MFT_RADIOCHECK) == 0u)//Toggle check state
			set_checked_state_((get_states_() & MFS_CHECKED) == 0u);
		else//Radio check item
			set_checked_state_(true);
	});

	add_event_handler_([this](events::item_check &e){
		if (!e.is_checked())
			return;

		if (auto tree_parent = dynamic_cast<tree *>(parent_); tree_parent != nullptr && (get_types_() & MFT_RADIOCHECK) != 0u){
			tree_parent->traverse_all_items([this](menu::item &child){//Remove check state from siblings
				if (auto check_item_child = dynamic_cast<check_item *>(&child); check_item_child != nullptr && check_item_child != this)
					check_item_child->set_checked_state_(false);
			}, true);
		}
	});
}

winp::menu::check_item::check_item(tree &parent)
	: check_item(parent, static_cast<std::size_t>(-1)){}

winp::menu::check_item::check_item(tree &parent, std::size_t index)
	: check_item(parent.get_thread()){
	set_parent(&parent, index);
}

winp::menu::check_item::~check_item() = default;

winp::utility::error_code winp::menu::check_item::set_checked_bitmap(HBITMAP value, const std::function<void(check_item &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_bitmap_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

HBITMAP winp::menu::check_item::get_checked_bitmap(const std::function<void(HBITMAP)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, bitmap_);
	}, (callback != nullptr), nullptr);
}

winp::utility::error_code winp::menu::check_item::set_unchecked_bitmap(HBITMAP value, const std::function<void(check_item &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_bitmap_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

HBITMAP winp::menu::check_item::get_unchecked_bitmap(const std::function<void(HBITMAP)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, bitmap_);
	}, (callback != nullptr), nullptr);
}

winp::utility::error_code winp::menu::check_item::set_checked_state(bool is_checked, const std::function<void(check_item &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_checked_state_(is_checked));
	}, (callback != nullptr), utility::error_code::nil);
}

bool winp::menu::check_item::is_checked(const std::function<void(bool)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, (get_states_() & MFS_CHECKED) != 0u);
	}, (callback != nullptr), false);
}

UINT winp::menu::check_item::get_filtered_states_() const{
	return (action_item::get_filtered_states_() & ~MFS_CHECKED);
}

winp::utility::error_code winp::menu::check_item::set_checked_bitmap_(HBITMAP value){
	if (value == checked_bitmap_)//No changes
		return utility::error_code::nil;

	checked_bitmap_ = value;
	if (handle_ == nullptr)
		return utility::error_code::nil;

	MENUITEMINFOW info{
		sizeof(MENUITEMINFOW)
	};

	info.fMask = MIIM_CHECKMARKS;
	info.hbmpChecked = checked_bitmap_;

	return ((SetMenuItemInfoW(handle_, id_, FALSE, &info) == FALSE) ? utility::error_code::action_could_not_be_completed : utility::error_code::nil);
}

winp::utility::error_code winp::menu::check_item::set_unchecked_bitmap_(HBITMAP value){
	if (value == unchecked_bitmap_)//No changes
		return utility::error_code::nil;

	unchecked_bitmap_ = value;
	if (handle_ == nullptr)
		return utility::error_code::nil;

	MENUITEMINFOW info{
		sizeof(MENUITEMINFOW)
	};

	info.fMask = MIIM_CHECKMARKS;
	info.hbmpUnchecked = unchecked_bitmap_;

	return ((SetMenuItemInfoW(handle_, id_, FALSE, &info) == FALSE) ? utility::error_code::action_could_not_be_completed : utility::error_code::nil);
}

winp::utility::error_code winp::menu::check_item::set_checked_state_(bool is_checked){
	if ((get_filtered_states_() & MFS_CHECKED) != 0u)
		return utility::error_code::nil;

	return set_states_(is_checked ? (states_ | MFS_CHECKED) : (states_ & ~MFS_CHECKED));
}
