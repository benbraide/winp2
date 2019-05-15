#include "../app/app_collection.h"

#include "../ui/ui_non_window_surface.h"
#include "../control/control_object.h"

#include "../menu/menu_separator.h"
#include "../menu/menu_link_item.h"

winp::thread::item_manager::item_manager(object &thread)
	: item_manager(thread, thread.get_local_id()){}

winp::thread::item_manager::item_manager(object &thread, DWORD thread_id)
	: thread_(thread){
	hook_handle_ = SetWindowsHookExW(WH_CBT, hook_entry_, nullptr, thread_id);
}

const winp::thread::object &winp::thread::item_manager::get_thread() const{
	return thread_;
}

winp::thread::object &winp::thread::item_manager::get_thread(){
	return thread_;
}

bool winp::thread::item_manager::is_thread_context() const{
	return thread_.is_thread_context();
}

const RECT &winp::thread::item_manager::get_update_rect() const{
	if (!is_thread_context())
		throw utility::error_code::outside_thread_context;
	return update_rect_;
}

HMENU winp::thread::item_manager::create_menu(menu::object &owner, HWND target){
	if (!is_thread_context())
		throw utility::error_code::outside_thread_context;

	auto value = ((target == nullptr) ? CreatePopupMenu() : CreateMenu());
	if (value == nullptr)
		return nullptr;

	menu_items_[value];
	menus_[value] = &owner;

	if (target != nullptr){
		SetMenu(target, value);
		DrawMenuBar(target);
	}

	return value;
}

bool winp::thread::item_manager::destroy_menu(HMENU handle, HWND target){
	if (!is_thread_context())
		throw utility::error_code::outside_thread_context;

	if (DestroyMenu(handle) == FALSE)
		return false;

	if (target != nullptr)
		SetMenu(target, nullptr);

	if (!menus_.empty())
		menus_.erase(handle);

	if (!menu_items_.empty())
		menu_items_.erase(handle);

	return true;
}

void winp::thread::item_manager::add_menu(menu::object &owner){
	auto handle = owner.get_handle();
	if (handle != nullptr){
		menu_items_[handle];
		menus_[handle] = &owner;
	}
}

void winp::thread::item_manager::remove_menu(menu::object &owner){
	auto handle = owner.get_handle();
	if (handle == nullptr)
		return;

	if (!menus_.empty())
		menus_.erase(handle);

	if (!menu_items_.empty())
		menu_items_.erase(handle);
}

UINT winp::thread::item_manager::generate_menu_item_id(menu::item &target, UINT id, std::size_t max_tries){
	if (!is_thread_context())
		throw utility::error_code::outside_thread_context;

	auto menu_object_parent = dynamic_cast<menu::object *>(target.get_parent());
	if (menu_object_parent == nullptr || menu_object_parent->get_handle() == nullptr)
		return 0u;

	auto is_system_menu = false;
	std::unordered_map<HMENU, menu_items_map_type>::iterator it;
	if (auto system_parent = dynamic_cast<menu::system_object *>(menu_object_parent); system_parent != nullptr){
		if (auto target_window = system_parent->get_target_window(); target_window != nullptr){
			it = menu_items_.find(GetSystemMenu(target_window->handle_, FALSE));
			is_system_menu = true;
		}
		else
			it = menu_items_.find(menu_object_parent->get_handle());
	}
	else
		it = menu_items_.find(menu_object_parent->get_handle());

	if (it == menu_items_.end())
		return 0u;

	if (id == 0u || it->second.find(id) != it->second.end()){//Generate new
		for (; max_tries > 0u; --max_tries){
			id = (thread_.generate_random_integer(1u, 0xEFFFu) & 0xFF00u);
			/*if (is_system_menu)
				id = (thread_.generate_random_integer(1u, 0xEFFFu) & 0xFF00u);
			else
				id = thread_.generate_random_integer(1u, std::numeric_limits<UINT>::max());*/

			if (!menu_item_id_is_reserved_(id) && it->second.find(id) == it->second.end())
				break;//ID is unique
		}

		if (max_tries == 0u)
			return 0u;
	}

	it->second[id] = &target;
	return id;
}

void winp::thread::item_manager::add_generated_item_id(menu::item &target){
	if (!is_thread_context())
		throw utility::error_code::outside_thread_context;

	if (target.get_id() == 0u)
		return;

	auto menu_object_parent = dynamic_cast<menu::object *>(target.get_parent());
	if (menu_object_parent == nullptr || menu_object_parent->get_handle() == nullptr)
		return;

	std::unordered_map<HMENU, menu_items_map_type>::iterator it;
	if (auto system_parent = dynamic_cast<menu::system_object *>(menu_object_parent); system_parent != nullptr){
		if (auto target_window = system_parent->get_target_window(); target_window != nullptr)
			it = menu_items_.find(GetSystemMenu(target_window->handle_, FALSE));
		else
			it = menu_items_.find(menu_object_parent->get_handle());
	}
	else
		it = menu_items_.find(menu_object_parent->get_handle());

	if (it != menu_items_.end())
		it->second[target.get_id()] = &target;
}

void winp::thread::item_manager::remove_generated_item_id(menu::item &target){
	if (!is_thread_context())
		throw utility::error_code::outside_thread_context;

	if (menu_items_.empty())
		return;

	auto menu_object_parent = dynamic_cast<menu::object *>(target.get_parent());
	if (menu_object_parent == nullptr || menu_object_parent->get_handle() == nullptr)
		return;

	auto it = menu_items_.find(menu_object_parent->get_handle());
	if (it != menu_items_.end())
		it->second.erase(target.get_id());
}

winp::ui::window_surface *winp::thread::item_manager::find_window_(HWND handle, bool cache) const{
	if (windows_.empty())
		return nullptr;

	if (handle == window_cache_.handle)
		return window_cache_.object;

	auto it = windows_.find(handle);
	if (it == windows_.end())
		return nullptr;

	if (cache){
		window_cache_.handle = handle;
		window_cache_.object = it->second;
	}

	return it->second;
}

winp::menu::item *winp::thread::item_manager::find_menu_item_(menu::object &menu, UINT id) const{
	HMENU handle;
	if (auto system_parent = dynamic_cast<menu::system_object *>(&menu); system_parent != nullptr){
		if (auto target_window = system_parent->get_target_window(); target_window != nullptr)
			handle = GetSystemMenu(target_window->handle_, FALSE);
		else
			handle = menu.get_handle();
	}
	else
		handle = menu.get_handle();

	return find_menu_item_(handle, id);
}

winp::menu::item *winp::thread::item_manager::find_menu_item_(HMENU handle, UINT id) const{
	if (menu_items_.empty())
		return nullptr;

	auto it = menu_items_.find(handle);
	if (it == menu_items_.end())
		return nullptr;

	auto item_it = it->second.find(id);
	return ((item_it == it->second.end()) ? nullptr : item_it->second);
}

winp::menu::item *winp::thread::item_manager::find_sub_menu_item_(HMENU handle, UINT id) const{
	if (menu_items_.empty())
		return nullptr;

	auto it = menu_items_.find(handle);
	if (it == menu_items_.end())
		return nullptr;

	menu::item *item = nullptr;
	for (auto &info : it->second){
		if (auto link_item = dynamic_cast<menu::link_item *>(info.second); link_item != nullptr){
			if ((item = find_menu_item_(link_item->get_target().get_handle(), id)) != nullptr)
				break;

			if ((item = find_sub_menu_item_(link_item->get_target().get_handle(), id)) != nullptr)
				break;
		}
	}

	return item;
}

bool winp::thread::item_manager::is_dialog_message_(MSG &msg) const{
	return (focused_window_ != nullptr && focused_window_->is_dialog_message_(msg));
}

LRESULT winp::thread::item_manager::handle_thread_message_(HWND handle, UINT message, WPARAM wparam, LPARAM lparam){
	switch (message){
	case WINP_WM_SEND_MESSAGE:
		return dispatch_message_(*reinterpret_cast<item *>(lparam), *reinterpret_cast<MSG *>(wparam));
	case WINP_WM_POST_MESSAGE:
		dispatch_message_(*reinterpret_cast<item *>(lparam), *reinterpret_cast<MSG *>(wparam));
		delete reinterpret_cast<MSG *>(wparam);
		return 0;
	default:
		break;
	}

	return CallWindowProcW(DefWindowProcW, handle, message, wparam, lparam);
}

LRESULT winp::thread::item_manager::dispatch_message_(item &target, MSG &msg){
	auto window_target = dynamic_cast<ui::window_surface *>(&target);
	switch (msg.message){
	case WM_NCCREATE:
		return get_result_(trigger_event_with_value_<events::create>(target, TRUE, msg, ((window_target == nullptr) ? nullptr : thread_.get_class_entry_(window_target->get_class_name()))), FALSE);
	case WM_NCDESTROY:
		return window_destroyed_(target, msg);
	case WM_CLOSE:
		return trigger_event_<events::close>(target, msg, ((window_target == nullptr) ? nullptr : thread_.get_class_entry_(window_target->get_class_name()))).second;
	case WM_SETCURSOR:
		return set_cursor_(target, msg);
	case WINP_WM_GET_BACKGROUND_BRUSH:
		return trigger_event_<events::background_brush>(target, msg, nullptr).second;
	case WINP_WM_GET_BACKGROUND_COLOR:
		return trigger_event_<events::background_color>(target, msg, nullptr).second;
	case WM_ERASEBKGND:
		return erase_background_(target, target, msg);
	case WM_PAINT:
		return paint_(target, target, msg, true);
	case WM_STYLECHANGING:
		return style_changing_(target, msg);
	case WM_WINDOWPOSCHANGING:
		return position_change_(target, msg, true);
	case WM_WINDOWPOSCHANGED:
		return position_change_(target, msg, false);
	case WM_NCHITTEST:
		return trigger_event_<events::hit_test>(target, msg, ((window_target == nullptr) ? nullptr : thread_.get_class_entry_(window_target->get_class_name()))).second;
	case WM_NCMOUSELEAVE:
	case WM_MOUSELEAVE:
		return mouse_leave_(target, msg);
	case WM_NCMOUSEMOVE:
	case WM_MOUSEMOVE:
		return mouse_move_(target, msg);
	case WM_NCLBUTTONDOWN:
		return mouse_<ui::window_surface, events::mouse_down>(target, msg, events::mouse::button_type_left, true, thread_);
	case WM_LBUTTONDOWN:
		return mouse_<ui::window_surface, events::mouse_down>(target, msg, events::mouse::button_type_left, false, thread_);
	case WM_NCMBUTTONDOWN:
		return mouse_<ui::window_surface, events::mouse_down>(target, msg, events::mouse::button_type_middle, true, thread_);
	case WM_MBUTTONDOWN:
		return mouse_<ui::window_surface, events::mouse_down>(target, msg, events::mouse::button_type_middle, false, thread_);
	case WM_NCRBUTTONDOWN:
		return mouse_<ui::window_surface, events::mouse_down>(target, msg, events::mouse::button_type_right, true, thread_);
	case WM_RBUTTONDOWN:
		return mouse_<ui::window_surface, events::mouse_down>(target, msg, events::mouse::button_type_right, false, thread_);
	case WM_NCLBUTTONUP:
		return mouse_<ui::window_surface, events::mouse_up>(target, msg, events::mouse::button_type_left, true, thread_);
	case WM_LBUTTONUP:
		return mouse_<ui::window_surface, events::mouse_up>(target, msg, events::mouse::button_type_left, false, thread_);
	case WM_NCMBUTTONUP:
		return mouse_<ui::window_surface, events::mouse_up>(target, msg, events::mouse::button_type_middle, true, thread_);
	case WM_MBUTTONUP:
		return mouse_<ui::window_surface, events::mouse_up>(target, msg, events::mouse::button_type_middle, false, thread_);
	case WM_NCRBUTTONUP:
		return mouse_<ui::window_surface, events::mouse_up>(target, msg, events::mouse::button_type_right, true, thread_);
	case WM_RBUTTONUP:
		return mouse_<ui::window_surface, events::mouse_up>(target, msg, events::mouse::button_type_right, false, thread_);
	case WM_NCLBUTTONDBLCLK:
		return mouse_<ui::window_surface, events::mouse_dbl_clk>(target, msg, events::mouse::button_type_left, true, thread_);
	case WM_LBUTTONDBLCLK:
		return mouse_<ui::window_surface, events::mouse_dbl_clk>(target, msg, events::mouse::button_type_left, false, thread_);
	case WM_NCMBUTTONDBLCLK:
		return mouse_<ui::window_surface, events::mouse_dbl_clk>(target, msg, events::mouse::button_type_middle, true, thread_);
	case WM_MBUTTONDBLCLK:
		return mouse_<ui::window_surface, events::mouse_dbl_clk>(target, msg, events::mouse::button_type_middle, false, thread_);
	case WM_NCRBUTTONDBLCLK:
		return mouse_<ui::window_surface, events::mouse_dbl_clk>(target, msg, events::mouse::button_type_right, true, thread_);
	case WM_RBUTTONDBLCLK:
		return mouse_<ui::window_surface, events::mouse_dbl_clk>(target, msg, events::mouse::button_type_right, false, thread_);
	case WM_MOUSEWHEEL:
	case WM_MOUSEHWHEEL:
		return mouse_<ui::window_surface, events::mouse_wheel>(target, msg, events::mouse::button_type_right, false, thread_);
	case WM_KEYDOWN:
		return key_<ui::window_surface, events::key_down>(target, msg, thread_);
	case WM_KEYUP:
		return key_<ui::window_surface, events::key_up>(target, msg, thread_);
	case WM_CHAR:
		return key_<ui::window_surface, events::key_press>(target, msg, thread_);
	case WM_SETFOCUS:
		focused_window_ = window_target;
		return trigger_event_<events::set_focus>(target, msg, ((window_target == nullptr) ? nullptr : thread_.get_class_entry_(window_target->get_class_name()))).second;
	case WM_KILLFOCUS:
		if (focused_window_ == window_target)
			focused_window_ = nullptr;
		return trigger_event_<events::kill_focus>(target, msg, ((window_target == nullptr) ? nullptr : thread_.get_class_entry_(window_target->get_class_name()))).second;
	case WM_ACTIVATE:
		return trigger_event_<events::activate>(target, msg, ((window_target == nullptr) ? nullptr : thread_.get_class_entry_(window_target->get_class_name()))).second;
	case WM_MOUSEACTIVATE:
		return trigger_event_<events::mouse_activate>(target, msg, ((window_target == nullptr) ? nullptr : thread_.get_class_entry_(window_target->get_class_name()))).second;
	case WM_ENABLE:
		return trigger_event_<events::enable>(target, msg, ((window_target == nullptr) ? nullptr : thread_.get_class_entry_(window_target->get_class_name()))).second;
	case WM_MENUSELECT:
		return menu_select_(target, msg);
	case WINP_WM_MENU_ITEM_SELECT:
		return trigger_event_<events::menu_item_select>(target, msg, ((window_target == nullptr) ? nullptr : thread_.get_class_entry_(window_target->get_class_name()))).second;
	case WINP_WM_ITEM_CHECK:
		return trigger_event_<events::item_check>(target, msg, ((window_target == nullptr) ? nullptr : thread_.get_class_entry_(window_target->get_class_name()))).second;
	case WINP_WM_MENU_ITEM_HIGHLIGHT:
		return trigger_event_<events::menu_item_highlight>(target, msg, ((window_target == nullptr) ? nullptr : thread_.get_class_entry_(window_target->get_class_name()))).second;
	case WM_MENUCOMMAND:
		return menu_command_(target, msg);
	case WM_SYSCOMMAND:
		return system_command_(target, msg);
	case WM_CONTEXTMENU:
	case WINP_WM_SPLIT_BUTTON_DROPDOWN:
		return context_menu_(target, msg);
	case WINP_WM_GET_CONTEXT_MENU_POSITION:
		return trigger_event_<events::get_context_menu_position>(target, msg, ((window_target == nullptr) ? nullptr : thread_.get_class_entry_(window_target->get_class_name()))).second;
	case WINP_WM_GET_CONTEXT_MENU_HANDLE:
		return trigger_event_<events::get_context_menu_handle>(target, msg, ((window_target == nullptr) ? nullptr : thread_.get_class_entry_(window_target->get_class_name()))).second;
	case WINP_WM_BLOCK_CONTEXT_MENU:
		return trigger_event_<events::block_context_menu>(target, msg, ((window_target == nullptr) ? nullptr : thread_.get_class_entry_(window_target->get_class_name()))).second;
	case WM_INITMENUPOPUP:
		return menu_init_(target, msg);
	case WM_COMMAND:
		return command_(target, msg);
	case WM_NOTIFY:
		return notify_(target, msg);
	case WM_SHOWWINDOW:
		if (msg.wParam == FALSE)
			return trigger_event_<events::hide>(target, msg, ((window_target == nullptr) ? nullptr : thread_.get_class_entry_(window_target->get_class_name()))).second;
		return trigger_event_<events::show>(target, msg, ((window_target == nullptr) ? nullptr : thread_.get_class_entry_(window_target->get_class_name()))).second;
	default:
		break;
	}

	return trigger_event_<events::unhandled>(target, msg, ((window_target == nullptr) ? nullptr : thread_.get_class_entry_(window_target->get_class_name()))).second;
}

LRESULT winp::thread::item_manager::dispatch_message_(item &target, HWND handle, UINT message, WPARAM wparam, LPARAM lparam){
	MSG msg{
		handle,
		message,
		wparam,
		lparam
	};

	return dispatch_message_(target, msg);
}

LRESULT winp::thread::item_manager::window_destroyed_(item &target, MSG &msg){
	if (msg.hwnd != nullptr){//Remove refs
		if (!top_level_windows_.empty())
			top_level_windows_.erase(msg.hwnd);

		if (!windows_.empty())
			windows_.erase(msg.hwnd);

		if (msg.hwnd == window_cache_.handle)
			window_cache_ = window_cache_info{};
	}

	auto window_target = dynamic_cast<ui::window_surface *>(&target);
	if (window_target != nullptr)
		window_target->handle_ = nullptr;

	if (auto tree_target = dynamic_cast<ui::tree *>(&target); tree_target != nullptr){
		tree_target->traverse_all_children([&](ui::object &child){
			if (dynamic_cast<ui::window_surface *>(&child) == nullptr)
				child.destroy();
		}, true);
	}

	return trigger_event_<events::destroy>(target, msg, ((window_target == nullptr) ? nullptr : thread_.get_class_entry_(window_target->get_class_name()))).second;
}

LRESULT winp::thread::item_manager::set_cursor_(item &target, MSG &msg){
	auto window_target = dynamic_cast<ui::window_surface *>(&target);
	auto result = trigger_event_<events::cursor>(target, msg, ((window_target == nullptr) ? nullptr : thread_.get_class_entry_(window_target->get_class_name())));

	if ((result.first & events::object::state_default_prevented) != 0u)
		return TRUE;//Default prevented

	auto value = (((result.first & events::object::state_result_set) == 0u) ? get_default_cursor_(msg) : reinterpret_cast<HCURSOR>(result.second));
	SetCursor((value == nullptr) ? LoadCursorW(nullptr, IDC_ARROW) : value);

	return TRUE;
}

LRESULT winp::thread::item_manager::erase_background_(item &context, item &target, MSG &msg){
	auto visible_context = dynamic_cast<ui::visible_surface *>(&context);
	if (visible_context == nullptr)//Visible surface required
		return 0;

	auto object_context = dynamic_cast<ui::object *>(&context);
	if (object_context == nullptr)//Object required
		return 0;

	LRESULT result = 0;
	auto window_context = dynamic_cast<ui::window_surface *>(&context);

	if (object_context->is_created() && (window_context != nullptr || visible_context->is_visible()))
		result = trigger_event_with_target_<events::erase_background>(context, target, msg, ((window_context == nullptr) ? nullptr : thread_.get_class_entry_(window_context->get_class_name()))).second;

	return result;
}

LRESULT winp::thread::item_manager::paint_(item &context, item &target, MSG &msg, bool check_interception){
	auto visible_context = dynamic_cast<ui::visible_surface *>(&context);
	if (visible_context == nullptr)//Visible surface required
		return 0;

	auto object_context = dynamic_cast<ui::object *>(&context);
	if (object_context == nullptr)//Object required
		return 0;

	auto window_context = dynamic_cast<ui::window_surface *>(&context);
	if (window_context == nullptr){
		if (paint_device_ == nullptr || !visible_context->is_visible())
			return 0;//Do nothing

		if (check_interception){
			POINT offset{};
			auto window_ancestor = object_context->get_first_ancestor_of<ui::window_surface>([&](ui::tree &ancestor){
				if (auto surface_ancestor = dynamic_cast<ui::surface *>(&ancestor); surface_ancestor != nullptr){
					auto ancestor_position = surface_ancestor->get_position();
					auto ancestor_client_offset = surface_ancestor->get_client_offset();

					offset.x += (ancestor_position.x + ancestor_client_offset.x);
					offset.y += (ancestor_position.y + ancestor_client_offset.y);
				}

				return true;
			});

			if (window_ancestor == nullptr)
				return 0;//Window ancestor required

			auto update_rect = update_rect_;
			auto context_dimension = visible_context->get_dimension();
			auto ancestor_client_start_offset = window_ancestor->get_client_start_offset();

			offset.x += ancestor_client_start_offset.x;
			offset.y += ancestor_client_start_offset.y;

			OffsetRect(&context_dimension, offset.x, offset.y);//Move relative to offset
			IntersectRect(&update_rect, &update_rect, &context_dimension);
			if (IsRectEmpty(&update_rect) != FALSE)
				return 0;//Outside update region
		}
		
		MSG paint_msg{ msg.hwnd, WM_ERASEBKGND, ((msg.message == WM_PRINTCLIENT) ? msg.wParam : reinterpret_cast<WPARAM>(paint_device_)) };
		erase_background_(context, target, paint_msg);
	}
	else if (msg.hwnd != nullptr){
		if (msg.message != WM_PRINTCLIENT){
			GetUpdateRect(msg.hwnd, &update_rect_, FALSE);
			if ((paint_device_ = GetDC(msg.hwnd)) != nullptr)
				IntersectClipRect(paint_device_, update_rect_.left, update_rect_.top, update_rect_.right, update_rect_.bottom);
		}
		else
			GetClipBox(reinterpret_cast<HDC>(msg.wParam), &update_rect_);
	}

	LRESULT result = 0;
	if (object_context->is_created())
		result = trigger_event_with_target_<events::paint>(context, target, msg, ((window_context == nullptr) ? nullptr : thread_.get_class_entry_(window_context->get_class_name()))).second;

	if (auto tree_context = dynamic_cast<ui::tree *>(&context); tree_context != nullptr){
		tree_context->traverse_all_children([&](ui::object &child){
			if (dynamic_cast<ui::window_surface *>(&child) == nullptr)
				paint_(child, target, msg, (window_context != nullptr));
		}, true);
	}

	if (window_context != nullptr && paint_device_ != nullptr){
		ReleaseDC(msg.hwnd, paint_device_);
		paint_device_ = nullptr;
	}

	return result;
}

LRESULT winp::thread::item_manager::position_change_(item &target, MSG &msg, bool changing){
	LRESULT result = 0;
	auto window_target = dynamic_cast<ui::window_surface *>(&target);

	if (changing){
		auto result_info = trigger_event_<events::position_change>(target, true, msg, ((window_target == nullptr) ? nullptr : thread_.get_class_entry_(window_target->get_class_name())));
		if ((result_info.first & events::object::state_default_prevented) != 0u)
			reinterpret_cast<WINDOWPOS *>(msg.lParam)->flags |= (SWP_NOMOVE | SWP_NOSIZE);
		result = result_info.second;
	}
	else
		result = trigger_event_<events::position_change>(target, false, msg, ((window_target == nullptr) ? nullptr : thread_.get_class_entry_(window_target->get_class_name()))).second;

	return result;
}

LRESULT winp::thread::item_manager::style_changing_(item &target, MSG &msg){
	auto window_target = dynamic_cast<ui::window_surface *>(&target);
	if (window_target != nullptr){
		reinterpret_cast<STYLESTRUCT *>(msg.lParam)->styleNew &= ~window_target->get_filtered_styles_((msg.wParam == GWL_EXSTYLE));
		reinterpret_cast<STYLESTRUCT *>(msg.lParam)->styleNew |= window_target->get_persistent_styles_((msg.wParam == GWL_EXSTYLE));
	}

	return trigger_event_<events::unhandled>(target, msg, ((window_target == nullptr) ? nullptr : thread_.get_class_entry_(window_target->get_class_name()))).second;
}

LRESULT winp::thread::item_manager::mouse_leave_(item &target, MSG &msg){
	auto window_target = dynamic_cast<ui::window_surface *>(&target);
	if (window_target == nullptr)
		return 0;

	auto position = GetMessagePos();
	if (window_target->absolute_hit_test(GET_X_LPARAM(position), GET_Y_LPARAM(position)) == HTNOWHERE){
		if (tracking_mouse_leave_ == &target)
			tracking_mouse_leave_ = nullptr;
		return mouse_<ui::window_surface, events::mouse_leave>(target, msg, events::mouse::button_type_nil, false, thread_);
	}

	return CallWindowProcW(thread_.get_class_entry_(window_target->get_class_name()), msg.hwnd, msg.message, msg.wParam, msg.lParam);
}

LRESULT winp::thread::item_manager::mouse_move_(item &target, MSG &msg){
	if (dynamic_cast<ui::window_surface *>(&target) == nullptr)
		return 0;

	if (tracking_mouse_leave_ != &target){
		track_mouse_leave_(msg.hwnd, ((msg.message == WM_MOUSEMOVE) ? 0 : TME_NONCLIENT));
		tracking_mouse_leave_ = &target;
	}

	return mouse_<ui::window_surface, events::mouse_move>(target, msg, events::mouse::button_type_nil, false, thread_);
}

LRESULT winp::thread::item_manager::menu_select_(item &target, MSG &msg){
	auto window_target = dynamic_cast<ui::window_surface *>(&target);
	if (window_target == nullptr)
		return trigger_event_<events::unhandled>(target, msg, nullptr).second;

	auto flags = HIWORD(msg.wParam);
	if ((flags & MF_HILITE) == 0u || menus_.empty())
		return trigger_event_<events::unhandled>(target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;

	menu::item *menu_item_target = nullptr;
	if ((flags & MF_SYSMENU) == 0u){
		auto menu = menus_.find(reinterpret_cast<HMENU>(msg.lParam));
		if (menu == menus_.end())
			return trigger_event_<events::unhandled>(target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;

		if ((flags & MF_POPUP) != 0u)//By index
			menu_item_target = menu->second->get_item_at(static_cast<UINT>(msg.wParam));
		else//By ID
			menu_item_target = find_menu_item_(*menu->second, static_cast<UINT>(msg.wParam));
	}
	else if ((static_cast<UINT>(msg.wParam) & 0xFFF0u) == 0u)
		menu_item_target = nullptr;
	else if ((menu_item_target = find_menu_item_(GetSystemMenu(window_target->handle_, FALSE), (static_cast<UINT>(msg.wParam) & 0xFFF0u))) == nullptr)//System menu item
		menu_item_target = find_sub_menu_item_(GetSystemMenu(window_target->handle_, FALSE), (static_cast<UINT>(msg.wParam) & 0xFFF0u));

	if (menu_item_target == nullptr)//find_sub_menu_item_
		return trigger_event_<events::unhandled>(target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;

	return trigger_event_<events::menu_item_highlight>(*menu_item_target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;
}

LRESULT winp::thread::item_manager::menu_command_(item &target, MSG &msg){
	auto window_target = dynamic_cast<ui::window_surface *>(&target);
	if (window_target == nullptr)
		return trigger_event_<events::unhandled>(target, msg, nullptr).second;

	auto menu = menus_.find(reinterpret_cast<HMENU>(msg.lParam));
	if (menu == menus_.end())
		return trigger_event_<events::unhandled>(target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;

	auto menu_item_target = menu->second->get_item_at(static_cast<UINT>(msg.wParam));
	if (menu_item_target == nullptr)
		return trigger_event_<events::unhandled>(target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;

	auto result = trigger_event_<events::menu_item_select>(*menu_item_target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;
	if (menu->second == active_context_menu_.get())
		active_context_menu_ = nullptr;

	return result;
}

LRESULT winp::thread::item_manager::system_command_(item &target, MSG &msg){
	auto window_target = dynamic_cast<ui::window_surface *>(&target);
	if (window_target == nullptr)
		return trigger_event_<events::unhandled>(target, msg, nullptr).second;

	if (msg.lParam == -1 || msg.lParam == 0)//Accelerator or mnemonic
		return trigger_event_<events::unhandled>(target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;

	auto menu_item_target = find_menu_item_(GetSystemMenu(window_target->handle_, FALSE), (static_cast<UINT>(msg.wParam) & 0xFFF0u));
	if (menu_item_target == nullptr && (menu_item_target = find_sub_menu_item_(GetSystemMenu(window_target->handle_, FALSE), (static_cast<UINT>(msg.wParam) & 0xFFF0u))) == nullptr)
		return trigger_event_<events::unhandled>(target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;

	return trigger_event_<events::menu_item_select>(*menu_item_target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;
}

LRESULT winp::thread::item_manager::context_menu_(item &target, MSG &msg){
	auto window_target = dynamic_cast<ui::window_surface *>(&target);
	if (window_target == nullptr)
		return trigger_event_<events::unhandled>(target, msg, nullptr).second;

	active_context_menu_object_ = nullptr;
	active_context_menu_ = nullptr;

	auto actual_target = find_window_(reinterpret_cast<HWND>(msg.wParam), false);
	auto &event_target = ((actual_target == nullptr) ? target : *actual_target);

	auto result_info = trigger_event_with_target_<events::get_context_menu_handle>(target, event_target, msg, thread_.get_class_entry_(window_target->get_class_name()));
	if ((result_info.first & events::object::state_default_prevented) != 0u)//Default prevented
		return trigger_event_<events::unhandled>(target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;

	POINT position{ GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam) };
	if (auto handle = reinterpret_cast<HMENU>(result_info.second); handle != nullptr){//Use menu
		if (GetMenuItemCount(handle) == 0)
			return 0;//Empty menu

		auto menu = menus_.find(handle);
		if (menu != menus_.end())
			active_context_menu_object_ = menu->second;

		if (position.x == -1 && position.y == -1){//Retrieve position
			auto value = thread_.send_message(target, WINP_WM_GET_CONTEXT_MENU_POSITION, 0, msg.lParam);
			position = POINT{ GET_X_LPARAM(value), GET_Y_LPARAM(value) };
		}

		TrackPopupMenu(handle, (GetSystemMetrics(SM_MENUDROPALIGNMENT) | TPM_RIGHTBUTTON), position.x, position.y, 0, window_target->handle_, nullptr);
		return 0;
	}
	
	if ((position.x != -1 || position.y != -1) && window_target->absolute_hit_test(position) != HTCLIENT)
		return trigger_event_<events::unhandled>(target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;

	if (target.events().get_bound_count<events::context_menu>() == 0u || thread_.send_message(target, WINP_WM_BLOCK_CONTEXT_MENU, 0, msg.lParam) != FALSE)//Context menu blocked
		return trigger_event_<events::unhandled>(target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;

	if ((active_context_menu_ = std::make_shared<ui::object_collection<menu::popup>>(thread_)) == nullptr || active_context_menu_->create() != utility::error_code::nil)
		return 0;

	MSG context_msg{ msg.hwnd, msg.message, reinterpret_cast<WPARAM>(active_context_menu_.get()), msg.lParam };
	if ((trigger_event_with_target_<events::context_menu>(target, event_target, context_msg, nullptr).first & events::object::state_default_prevented) != 0u || GetMenuItemCount(active_context_menu_->get_handle()) == 0){
		active_context_menu_ = nullptr;
		return 0;//Default prevented or empty menu
	}

	if (position.x == -1 && position.y == -1){//Retrieve position
		auto value = thread_.send_message(target, WINP_WM_GET_CONTEXT_MENU_POSITION, 0, msg.lParam);
		position = POINT{ GET_X_LPARAM(value), GET_Y_LPARAM(value) };
	}

	TrackPopupMenu(active_context_menu_->get_handle(), (GetSystemMetrics(SM_MENUDROPALIGNMENT) | TPM_RIGHTBUTTON), position.x, position.y, 0, window_target->handle_, nullptr);
	return 0;
}

LRESULT winp::thread::item_manager::menu_init_(item &target, MSG &msg){
	auto window_target = dynamic_cast<ui::window_surface *>(&target);
	if (window_target == nullptr)
		return trigger_event_<events::unhandled>(target, msg, nullptr).second;

	if (active_context_menu_object_ == nullptr)
		return trigger_event_<events::unhandled>(target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;

	auto menu = menus_.find(reinterpret_cast<HMENU>(msg.wParam));
	if (menu == menus_.end() || menu->second != active_context_menu_object_)
		return trigger_event_<events::unhandled>(target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;

	MSG init_msg{ msg.hwnd, WINP_WM_INIT_MENU_ITEM };
	active_context_menu_object_->traverse_all_items([&](menu::item &item){
		if (dynamic_cast<menu::separator *>(&item) == nullptr){
			if (get_result_(trigger_event_with_target_and_value_<events::menu_init_item>(*active_context_menu_object_, item, TRUE, init_msg, nullptr), FALSE) == FALSE)
				item.set_enabled_state(false);
			else
				item.set_enabled_state(true);
		}
	}, true);

	active_context_menu_object_ = nullptr;
	return trigger_event_<events::unhandled>(target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;
}

LRESULT winp::thread::item_manager::command_(item &target, MSG &msg){
	auto window_target = dynamic_cast<ui::window_surface *>(&target);
	if (window_target == nullptr)
		return trigger_event_<events::unhandled>(target, msg, nullptr).second;

	if (msg.lParam != 0){//Control command
		auto control_target = dynamic_cast<control::object *>(find_window_(reinterpret_cast<HWND>(msg.lParam), false));
		return ((control_target == nullptr) ? trigger_event_<events::unhandled>(target, msg, thread_.get_class_entry_(window_target->get_class_name())).second : control_target->dispatch_command_(msg));
	}

	if (HIWORD(msg.wParam) != 0u){//Accelerator command

	}

	return trigger_event_<events::unhandled>(target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;
}

LRESULT winp::thread::item_manager::notify_(item &target, MSG &msg){
	auto window_target = dynamic_cast<ui::window_surface *>(&target);
	if (window_target == nullptr)
		return trigger_event_<events::unhandled>(target, msg, nullptr).second;

	auto control_target = dynamic_cast<control::object *>(find_window_(reinterpret_cast<NMHDR *>(msg.lParam)->hwndFrom, false));
	return ((control_target == nullptr) ? trigger_event_<events::unhandled>(target, msg, thread_.get_class_entry_(window_target->get_class_name())).second : control_target->dispatch_notification_(msg));
}

bool winp::thread::item_manager::menu_item_id_is_reserved_(UINT id){
	return (HIWORD(id) == 1u);
}

HCURSOR winp::thread::item_manager::get_default_cursor_(const MSG &msg){
	switch (LOWORD(msg.lParam)){
	case HTERROR://Play beep if applicable
		switch (HIWORD(msg.lParam)){
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_XBUTTONDOWN:
			MessageBeep(0);
			break;
		default:
			break;
		}

		return nullptr;
	case HTCLIENT://Use class cursor
		break;
	case HTLEFT:
	case HTRIGHT:
		return LoadCursorW(nullptr, IDC_SIZEWE);
	case HTTOP:
	case HTBOTTOM:
		return LoadCursorW(nullptr, IDC_SIZENS);
	case HTTOPLEFT:
	case HTBOTTOMRIGHT:
		return LoadCursorW(nullptr, IDC_SIZENWSE);
	case HTTOPRIGHT:
	case HTBOTTOMLEFT:
		return LoadCursorW(nullptr, IDC_SIZENESW);
	default:
		return LoadCursorW(nullptr, IDC_ARROW);
	}

	auto value = ((msg.hwnd == nullptr) ? nullptr : reinterpret_cast<HCURSOR>(GetClassLongPtrW(msg.hwnd, GCLP_HCURSOR)));
	return ((value == nullptr) ? nullptr : LoadCursorW(nullptr, IDC_ARROW));
}

void winp::thread::item_manager::track_mouse_leave_(HWND target, UINT flags){
	if (target != nullptr){
		TRACKMOUSEEVENT info{ sizeof(TRACKMOUSEEVENT), (TME_LEAVE | flags), target, 0 };
		TrackMouseEvent(&info);
	}
}

void winp::thread::item_manager::trigger_event_(events::object &e){
	e.get_context().trigger_event_(e);
}

bool winp::thread::item_manager::bubble_event_(events::object &e){
	return item::bubble_event_(e);
}

LRESULT winp::thread::item_manager::get_result_(const std::pair<unsigned int, LRESULT> &info, LRESULT prevented_result){
	return (((info.first & events::object::state_default_prevented) == 0u) ? info.second : prevented_result);
}

LRESULT CALLBACK winp::thread::item_manager::entry_(HWND handle, UINT message, WPARAM wparam, LPARAM lparam){
	auto current_thread = app::collection::get_current_thread();
	if (current_thread == nullptr)
		return CallWindowProcW(DefWindowProcW, handle, message, wparam, lparam);

	auto &manager = current_thread->get_item_manager();
	if (handle == manager.thread_.get_message_handle())
		return manager.handle_thread_message_(handle, message, wparam, lparam);

	if (auto target = manager.find_window_(handle, true); target != nullptr)
		return manager.dispatch_message_(*target, handle, message, wparam, lparam);

	return CallWindowProcW(DefWindowProcW, handle, message, wparam, lparam);
}

LRESULT CALLBACK winp::thread::item_manager::hook_entry_(int code, WPARAM wparam, LPARAM lparam){
	auto &manager = app::collection::get_current_thread()->get_item_manager();
	switch (code){
	case HCBT_DESTROYWND:
		if (reinterpret_cast<HWND>(wparam) == manager.window_cache_.handle || (!manager.windows_.empty() && manager.windows_.find(reinterpret_cast<HWND>(wparam)) != manager.windows_.end()))
			SetMenu(reinterpret_cast<HWND>(wparam), nullptr);
		return CallNextHookEx(nullptr, code, wparam, lparam);
	case HCBT_CREATEWND:
		break;
	default:
		return CallNextHookEx(nullptr, code, wparam, lparam);
	}

	if (manager.window_cache_.handle != nullptr || manager.window_cache_.object == nullptr || manager.window_cache_.object != static_cast<ui::window_surface *>(reinterpret_cast<CBT_CREATEWNDW *>(lparam)->lpcs->lpCreateParams))
		return CallNextHookEx(nullptr, code, wparam, lparam);//Does not match object creating window

	manager.window_cache_.handle = reinterpret_cast<HWND>(wparam);
	manager.window_cache_.object->handle_ = manager.window_cache_.handle;

	manager.windows_[manager.window_cache_.handle] = manager.window_cache_.object;
	if (manager.window_cache_.object->is_top_level_())
		manager.top_level_windows_[manager.window_cache_.handle] = manager.window_cache_.object;

	if (auto class_entry = manager.thread_.get_class_entry_(manager.window_cache_.object->get_class_name()); class_entry != nullptr && class_entry != DefWindowProcW)
		SetWindowLongPtrW(manager.window_cache_.handle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(entry_));//Subclass window

	return CallNextHookEx(nullptr, code, wparam, lparam);
}
