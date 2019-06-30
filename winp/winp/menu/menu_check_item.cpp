#include "../app/app_object.h"

#include "menu_group.h"
#include "menu_check_item.h"

winp::menu::check_item::check_item()
	: check_item(app::object::get_thread()){}

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

		if (auto radio_group_parent = get_first_ancestor_of_<radio_group>(nullptr); radio_group_parent != nullptr){
			radio_group_parent->traverse_all_offspring_of<check_item>([this](check_item &offspring){//Remove check state from siblings
				if (&offspring != this)
					offspring.set_checked_state_(false);
			}, true);
		}
	});
}

winp::menu::check_item::check_item(ui::tree &parent)
	: check_item(parent, static_cast<std::size_t>(-1)){}

winp::menu::check_item::check_item(ui::tree &parent, std::size_t index)
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

	return ((SetMenuItemInfoW(handle_, get_insertion_index_(), TRUE, &info) == FALSE) ? utility::error_code::action_could_not_be_completed : utility::error_code::nil);
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

	return ((SetMenuItemInfoW(handle_, get_insertion_index_(), TRUE, &info) == FALSE) ? utility::error_code::action_could_not_be_completed : utility::error_code::nil);
}

winp::utility::error_code winp::menu::check_item::set_checked_state_(bool is_checked){
	if (((get_filtered_states_() & MFS_CHECKED) != 0u) == is_checked)
		return utility::error_code::nil;

	return set_states_(is_checked ? (states_ | MFS_CHECKED) : (states_ & ~MFS_CHECKED));
}

bool winp::menu::check_item::is_radio_() const{
	return (get_first_ancestor_of_<radio_group>(nullptr) != nullptr);
}
