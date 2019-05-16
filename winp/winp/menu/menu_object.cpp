#include "../ui/ui_window_surface.h"
#include "../app/app_object.h"

#include "menu_object.h"
#include "menu_link_item.h"

winp::menu::object::object() 
	: object(app::object::get_thread()){}

winp::menu::object::object(thread::object &thread)
	: tree(thread){}

winp::menu::object::~object(){
	destruct();
}

HMENU winp::menu::object::get_handle(const std::function<void(HMENU)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, handle_);
	}, (callback != nullptr), nullptr);
}

winp::utility::error_code winp::menu::object::create_(){
	if (handle_ != nullptr)
		return utility::error_code::nil;

	if ((handle_ = create_handle_()) == nullptr)
		return utility::error_code::action_could_not_be_completed;

	if (thread_.send_message(*this, WM_NCCREATE) == FALSE){
		destroy_();
		return utility::error_code::action_prevented;
	}

	MENUINFO info{
		sizeof(MENUINFO),
		MIM_STYLE,
		MNS_NOTIFYBYPOS
	};

	SetMenuInfo(handle_, &info);
	thread_.send_message(*this, WM_CREATE);

	return utility::error_code::nil;
}

winp::utility::error_code winp::menu::object::destroy_(){
	if (handle_ == nullptr)
		return utility::error_code::nil;

	auto error_code = destroy_handle_();
	if (error_code != utility::error_code::nil)
		return error_code;

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

winp::menu::system_object::system_object() = default;

winp::menu::system_object::system_object(ui::window_surface &target_window)
	: target_window_(&target_window){}

winp::menu::system_object::~system_object() = default;

winp::ui::window_surface *winp::menu::system_object::get_target_window(const std::function<void(ui::window_surface *)> &callback) const{
	return dynamic_cast<const thread::item *>(this)->compute_or_post_task_inside_thread_context([=]{
		return dynamic_cast<const thread::item *>(this)->pass_return_value_to_callback(callback, target_window_);
	}, (callback != nullptr), nullptr);
}

winp::menu::popup::popup()
	: popup(app::object::get_thread()){}

winp::menu::popup::popup(thread::object &thread)
	: object(thread){}

winp::menu::popup::~popup(){
	destruct();
}

HMENU winp::menu::popup::create_handle_(){
	return thread_.get_item_manager().create_menu(*this, nullptr);
}

winp::utility::error_code winp::menu::popup::destroy_handle_(){
	return (thread_.get_item_manager().destroy_menu(handle_, nullptr) ? utility::error_code::nil : utility::error_code::action_could_not_be_completed);
}

winp::menu::system_popup::system_popup()
	: system_popup(app::object::get_thread()){}

winp::menu::system_popup::system_popup(thread::object &thread)
	: popup(thread){}

winp::menu::system_popup::system_popup(ui::window_surface &target_window)
	: system_popup(target_window.get_thread()){
	target_window_ = &target_window;
}

winp::menu::system_popup::~system_popup() = default;

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

HMENU winp::menu::bar::create_handle_(){
	if (auto owner_handle = owner_.get_handle(); owner_handle != nullptr)
		return thread_.get_item_manager().create_menu(*this, owner_handle);
	return nullptr;
}

winp::utility::error_code winp::menu::bar::destroy_handle_(){
	return (thread_.get_item_manager().destroy_menu(handle_, owner_.get_handle()) ? utility::error_code::nil : utility::error_code::action_could_not_be_completed);
}
