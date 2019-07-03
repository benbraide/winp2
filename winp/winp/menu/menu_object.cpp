#include "../ui/ui_window_surface.h"
#include "../app/app_object.h"

#include "menu_object.h"
#include "menu_action_item.h"
#include "menu_link_item.h"
#include "menu_separator.h"

winp::menu::object::object() 
	: object(app::object::get_thread()){}

winp::menu::object::object(thread::object &thread)
	: tree(thread){
	add_event_handler_([this](events::menu_init &e){
		e.do_default();

		MSG msg{ nullptr, WINP_WM_INIT_MENU_ITEM };
		traverse_items_([&](menu::item &item){
			if (dynamic_cast<menu::separator *>(&item) != nullptr)
				return true;

			switch (static_cast<events::menu_init_item::enable_type>(trigger_event_with_target_<events::menu_init_item>(item, msg, nullptr).second)){
			case events::menu_init_item::enable_type::enable:
				item.set_enabled_state(true);
				break;
			case events::menu_init_item::enable_type::disable:
				item.set_enabled_state(false);
				break;
			default:
				break;
			}

			return true;
		});
	});
}

winp::menu::object::~object(){
	destruct();
}

HMENU winp::menu::object::get_handle(const std::function<void(HMENU)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, handle_);
	}, (callback != nullptr), nullptr);
}

winp::ui::object *winp::menu::object::get_target(const std::function<void(ui::object *)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, get_target_());
	}, (callback != nullptr), nullptr);
}

winp::utility::error_code winp::menu::object::create_(){
	if (handle_ != nullptr)
		return utility::error_code::nil;

	if (is_destructed_)
		return utility::error_code::object_destructed;

	if ((handle_ = create_handle_()) == nullptr)
		return utility::error_code::action_could_not_be_completed;

	if (thread_.send_message(*this, WM_NCCREATE) == FALSE){
		destroy_();
		return utility::error_code::action_prevented;
	}

	if (dynamic_cast<system_popup *>(this) == nullptr){
		MENUINFO info{
			sizeof(MENUINFO),
			MIM_STYLE,
			MNS_NOTIFYBYPOS
		};

		SetMenuInfo(handle_, &info);
	}

	thread_.get_item_manager().add_menu_(*this);
	thread_.send_message(*this, WM_CREATE);

	if (auto popup_self = dynamic_cast<popup *>(this); popup_self != nullptr){
		if (auto link_item_owner = dynamic_cast<link_item *>(popup_self->owner_); link_item_owner != nullptr)
			link_item_owner->create();
	}

	return utility::error_code::nil;
}

winp::utility::error_code winp::menu::object::destroy_(){
	if (handle_ == nullptr)
		return utility::error_code::nil;

	if (auto error_code = destroy_handle_(); error_code != utility::error_code::nil)
		return error_code;

	thread_.get_item_manager().remove_menu_(*this);
	handle_ = nullptr;
	thread_.send_message(*this, WM_NCDESTROY);

	return utility::error_code::nil;
}

bool winp::menu::object::is_created_() const{
	return (handle_ != nullptr);
}

winp::utility::error_code winp::menu::object::set_parent_value_(ui::tree *value, bool changing){
	return utility::error_code::invalid_parent;
}

std::size_t winp::menu::object::get_items_count_before_() const{
	return 0u;
}

winp::menu::popup::popup()
	: popup(app::object::get_thread()){}

winp::menu::popup::popup(thread::object &thread)
	: object(thread){}

winp::menu::popup::popup(link_item &owner)
	: object(owner.get_thread()){
	owner_ = &owner;
}

winp::menu::popup::~popup(){
	destruct();
}

bool winp::menu::popup::is_system(const std::function<void(bool)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, is_system_());
	}, (callback != nullptr), false);
}

bool winp::menu::popup::is_sub(const std::function<void(bool)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, is_sub_());
	}, (callback != nullptr), false);
}

void winp::menu::popup::child_inserted_(ui::object &child){
	object::child_inserted_(child);
	if (is_modifying_ && dynamic_cast<menu::item *>(&child) != nullptr)
		modified_list_[&child] = true;
}

void winp::menu::popup::child_erased_(ui::object &child){
	if (!modified_list_.empty())
		modified_list_.erase(&child);
	object::child_erased_(child);
}

winp::menu::tree *winp::menu::popup::get_top_() const{
	if (auto link_owner = dynamic_cast<link_item *>(owner_); link_owner != nullptr){
		if (auto tree_parent = dynamic_cast<tree *>(link_owner->parent_); tree_parent != nullptr)
			return tree_parent->get_top_();
	}

	return object::get_top_();
}

winp::ui::object *winp::menu::popup::get_target_() const{
	if (target_ == nullptr || owner_ != nullptr)
		return owner_;

	if (auto menu_target = dynamic_cast<object *>(target_); menu_target != nullptr)
		return menu_target->get_target_();

	return target_;
}

HMENU winp::menu::popup::create_handle_(){
	return CreatePopupMenu();
}

winp::utility::error_code winp::menu::popup::destroy_handle_(){
	if (DestroyMenu(handle_) == FALSE)
		return utility::error_code::action_could_not_be_completed;

	clear_modified_list_();
	return utility::error_code::nil;
}

bool winp::menu::popup::is_system_() const{
	if (auto top_popup = dynamic_cast<popup *>(get_top_()); top_popup != nullptr)
		return (top_popup != this && top_popup->is_system_());
	return false;
}

bool winp::menu::popup::is_sub_() const{
	return (owner_ != nullptr && dynamic_cast<link_item *>(owner_) != nullptr);
}

void winp::menu::popup::clear_modified_list_(){
	if (!modified_list_.empty()){
		auto list = std::move(modified_list_);
		for (auto &item : list)//Remove all modified items
			item.first->set_parent(nullptr);
	}
}

winp::menu::wrapped_popup::wrapped_popup(thread::object &thread)
	: popup(thread){}

winp::menu::wrapped_popup::wrapped_popup(thread::object &thread, HMENU target_handle)
	: wrapped_popup(thread){
	handle_ = target_handle;
	wrap_();
}

winp::menu::wrapped_popup::wrapped_popup(link_item &owner, HMENU target_handle)
	: wrapped_popup(owner.get_thread()){
	handle_ = target_handle;
	owner_ = &owner;
	wrap_();
}

winp::menu::wrapped_popup::~wrapped_popup(){
	destruct();
}

bool winp::menu::wrapped_popup::should_generate_id_(menu::item &target) const{
	return !is_wrapping_;
}

winp::utility::error_code winp::menu::wrapped_popup::destroy_handle_(){
	if (handle_ == nullptr)
		return utility::error_code::nil;

	if (!wrapped_objects_.empty()){//Destruct all wrapped objects
		for (auto wrapped : wrapped_objects_)
			objects_.erase(wrapped);
		wrapped_objects_.clear();
	}

	if (!children_.empty()){
		for (auto child : children_)
			child->destroy();
	}

	return utility::error_code::nil;
}

void winp::menu::wrapped_popup::wrap_(){
	if (handle_ == nullptr)
		return;

	auto count = GetMenuItemCount(handle_);
	if (count == 0u)//Empty list
		return;

	MENUITEMINFOW info{
		sizeof(MENUITEMINFOW),
		(MIIM_ID | MIIM_FTYPE | MIIM_SUBMENU)
	};

	MENUITEMINFOW update_info{
		sizeof(MENUITEMINFOW)
	};

	menu::item *item;

	is_wrapping_ = true;
	wrapped_objects_.reserve(count);

	auto is_system = is_system_();
	for (auto index = 0; index < count; ++index){
		if (GetMenuItemInfoW(handle_, index, TRUE, &info) == FALSE)
			continue;

		if (info.hSubMenu != nullptr)//Sub-menu
			item = new wrapped_link_item(*this, index);
		else if ((info.fType & MFT_SEPARATOR) == 0u)//Action
			item = new wrapped_action_item(*this, index);
		else//Separator
			item = new wrapped_separator(*this, index);

		if (item == nullptr)
			continue;

		objects_[item].reset(item);
		wrapped_objects_.push_back(item);

		if (is_system){
			if ((item->local_id_ = info.wID) == 0u && (item->local_id_ = generate_id_(*item, info.wID)) == 0u){//Update data
				update_info.fMask = MIIM_DATA;
				update_info.dwItemData = reinterpret_cast<ULONG_PTR>(item);
				SetMenuItemInfoW(handle_, index, TRUE, &update_info);
			}
			else if (item->local_id_ != info.wID){//Update Id
				update_info.fMask = MIIM_ID;
				update_info.wID = item->local_id_;
				SetMenuItemInfoW(handle_, index, TRUE, &update_info);
			}
		}
		else if ((item->local_id_ = generate_id_(*item, info.wID)) == 0u){//Update data
			update_info.fMask = MIIM_DATA;
			update_info.dwItemData = reinterpret_cast<ULONG_PTR>(item);
			SetMenuItemInfoW(handle_, index, TRUE, &update_info);
		}
		else if (item->local_id_ != info.wID){//Update Id
			update_info.fMask = MIIM_ID;
			update_info.wID = item->local_id_;
			SetMenuItemInfoW(handle_, index, TRUE, &update_info);
		}
	}

	is_wrapping_ = false;
}

winp::menu::system_popup::system_popup(ui::window_surface &owner)
	: wrapped_popup(owner.get_thread()){
	owner_ = &owner;
}

winp::menu::system_popup::~system_popup() = default;

HMENU winp::menu::system_popup::create_handle_(){
	if (auto owner_handle = dynamic_cast<ui::window_surface *>(owner_)->get_handle(); owner_handle != nullptr){
		GetSystemMenu(owner_handle, TRUE);//Reset menu
		handle_ = GetSystemMenu(owner_handle, FALSE);
		wrap_();
	}

	return handle_;
}

winp::menu::tree *winp::menu::system_popup::get_top_() const{
	return const_cast<system_popup *>(this);
}

bool winp::menu::system_popup::is_system_() const{
	return true;
}

bool winp::menu::system_popup::is_sub_() const{
	return false;
}

winp::menu::bar::bar(ui::window_surface &owner)
	: object(owner.get_thread()), owner_(owner){}

winp::menu::bar::~bar(){
	destruct();
}

const winp::ui::window_surface &winp::menu::bar::get_owner(const std::function<void(const ui::window_surface &)> &callback) const{
	return *compute_or_post_task_inside_thread_context([=]{
		return &pass_return_ref_value_to_callback(callback, &owner_);
	}, (callback != nullptr), &owner_);
}

winp::ui::window_surface &winp::menu::bar::get_owner(const std::function<void(ui::window_surface &)> &callback){
	return *compute_or_post_task_inside_thread_context([=]{
		return &pass_return_ref_value_to_callback(callback, &owner_);
	}, (callback != nullptr), &owner_);
}

void winp::menu::bar::child_inserted_(ui::object &child){
	object::child_inserted_(child);
	if (auto owner_handle = owner_.get_handle(); owner_handle != nullptr && handle_ != nullptr)
		DrawMenuBar(owner_handle);
}

void winp::menu::bar::child_erased_(ui::object &child){
	if (auto owner_handle = owner_.get_handle(); owner_handle != nullptr && handle_ != nullptr)
		DrawMenuBar(owner_handle);
	object::child_erased_(child);
}

winp::ui::object *winp::menu::bar::get_target_() const{
	return &owner_;
}

HMENU winp::menu::bar::create_handle_(){
	auto value = CreateMenu();
	if (value == nullptr)
		return nullptr;

	if (auto owner_handle = owner_.get_handle(); owner_handle != nullptr){
		SetMenu(owner_handle, value);
		DrawMenuBar(owner_handle);
	}

	return value;
}

winp::utility::error_code winp::menu::bar::destroy_handle_(){
	if (DestroyMenu(handle_) == FALSE)
		return utility::error_code::action_could_not_be_completed;

	if (auto owner_handle = owner_.get_handle(); owner_handle != nullptr)
		SetMenu(owner_handle, nullptr);

	return utility::error_code::nil;
}
