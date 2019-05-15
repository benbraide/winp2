#include "../app/app_object.h"

#include "menu_object.h"

winp::menu::item::item()
	: item(app::object::get_thread()){}

winp::menu::item::item(thread::object &thread)
	: object(thread){}

winp::menu::item::item(tree &parent)
	: item(parent, static_cast<std::size_t>(-1)){}

winp::menu::item::item(tree &parent, std::size_t index)
	: item(parent.get_thread()){
	set_parent(&parent, index);
}

winp::menu::item::~item(){
	destruct();
}

UINT winp::menu::item::get_id(const std::function<void(UINT)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, get_id_());
	}, (callback != nullptr), 0u);
}

winp::utility::error_code winp::menu::item::set_absolute_index(std::size_t value, const std::function<void(item &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_absolute_index_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

std::size_t winp::menu::item::get_absolute_index(const std::function<void(std::size_t)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, get_absolute_index_());
	}, (callback != nullptr), static_cast<std::size_t>(-1));
}

std::size_t winp::menu::item::get_items_count_before(const std::function<void(std::size_t)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, get_items_count_before_());
	}, (callback != nullptr), 0u);
}

winp::utility::error_code winp::menu::item::set_bitmap(HBITMAP value, const std::function<void(item &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_bitmap_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

HBITMAP winp::menu::item::get_bitmap(const std::function<void(HBITMAP)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, bitmap_);
	}, (callback != nullptr), nullptr);
}

winp::utility::error_code winp::menu::item::set_states(UINT value, const std::function<void(item &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_states_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::menu::item::add_states(UINT value, const std::function<void(item &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_states_(states_ | value));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::menu::item::remove_states(UINT value, const std::function<void(item &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_states_(states_ & ~value));
	}, (callback != nullptr), utility::error_code::nil);
}

UINT winp::menu::item::get_states(const std::function<void(UINT)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, get_states_());
	}, (callback != nullptr), 0u);
}

UINT winp::menu::item::get_persistent_states(const std::function<void(UINT)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, get_persistent_states_());
	}, (callback != nullptr), 0u);
}

UINT winp::menu::item::get_filtered_states(const std::function<void(UINT)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, get_filtered_states_());
	}, (callback != nullptr), 0u);
}

UINT winp::menu::item::get_types(const std::function<void(UINT)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, get_types_());
	}, (callback != nullptr), 0u);
}

winp::utility::error_code winp::menu::item::set_enabled_state(bool is_enabled, const std::function<void(item &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_enabled_state_(is_enabled));
	}, (callback != nullptr), utility::error_code::nil);
}

bool winp::menu::item::is_enabled(const std::function<void(bool)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, (get_states_() & MFS_DISABLED) == 0u);
	}, (callback != nullptr), false);
}

bool winp::menu::item::is_owner_drawn(const std::function<void(bool)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, is_owner_drawn_());
	}, (callback != nullptr), false);
}

winp::utility::error_code winp::menu::item::select(const std::function<void(item &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		thread_.send_message(*this, WINP_WM_MENU_ITEM_SELECT);
		return pass_return_value_to_callback(callback, *this, utility::error_code::nil);
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::menu::item::create_(){
	if (handle_ != nullptr)
		return utility::error_code::nil;

	if (parent_ != nullptr && parent_->auto_create() != utility::error_code::nil)
		return utility::error_code::parent_not_created;

	auto object_parent = dynamic_cast<menu::object *>(parent_);
	if (object_parent == nullptr || !object_parent->is_created())
		return utility::error_code::parent_not_created;

	if (auto error_code = generate_id_(); error_code != utility::error_code::nil)
		return error_code;

	if ((handle_ = create_handle_(*object_parent)) == nullptr)
		return utility::error_code::action_could_not_be_completed;

	if (thread_.send_message(*this, WM_NCCREATE) == FALSE){
		destroy_();
		return utility::error_code::action_prevented;
	}

	return utility::error_code::nil;
}

winp::utility::error_code winp::menu::item::destroy_(){
	if (handle_ == nullptr)
		return utility::error_code::nil;

	if (RemoveMenu(handle_, id_, MF_BYCOMMAND) == FALSE)
		return utility::error_code::action_could_not_be_completed;

	handle_ = nullptr;
	thread_.send_message(*this, WM_NCDESTROY);
	thread_.get_item_manager().remove_generated_item_id(*this);

	return utility::error_code::nil;
}

bool winp::menu::item::is_created_() const{
	return (handle_ != nullptr);
}

winp::utility::error_code winp::menu::item::set_parent_value_(ui::tree *value, bool changing){
	if (changing){
		if (value != nullptr && dynamic_cast<menu::tree *>(value) == nullptr)
			return utility::error_code::invalid_parent;
		return ui::object::set_parent_value_(value, true);
	}
	
	if (handle_ != nullptr){//Remove item from menu
		RemoveMenu(handle_, id_, MF_BYCOMMAND);
		handle_ = nullptr;
	}

	return ui::object::set_parent_value_(value, false);
}

winp::utility::error_code winp::menu::item::set_index_value_(std::size_t value, bool changing){
	if (changing)
		return ui::object::set_index_value_(value, true);

	if (handle_ != nullptr){
		RemoveMenu(handle_, id_, MF_BYCOMMAND);
		if (auto object_parent = dynamic_cast<menu::object *>(parent_); object_parent == nullptr || !object_parent->is_created())
			handle_ = nullptr;
		else//Recreate
			handle_ = create_handle_(*object_parent);
	}

	return ui::object::set_index_value_(value, false);
}

winp::utility::error_code winp::menu::item::generate_id_(){
	if ((id_ = thread_.get_item_manager().generate_menu_item_id(*this, id_)) == 0u)
		return utility::error_code::action_could_not_be_completed;
	return utility::error_code::nil;
}

UINT winp::menu::item::fill_basic_info_(menu::object &parent, MENUITEMINFOW &info){
	info.cbSize = sizeof(MENUITEMINFOW);
	info.fMask = (MIIM_FTYPE | MIIM_STATE | MIIM_ID | MIIM_BITMAP);
	info.fType = get_types_();
	info.fState = get_states_();
	info.wID = id_;
	info.hbmpItem = bitmap_;

	UINT index = 0;
	auto absolute_index = get_absolute_index_();

	parent.traverse_items([&](item &child){
		if (&child == this || absolute_index <= child.get_absolute_index())
			return false;

		++index;
		return true;
	}, true);

	return index;
}

UINT winp::menu::item::get_id_() const{
	return id_;
}

winp::utility::error_code winp::menu::item::set_absolute_index_(std::size_t value){
	if (auto tree_parent = dynamic_cast<menu::tree *>(parent_); tree_parent != nullptr)
		return set_index_(value - tree_parent->get_items_count_before());
	return set_index_(value);
}

std::size_t winp::menu::item::get_absolute_index_() const{
	auto index = get_index_();
	if (index == static_cast<std::size_t>(-1))
		return index;

	if (auto parent = dynamic_cast<tree *>(parent_); parent != nullptr)
		return (parent->get_items_count_before() + index);

	return index;
}

std::size_t winp::menu::item::get_items_count_before_() const{
	std::size_t count = 0u;
	if (auto parent = dynamic_cast<tree *>(parent_); parent != nullptr){
		count = parent->get_items_count_before();
		for (auto child : parent->get_children()){
			if (child == this)
				break;

			if (auto tree_child = dynamic_cast<tree *>(child); tree_child != nullptr)
				count += tree_child->get_items_count();
			else if (dynamic_cast<menu::item *>(child) != nullptr)
				++count;
		}
	}

	return count;
}

winp::utility::error_code winp::menu::item::set_bitmap_(HBITMAP value){
	if (value == bitmap_)//No changes
		return utility::error_code::nil;

	bitmap_ = value;
	if (handle_ == nullptr)
		return utility::error_code::nil;

	MENUITEMINFOW info{
		sizeof(MENUITEMINFOW)
	};

	info.fMask = MIIM_BITMAP;
	info.hbmpItem = bitmap_;

	return ((SetMenuItemInfoW(handle_, id_, FALSE, &info) == FALSE) ? utility::error_code::action_could_not_be_completed : utility::error_code::nil);
}

winp::utility::error_code winp::menu::item::set_states_(UINT value){
	value &= ~get_filtered_states_();
	if ((value | get_persistent_states_()) == get_states_())
		return utility::error_code::nil;//No changes

	auto was_enabled = ((states_ & MFS_DISABLED) == 0u);
	auto was_checked = ((states_ & MFS_CHECKED) != 0u);

	states_ = value;
	update_states_();

	if (was_enabled != ((states_ & MFS_DISABLED) == 0u))//Enabled state changed
		thread_.send_message(*this, WM_ENABLE, (was_enabled ? FALSE : TRUE));

	if (was_checked != ((states_ & MFS_CHECKED) != 0u))//Checked state changed
		thread_.send_message(*this, WINP_WM_ITEM_CHECK, (was_checked ? FALSE : TRUE));

	return utility::error_code::nil;
}

winp::utility::error_code winp::menu::item::update_states_(){
	if (handle_ == nullptr)
		return utility::error_code::nil;

	MENUITEMINFOW info{
		sizeof(MENUITEMINFOW),
		MIIM_STATE,
		0,
		get_states_()
	};

	return ((SetMenuItemInfoW(handle_, id_, FALSE, &info) == FALSE) ? utility::error_code::action_could_not_be_completed : utility::error_code::nil);
}

UINT winp::menu::item::get_states_() const{
	if (auto parent = dynamic_cast<tree *>(parent_); parent != nullptr)
		return (((states_ | parent->get_states(get_index_())) & ~get_filtered_states_()) | get_persistent_states_());
	return ((states_ & ~get_filtered_states_()) | get_persistent_states_());
}

UINT winp::menu::item::get_persistent_states_() const{
	return 0u;
}

UINT winp::menu::item::get_filtered_states_() const{
	return MFS_CHECKED;
}

winp::utility::error_code winp::menu::item::update_types_(){
	if (handle_ == nullptr)
		return utility::error_code::nil;

	MENUITEMINFOW info{
		sizeof(MENUITEMINFOW),
		MIIM_FTYPE,
		get_types_()
	};

	return ((SetMenuItemInfoW(handle_, id_, FALSE, &info) == FALSE) ? utility::error_code::action_could_not_be_completed : utility::error_code::nil);
}

UINT winp::menu::item::get_types_() const{
	if (auto parent = dynamic_cast<tree *>(parent_); parent != nullptr)
		return (types_ | parent->get_types(get_index_()));
	return types_;
}

winp::utility::error_code winp::menu::item::set_enabled_state_(bool is_enabled){
	if ((get_filtered_states_() & MFS_DISABLED) != 0u)
		return utility::error_code::nil;

	return set_states_(is_enabled ? (states_ & ~MFS_DISABLED) : (states_ | MFS_DISABLED));
}

bool winp::menu::item::is_owner_drawn_() const{
	return false;
}
