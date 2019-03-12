#include "../app/app_collection.h"
#include "../ui/ui_window_surface.h"
#include "../ui/ui_non_window_surface.h"

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

	auto it = menu_items_.find(menu_object_parent->get_handle());
	if (it == menu_items_.end())
		return 0u;

	if (id == 0u || it->second.find(id) != it->second.end()){//Generate new
		for (; max_tries > 0u; --max_tries){
			id = thread_.generate_random_integer(static_cast<UINT>(1), std::numeric_limits<UINT>::max());
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

	auto it = menu_items_.find(menu_object_parent->get_handle());
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

winp::menu::item *winp::thread::item_manager::find_menu_item_(menu::object &menu, UINT id) const{
	if (menu_items_.empty())
		return nullptr;

	auto it = menu_items_.find(menu.get_handle());
	if (it == menu_items_.end())
		return nullptr;

	auto item_it = it->second.find(id);
	return ((item_it == it->second.end()) ? nullptr : item_it->second);
}

bool winp::thread::item_manager::is_dialog_message_(MSG &msg) const{
	return false;
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
		return get_result_(trigger_event_with_value_<events::create>(target, TRUE, msg, ((window_target == nullptr) ? nullptr : thread_.get_app().get_class_entry(window_target->get_class_name()))), FALSE);
	case WM_NCDESTROY:
		return window_destroyed_(target, msg);
	case WM_CLOSE:
		return trigger_event_<events::close>(target, msg, ((window_target == nullptr) ? nullptr : thread_.get_app().get_class_entry(window_target->get_class_name()))).second;
	case WM_SETCURSOR:
		return set_cursor_(target, msg);
	case WINP_WM_GET_BACKGROUND_COLOR:
		return trigger_event_<events::background_color>(target, msg, nullptr).second;
	case WM_ERASEBKGND:
		return erase_background_(target, target, msg);
	case WM_PAINT:
		return paint_(target, target, msg);
	case WM_WINDOWPOSCHANGING:
		return position_change_(target, msg, true);
	case WM_WINDOWPOSCHANGED:
		return position_change_(target, msg, false);
	case WM_NCMOUSELEAVE:
	case WM_MOUSELEAVE:
		return mouse_leave_(target, msg);
	case WM_NCMOUSEMOVE:
	case WM_MOUSEMOVE:
		return mouse_move_(target, msg);
	case WM_NCLBUTTONDOWN:
		return mouse_<ui::window_surface, events::mouse_down>(target, msg, events::mouse::button_type_left, true, thread_.get_app());
	case WM_LBUTTONDOWN:
		return mouse_<ui::window_surface, events::mouse_down>(target, msg, events::mouse::button_type_left, false, thread_.get_app());
	case WM_NCMBUTTONDOWN:
		return mouse_<ui::window_surface, events::mouse_down>(target, msg, events::mouse::button_type_middle, true, thread_.get_app());
	case WM_MBUTTONDOWN:
		return mouse_<ui::window_surface, events::mouse_down>(target, msg, events::mouse::button_type_middle, false, thread_.get_app());
	case WM_NCRBUTTONDOWN:
		return mouse_<ui::window_surface, events::mouse_down>(target, msg, events::mouse::button_type_right, true, thread_.get_app());
	case WM_RBUTTONDOWN:
		return mouse_<ui::window_surface, events::mouse_down>(target, msg, events::mouse::button_type_right, false, thread_.get_app());
	case WM_NCLBUTTONUP:
		return mouse_<ui::window_surface, events::mouse_up>(target, msg, events::mouse::button_type_left, true, thread_.get_app());
	case WM_LBUTTONUP:
		return mouse_<ui::window_surface, events::mouse_up>(target, msg, events::mouse::button_type_left, false, thread_.get_app());
	case WM_NCMBUTTONUP:
		return mouse_<ui::window_surface, events::mouse_up>(target, msg, events::mouse::button_type_middle, true, thread_.get_app());
	case WM_MBUTTONUP:
		return mouse_<ui::window_surface, events::mouse_up>(target, msg, events::mouse::button_type_middle, false, thread_.get_app());
	case WM_NCRBUTTONUP:
		return mouse_<ui::window_surface, events::mouse_up>(target, msg, events::mouse::button_type_right, true, thread_.get_app());
	case WM_RBUTTONUP:
		return mouse_<ui::window_surface, events::mouse_up>(target, msg, events::mouse::button_type_right, false, thread_.get_app());
	case WM_NCLBUTTONDBLCLK:
		return mouse_<ui::window_surface, events::mouse_dbl_clk>(target, msg, events::mouse::button_type_left, true, thread_.get_app());
	case WM_LBUTTONDBLCLK:
		return mouse_<ui::window_surface, events::mouse_dbl_clk>(target, msg, events::mouse::button_type_left, false, thread_.get_app());
	case WM_NCMBUTTONDBLCLK:
		return mouse_<ui::window_surface, events::mouse_dbl_clk>(target, msg, events::mouse::button_type_middle, true, thread_.get_app());
	case WM_MBUTTONDBLCLK:
		return mouse_<ui::window_surface, events::mouse_dbl_clk>(target, msg, events::mouse::button_type_middle, false, thread_.get_app());
	case WM_NCRBUTTONDBLCLK:
		return mouse_<ui::window_surface, events::mouse_dbl_clk>(target, msg, events::mouse::button_type_right, true, thread_.get_app());
	case WM_RBUTTONDBLCLK:
		return mouse_<ui::window_surface, events::mouse_dbl_clk>(target, msg, events::mouse::button_type_right, false, thread_.get_app());
	case WM_MOUSEWHEEL:
	case WM_MOUSEHWHEEL:
		return mouse_<ui::window_surface, events::mouse_wheel>(target, msg, events::mouse::button_type_right, false, thread_.get_app());
	case WM_KEYDOWN:
		return key_<ui::window_surface, events::key_down>(target, msg, thread_.get_app());
	case WM_KEYUP:
		return key_<ui::window_surface, events::key_up>(target, msg, thread_.get_app());
	case WM_CHAR:
		return key_<ui::window_surface, events::key_press>(target, msg, thread_.get_app());
	case WM_SETFOCUS:
		return trigger_event_<events::set_focus>(target, msg, ((window_target == nullptr) ? nullptr : thread_.get_app().get_class_entry(window_target->get_class_name()))).second;
	case WM_KILLFOCUS:
		return trigger_event_<events::kill_focus>(target, msg, ((window_target == nullptr) ? nullptr : thread_.get_app().get_class_entry(window_target->get_class_name()))).second;
	case WM_ACTIVATE:
		return trigger_event_<events::activate>(target, msg, ((window_target == nullptr) ? nullptr : thread_.get_app().get_class_entry(window_target->get_class_name()))).second;
	case WM_MOUSEACTIVATE:
		return trigger_event_<events::mouse_activate>(target, msg, ((window_target == nullptr) ? nullptr : thread_.get_app().get_class_entry(window_target->get_class_name()))).second;
	case WM_ENABLE:
		return trigger_event_<events::enable>(target, msg, ((window_target == nullptr) ? nullptr : thread_.get_app().get_class_entry(window_target->get_class_name()))).second;
	case WM_MENUSELECT:
		return menu_select_(target, msg);
	case WINP_WM_MENU_ITEM_SELECT:
		return trigger_event_<events::menu_item_select>(target, msg, ((window_target == nullptr) ? nullptr : thread_.get_app().get_class_entry(window_target->get_class_name()))).second;
	case WINP_WM_MENU_ITEM_CHECK:
		return trigger_event_<events::menu_item_check>(target, msg, ((window_target == nullptr) ? nullptr : thread_.get_app().get_class_entry(window_target->get_class_name()))).second;
	case WINP_WM_MENU_ITEM_HIGHLIGHT:
		return trigger_event_<events::menu_item_highlight>(target, msg, ((window_target == nullptr) ? nullptr : thread_.get_app().get_class_entry(window_target->get_class_name()))).second;
	default:
		break;
	}

	return trigger_event_<events::unhandled>(target, msg, ((window_target == nullptr) ? nullptr : thread_.get_app().get_class_entry(window_target->get_class_name()))).second;
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

	return trigger_event_<events::destroy>(target, msg, ((window_target == nullptr) ? nullptr : thread_.get_app().get_class_entry(window_target->get_class_name()))).second;
}

LRESULT winp::thread::item_manager::set_cursor_(item &target, MSG &msg){
	auto window_target = dynamic_cast<ui::window_surface *>(&target);
	auto result = trigger_event_<events::cursor>(target, msg, ((window_target == nullptr) ? nullptr : thread_.get_app().get_class_entry(window_target->get_class_name())));

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
	auto window_context = dynamic_cast<ui::window_surface *>(&context);

	if (window_context == nullptr && (!visible_context->is_visible() || (object_context != nullptr && !object_context->is_created())))
		return 0;//Surface is not visible

	auto result = trigger_event_with_target_<events::erase_background>(context, target, msg, ((window_context == nullptr) ? nullptr : thread_.get_app().get_class_entry(window_context->get_class_name()))).second;
	if (auto tree_context = dynamic_cast<ui::tree *>(&context); tree_context != nullptr){
		tree_context->traverse_all_children([&](ui::object &child){
			if (dynamic_cast<ui::window_surface *>(&child) == nullptr)
				erase_background_(child, target, msg);
		}, true);
	}

	return result;
}

LRESULT winp::thread::item_manager::paint_(item &context, item &target, MSG &msg){
	auto visible_context = dynamic_cast<ui::visible_surface *>(&context);
	if (visible_context == nullptr)//Visible surface required
		return 0;

	auto object_context = dynamic_cast<ui::object *>(&context);
	auto window_context = dynamic_cast<ui::window_surface *>(&context);

	if (window_context != nullptr && msg.hwnd != nullptr)
		GetUpdateRect(msg.hwnd, &update_rect_, FALSE);
	else if (window_context == nullptr && (!visible_context->is_visible() || (object_context != nullptr && !object_context->is_created())))
		return 0;//Surface is not visible

	auto result = trigger_event_with_target_<events::paint>(context, target, msg, ((window_context == nullptr) ? nullptr : thread_.get_app().get_class_entry(window_context->get_class_name()))).second;
	if (auto tree_context = dynamic_cast<ui::tree *>(&context); tree_context != nullptr){
		tree_context->traverse_all_children([&](ui::object &child){
			if (dynamic_cast<ui::window_surface *>(&child) == nullptr)
				paint_(child, target, msg);
		}, true);
	}

	return result;
}

LRESULT winp::thread::item_manager::position_change_(item &target, MSG &msg, bool changing){
	LRESULT result = 0;
	auto window_target = dynamic_cast<ui::window_surface *>(&target);
	if (changing){
		auto result_info = trigger_event_<events::position_change>(target, true, msg, ((window_target == nullptr) ? nullptr : thread_.get_app().get_class_entry(window_target->get_class_name())));
		if ((result_info.first & events::object::state_default_prevented) != 0u)
			reinterpret_cast<WINDOWPOS *>(msg.lParam)->flags |= (SWP_NOMOVE | SWP_NOSIZE);
		result = result_info.second;
	}
	else if (auto non_window_target = dynamic_cast<ui::non_window_surface *>(&target); non_window_target != nullptr && (reinterpret_cast<WINDOWPOS *>(msg.lParam)->flags & (SWP_NOMOVE | SWP_NOSIZE)) != (SWP_NOMOVE | SWP_NOSIZE)){
		non_window_target->resize_handle_();
		result = trigger_event_<events::position_change>(target, false, msg, ((window_target == nullptr) ? nullptr : thread_.get_app().get_class_entry(window_target->get_class_name()))).second;
	}

	return result;
}

LRESULT winp::thread::item_manager::mouse_leave_(item &target, MSG &msg){
	auto window_target = dynamic_cast<ui::window_surface *>(&target);
	if (window_target == nullptr)
		return 0;

	auto position = GetMessagePos();
	if (window_target->absolute_hit_test(GET_X_LPARAM(position), GET_Y_LPARAM(position)) == HTNOWHERE){
		if (tracking_mouse_leave_ == &target)
			tracking_mouse_leave_ = nullptr;
		return mouse_<ui::window_surface, events::mouse_leave>(target, msg, events::mouse::button_type_nil, false, thread_.get_app());
	}

	return CallWindowProcW(thread_.get_app().get_class_entry(window_target->get_class_name()), msg.hwnd, msg.message, msg.wParam, msg.lParam);
}

LRESULT winp::thread::item_manager::mouse_move_(item &target, MSG &msg){
	if (dynamic_cast<ui::window_surface *>(&target) == nullptr)
		return 0;

	if (tracking_mouse_leave_ != &target){
		track_mouse_leave_(msg.hwnd, ((msg.message == WM_MOUSEMOVE) ? 0 : TME_NONCLIENT));
		tracking_mouse_leave_ = &target;
	}

	return mouse_<ui::window_surface, events::mouse_move>(target, msg, events::mouse::button_type_nil, false, thread_.get_app());
}

LRESULT winp::thread::item_manager::menu_select_(item &target, MSG &msg){
	auto window_target = dynamic_cast<ui::window_surface *>(&target);

	auto flags = HIWORD(msg.wParam);
	if ((flags & MF_HILITE) == 0u || menus_.empty())
		return trigger_event_<events::unhandled>(target, msg, ((window_target == nullptr) ? nullptr : thread_.get_app().get_class_entry(window_target->get_class_name()))).second;

	auto menu = menus_.find(reinterpret_cast<HMENU>(msg.lParam));
	if (menu == menus_.end())
		return trigger_event_<events::unhandled>(target, msg, ((window_target == nullptr) ? nullptr : thread_.get_app().get_class_entry(window_target->get_class_name()))).second;

	menu::item *menu_item_target = nullptr;
	if ((flags & MF_POPUP) != 0u)//By index
		menu_item_target = menu->second->get_item_at(static_cast<UINT>(msg.wParam));
	else if (static_cast<UINT>(msg.wParam) != 0u)//By ID
		menu_item_target = find_menu_item_(*menu->second, static_cast<UINT>(msg.wParam));

	if (menu_item_target == nullptr)
		return trigger_event_<events::unhandled>(target, msg, ((window_target == nullptr) ? nullptr : thread_.get_app().get_class_entry(window_target->get_class_name()))).second;

	return trigger_event_<events::menu_item_highlight>(*menu_item_target, msg, ((window_target == nullptr) ? nullptr : thread_.get_app().get_class_entry(window_target->get_class_name()))).second;
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
	auto &manager = app::collection::get_current_thread()->get_item_manager();
	if (handle == manager.thread_.get_message_handle())
		return manager.handle_thread_message_(handle, message, wparam, lparam);

	if (handle == manager.window_cache_.handle)
		return manager.dispatch_message_(*manager.window_cache_.object, handle, message, wparam, lparam);

	if (auto it = manager.windows_.find(handle); it != manager.windows_.end()){
		manager.window_cache_.handle = handle;
		manager.window_cache_.object = it->second;
		return manager.dispatch_message_(*it->second, handle, message, wparam, lparam);
	}

	return CallWindowProcW(DefWindowProcW, handle, message, wparam, lparam);
}

LRESULT CALLBACK winp::thread::item_manager::hook_entry_(int code, WPARAM wparam, LPARAM lparam){
	auto &manager = app::collection::get_current_thread()->get_item_manager();
	switch (code){
	case HCBT_CREATEWND:
		break;
	case HCBT_DESTROYWND:
		if (reinterpret_cast<HWND>(wparam) == manager.window_cache_.handle || (!manager.windows_.empty() && manager.windows_.find(reinterpret_cast<HWND>(wparam)) != manager.windows_.end()))
			SetMenu(reinterpret_cast<HWND>(wparam), nullptr);
		return CallNextHookEx(nullptr, code, wparam, lparam);
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

	if (auto class_entry = manager.thread_.get_app().get_class_entry(manager.window_cache_.object->get_class_name()); class_entry != nullptr && class_entry != DefWindowProcW)
		SetWindowLongPtrW(manager.window_cache_.handle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(class_entry));//Subclass window

	return CallNextHookEx(nullptr, code, wparam, lparam);
}
