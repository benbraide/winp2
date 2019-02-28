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
	case WM_CREATE:
	case WM_DESTROY:
		return ((window_target == nullptr) ? 0 : CallWindowProcW(thread_.get_app().get_class_entry(window_target->get_class_name()), msg.hwnd, msg.message, msg.wParam, msg.lParam));
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
	switch (code){
	case HCBT_CREATEWND:
		break;
	default:
		return CallNextHookEx(nullptr, code, wparam, lparam);
	}

	auto &manager = app::collection::get_current_thread()->get_item_manager();
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
