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

UINT winp::menu::object::get_insertion_offset_() const{
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
	return ((DestroyMenu(handle_) == FALSE) ? utility::error_code::action_could_not_be_completed : utility::error_code::nil);
}

bool winp::menu::popup::is_system_() const{
	return false;
}

bool winp::menu::popup::is_sub_() const{
	return (owner_ != nullptr && dynamic_cast<link_item *>(owner_) != nullptr);
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

UINT winp::menu::wrapped_popup::get_insertion_offset_() const{
	return insertion_offset_;
}

void winp::menu::wrapped_popup::wrap_(){
	if (handle_ == nullptr)
		return;

	if ((insertion_offset_ = GetMenuItemCount(handle_)) == 0u)//Empty list
		return;

	MENUITEMINFOW info{
		sizeof(MENUITEMINFOW),
		(MIIM_ID | MIIM_FTYPE | MIIM_SUBMENU)
	};

	MENUITEMINFOW update_info{
		sizeof(MENUITEMINFOW),
		MIIM_DATA
	};

	menu::item *item;
	auto is_system = is_system_();

	is_wrapping_ = true;
	wrapped_objects_.reserve(insertion_offset_);

	for (auto index = 0u; index < insertion_offset_; ++index){
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

		if ((item->local_id_ = info.wID) == 0u && is_system){//Update data
			update_info.dwItemData = reinterpret_cast<ULONG_PTR>(item);
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

winp::menu::appended_popup::appended_popup(popup &popup_target)
	: popup(popup_target.get_thread()), popup_target_(popup_target){
	handle_ = popup_target_.handle_;
	popup_target_.events().bind([this](events::destroy &e){
		handle_ = nullptr;
		e.unbind_on_exit();
	});
}

winp::menu::appended_popup::~appended_popup(){
	destruct();
}

const winp::menu::popup &winp::menu::appended_popup::get_popup_target(const std::function<void(const popup &)> &callback) const{
	return *compute_or_post_task_inside_thread_context([=]{
		return &pass_return_ref_value_to_callback(callback, &popup_target_);
	}, (callback != nullptr), &popup_target_);
}

winp::menu::popup &winp::menu::appended_popup::get_popup_target(const std::function<void(popup &)> &callback){
	return *compute_or_post_task_inside_thread_context([=]{
		return &pass_return_ref_value_to_callback(callback, &popup_target_);
	}, (callback != nullptr), &popup_target_);
}

winp::utility::error_code winp::menu::appended_popup::create_(){
	return utility::error_code::nil;
}

winp::utility::error_code winp::menu::appended_popup::destroy_(){
	if (handle_ == nullptr)
		return utility::error_code::nil;

	if (!children_.empty()){
		for (auto child : children_)
			child->destroy();
	}

	handle_ = nullptr;
	return utility::error_code::nil;
}

winp::ui::object *winp::menu::appended_popup::get_target_() const{
	return popup_target_.get_target_();
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

winp::ui::object *winp::menu::bar::get_target_() const{
	return &owner_;
}

HMENU winp::menu::bar::create_handle_(){
	auto owner_handle = owner_.get_handle();
	if (owner_handle == nullptr)
		return nullptr;

	auto value = CreateMenu();
	if (value == nullptr)
		return nullptr;

	SetMenu(owner_handle, value);
	DrawMenuBar(owner_handle);

	return value;
}

winp::utility::error_code winp::menu::bar::destroy_handle_(){
	if (DestroyMenu(handle_) == FALSE)
		return utility::error_code::action_could_not_be_completed;

	SetMenu(owner_.get_handle(), nullptr);
	return utility::error_code::nil;
}
