#include "../app/app_collection.h"
#include "../ui/ui_window_surface.h"

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

HWND winp::thread::item_manager::create_window(ui::window_surface &owner, const CREATESTRUCTW &info){
	if (!thread_.is_thread_context())
		throw utility::error_code::outside_thread_context;

	window_cache_.handle = nullptr;
	window_cache_.object = &owner;

	return CreateWindowExW(
		info.dwExStyle,
		info.lpszClass,
		info.lpszName,
		info.style,
		info.x,
		info.y,
		info.cx,
		info.cy,
		info.hwndParent,
		info.hMenu,
		info.hInstance,
		&owner
	);
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
	switch (msg.message){
	case WM_NCCREATE:
		return get_result_(trigger_event_with_value_<events::create>(target, TRUE), FALSE);
	case WM_NCDESTROY:
		return window_destroyed_(target, msg);
	case WM_CREATE:
	case WM_DESTROY:
		return ((msg.hwnd == nullptr) ? 0 : CallWindowProcW(DefWindowProcW, msg.hwnd, msg.message, msg.wParam, msg.lParam));
	default:
		break;
	}

	if (auto window_target = dynamic_cast<ui::window_surface *>(&target); window_target != nullptr)
		return trigger_event_<events::unhandled>(target, msg, thread_.get_app().get_class_entry(window_target->get_class_name())).second;

	return trigger_event_<events::unhandled>(target, msg, nullptr).second;
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

	if (auto window_target = dynamic_cast<ui::window_surface *>(&target); window_target != nullptr)
		window_target->handle_ = nullptr;

	return trigger_event_<events::destroy>(target).first;
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
