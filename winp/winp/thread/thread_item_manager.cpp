#include "../app/app_object.h"

#include "../ui/ui_io_hooks.h"
#include "../ui/ui_non_window_surface.h"
#include "../control/control_object.h"

#include "../menu/menu_separator.h"
#include "../menu/menu_link_item.h"

winp::thread::item_manager::item_manager(object &thread)
	: item_manager(thread, thread.get_local_id()){}

winp::thread::item_manager::item_manager(object &thread, DWORD thread_id)
	: thread_(thread){
	mouse_.full_feature_enabled = true;
	hook_handle_ = SetWindowsHookExW(WH_CBT, hook_entry_, nullptr, thread_id);

	mouse_.drag_threshold.cx = GetSystemMetrics(SM_CXDRAG);
	mouse_.drag_threshold.cy = GetSystemMetrics(SM_CYDRAG);

	update_rgn_.reset(CreateRectRgn(0, 0, 1, 1), true);
}

winp::thread::item_manager::~item_manager(){
	if (hook_handle_ != nullptr){
		UnhookWindowsHookEx(hook_handle_);
		hook_handle_ = nullptr;
	}
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

winp::utility::rgn winp::thread::item_manager::get_update_rgn() const{
	return update_rgn_;
}

RECT winp::thread::item_manager::get_update_rect() const{
	if (!is_thread_context())
		throw utility::error_code::outside_thread_context;
	return update_rgn_.get_dimension();
}

const winp::thread::item_manager::mouse_info &winp::thread::item_manager::get_mouse_state() const{
	if (!is_thread_context())
		throw utility::error_code::outside_thread_context;
	return mouse_;
}

winp::ui::object *winp::thread::item_manager::find_deepest_mouse_target(ui::object &target, const POINT &mouse_position){
	if (dynamic_cast<ui::window_surface *>(&target) != nullptr)
		return nullptr;

	auto visible_target = dynamic_cast<ui::visible_surface *>(&target);
	if (visible_target != nullptr && (!visible_target->is_visible() || visible_target->absolute_hit_test_(mouse_position.x, mouse_position.y) == HTNOWHERE))
		return nullptr;

	ui::object *deepest_target = nullptr;
	if (target.has_hook_<ui::io_hook>())
		deepest_target = &target;

	if (auto tree_target = dynamic_cast<ui::tree *>(&target); tree_target != nullptr){
		ui::object *deepest_child_target = nullptr;
		auto &children = tree_target->get_children();

		for (auto it = children.rbegin(); it != children.rend(); ++it){
			if ((deepest_child_target = find_deepest_mouse_target(**it, mouse_position)) != nullptr)
				break;
		}

		if (deepest_child_target != nullptr)
			deepest_target = deepest_child_target;
	}

	return deepest_target;
}

void winp::thread::item_manager::add_menu_(menu::object &owner){
	if (auto handle = owner.get_handle(); handle != nullptr && (menus_.empty() || menus_.find(handle) == menus_.end()))
		menus_[handle] = &owner;
}

void winp::thread::item_manager::remove_menu_(menu::object &owner){
	if (!menus_.empty()){
		for (auto it = menus_.begin(); it != menus_.end(); ++it){
			if (it->second == &owner){
				menus_.erase(it);
				break;
			}
		}
	}
}

void winp::thread::item_manager::add_timer_(const std::chrono::milliseconds &duration, const std::function<void()> &callback, unsigned __int64 id){
	timers_[id] = callback;
	SetTimer(thread_.message_hwnd_, static_cast<UINT_PTR>(id), static_cast<UINT>(duration.count()), timer_entry_);
}

void winp::thread::item_manager::remove_timer_(unsigned __int64 id){
	if (timers_.empty())
		return;

	if (auto it = timers_.find(id); it != timers_.end()){
		KillTimer(thread_.message_hwnd_, static_cast<UINT_PTR>(id));
		timers_.erase(it);
	}
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

bool winp::thread::item_manager::is_dialog_message_(MSG &msg) const{
	if (mouse_.pressed != nullptr)
		return mouse_.pressed->is_dialog_message_(msg);
	return (focused_object_ != nullptr && focused_object_->is_dialog_message_(msg));
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
		return get_result_(trigger_event_with_value_<events::create>(target, TRUE, true, msg, ((window_target == nullptr) ? nullptr : thread_.get_class_entry_(window_target->get_class_name()))), FALSE);
	case WM_CREATE:
		trigger_event_<events::create>(target, false, msg, ((window_target == nullptr) ? nullptr : thread_.get_class_entry_(window_target->get_class_name())));
		return 0;
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
		return paint_(target, target, msg);
	case WM_NCPAINT:
		return trigger_event_<events::non_client_paint>(target, msg, ((window_target == nullptr) ? nullptr : thread_.get_class_entry_(window_target->get_class_name()))).second;
	case WM_DRAWITEM:
		return draw_item_(target, msg);
	case WM_MEASUREITEM:
		return measure_item_(target, msg);
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
		return mouse_leave_(target, msg, GetMessagePos());
	case WM_NCMOUSEMOVE:
	case WM_MOUSEMOVE:
		return mouse_move_(target, msg, GetMessagePos());
	case WM_NCLBUTTONDOWN:
		if (mouse_.full_feature_enabled)
			return mouse_down_(target, msg, GetMessagePos(), events::mouse::button_type_left, true);
		return mouse_message_<ui::window_surface, events::mouse_down>(target, msg, events::mouse::button_type_left, true, thread_);
	case WM_LBUTTONDOWN:
		if (mouse_.full_feature_enabled)
			return mouse_down_(target, msg, GetMessagePos(), events::mouse::button_type_left, false);
		return mouse_message_<ui::window_surface, events::mouse_down>(target, msg, events::mouse::button_type_left, false, thread_);
	case WM_NCMBUTTONDOWN:
		if (mouse_.full_feature_enabled)
			return mouse_down_(target, msg, GetMessagePos(), events::mouse::button_type_middle, true);
		return mouse_message_<ui::window_surface, events::mouse_down>(target, msg, events::mouse::button_type_middle, true, thread_);
	case WM_MBUTTONDOWN:
		if (mouse_.full_feature_enabled)
			return mouse_down_(target, msg, GetMessagePos(), events::mouse::button_type_middle, false);
		return mouse_message_<ui::window_surface, events::mouse_down>(target, msg, events::mouse::button_type_middle, false, thread_);
	case WM_NCRBUTTONDOWN:
		if (mouse_.full_feature_enabled)
			return mouse_down_(target, msg, GetMessagePos(), events::mouse::button_type_right, true);
		return mouse_message_<ui::window_surface, events::mouse_down>(target, msg, events::mouse::button_type_middle, true, thread_);
	case WM_RBUTTONDOWN:
		if (mouse_.full_feature_enabled)
			return mouse_down_(target, msg, GetMessagePos(), events::mouse::button_type_right, false);
		return mouse_message_<ui::window_surface, events::mouse_down>(target, msg, events::mouse::button_type_right, false, thread_);
	case WM_NCLBUTTONUP:
		if (mouse_.full_feature_enabled)
			return mouse_up_(target, msg, GetMessagePos(), events::mouse::button_type_left, true);
		return mouse_message_<ui::window_surface, events::mouse_up>(target, msg, events::mouse::button_type_left, true, thread_);
	case WM_LBUTTONUP:
		if (mouse_.full_feature_enabled)
			return mouse_up_(target, msg, GetMessagePos(), events::mouse::button_type_left, false);
		return mouse_message_<ui::window_surface, events::mouse_up>(target, msg, events::mouse::button_type_left, false, thread_);
	case WM_NCMBUTTONUP:
		if (mouse_.full_feature_enabled)
			return mouse_up_(target, msg, GetMessagePos(), events::mouse::button_type_middle, true);
		return mouse_message_<ui::window_surface, events::mouse_up>(target, msg, events::mouse::button_type_middle, true, thread_);
	case WM_MBUTTONUP:
		if (mouse_.full_feature_enabled)
			return mouse_up_(target, msg, GetMessagePos(), events::mouse::button_type_middle, false);
		return mouse_message_<ui::window_surface, events::mouse_up>(target, msg, events::mouse::button_type_middle, false, thread_);
	case WM_NCRBUTTONUP:
		if (mouse_.full_feature_enabled)
			return mouse_up_(target, msg, GetMessagePos(), events::mouse::button_type_right, true);
		return mouse_message_<ui::window_surface, events::mouse_up>(target, msg, events::mouse::button_type_right, true, thread_);
	case WM_RBUTTONUP:
		if (mouse_.full_feature_enabled)
			return mouse_up_(target, msg, GetMessagePos(), events::mouse::button_type_right, false);
		return mouse_message_<ui::window_surface, events::mouse_up>(target, msg, events::mouse::button_type_right, false, thread_);
	case WM_NCLBUTTONDBLCLK:
		if (mouse_.full_feature_enabled)
			return mouse_dbl_clk_(target, msg, GetMessagePos(), events::mouse::button_type_left, true);
		return mouse_message_<ui::window_surface, events::mouse_dbl_clk>(target, msg, events::mouse::button_type_left, true, thread_);
	case WM_LBUTTONDBLCLK:
		if (mouse_.full_feature_enabled)
			return mouse_dbl_clk_(target, msg, GetMessagePos(), events::mouse::button_type_left, false);
		return mouse_message_<ui::window_surface, events::mouse_dbl_clk>(target, msg, events::mouse::button_type_left, false, thread_);
	case WM_NCMBUTTONDBLCLK:
		if (mouse_.full_feature_enabled)
			return mouse_dbl_clk_(target, msg, GetMessagePos(), events::mouse::button_type_middle, true);
		return mouse_message_<ui::window_surface, events::mouse_dbl_clk>(target, msg, events::mouse::button_type_middle, true, thread_);
	case WM_MBUTTONDBLCLK:
		if (mouse_.full_feature_enabled)
			return mouse_dbl_clk_(target, msg, GetMessagePos(), events::mouse::button_type_middle, false);
		return mouse_message_<ui::window_surface, events::mouse_dbl_clk>(target, msg, events::mouse::button_type_middle, false, thread_);
	case WM_NCRBUTTONDBLCLK:
		if (mouse_.full_feature_enabled)
			return mouse_dbl_clk_(target, msg, GetMessagePos(), events::mouse::button_type_right, true);
		return mouse_message_<ui::window_surface, events::mouse_dbl_clk>(target, msg, events::mouse::button_type_right, true, thread_);
	case WM_RBUTTONDBLCLK:
		if (mouse_.full_feature_enabled)
			return mouse_dbl_clk_(target, msg, GetMessagePos(), events::mouse::button_type_right, false);
		return mouse_message_<ui::window_surface, events::mouse_dbl_clk>(target, msg, events::mouse::button_type_right, false, thread_);
	case WM_MOUSEWHEEL:
	case WM_MOUSEHWHEEL:
		if (mouse_.full_feature_enabled)
			return mouse_wheel_(target, msg, GetMessagePos());
		return mouse_message_<ui::window_surface, events::mouse_wheel>(target, msg, events::mouse::button_type_right, false, thread_);
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
		return key_(target, msg);
	case WM_SETFOCUS:
		return set_focus_(target, msg);
	case WM_KILLFOCUS:
		return kill_focus_(target, msg);
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
	case WM_INITMENUPOPUP:
		return menu_init_(target, msg);
	case WM_UNINITMENUPOPUP:
		return menu_uninit_(target, msg);
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

	RECT d{};
	GetClipBox(reinterpret_cast<HDC>(msg.wParam), &d);

	if (object_context->is_created() && (window_context != nullptr || visible_context->is_visible()))
		return trigger_event_with_target_<events::erase_background>(context, target, msg, ((window_context == nullptr) ? nullptr : thread_.get_class_entry_(window_context->get_class_name()))).second;

	return 0;
}

LRESULT winp::thread::item_manager::paint_(item &context, item &target, MSG &msg){
	auto visible_context = dynamic_cast<ui::visible_surface *>(&context);
	if (visible_context == nullptr)//Visible surface required
		return 0;

	auto object_context = dynamic_cast<ui::object *>(&context);
	if (object_context == nullptr)//Object required
		return 0;

	auto is_window_context = false;
	auto paint_device_store_id = -1;

	LRESULT result = 0;
	if (auto non_window_context = dynamic_cast<ui::non_window_surface *>(&context); non_window_context != nullptr){
		if (paint_device_ == nullptr || non_window_context->handle_ == nullptr || !non_window_context->is_visible_())
			return 0;//Do nothing

		auto &position = non_window_context->get_current_position_();
		auto offset = non_window_context->convert_position_relative_to_ancestor_<ui::window_surface>(0, 0);

		auto outer_handle = non_window_context->get_outer_handle_();
		if (outer_handle != nullptr){//Check non-client
			outer_handle.move((position.x + offset.x), (position.y + offset.y));

			auto intersection = update_rgn_.get_intersection(outer_handle);
			if (intersection == nullptr)
				return 0;//Do nothing

			if (intersection.is_empty())
				return 0;

			if (outer_handle != non_window_context->handle_){
				MSG paint_msg{ msg.hwnd, WM_NCPAINT, reinterpret_cast<WPARAM>(paint_device_) };
				trigger_event_with_target_<events::non_client_paint>(context, target, paint_msg, nullptr);
			}
		}

		MSG paint_msg{ msg.hwnd, WM_ERASEBKGND, reinterpret_cast<WPARAM>(paint_device_) };
		trigger_event_with_target_<events::erase_background>(context, target, paint_msg, nullptr);

		paint_msg.message = WM_PRINTCLIENT;
		trigger_event_with_target_<events::paint>(context, target, paint_msg, nullptr);
	}
	else if (auto window_context = dynamic_cast<ui::window_surface *>(&context); window_context != nullptr){
		if (msg.hwnd != nullptr){//Store update region
			if (msg.message == WM_PAINT){
				GetUpdateRgn(msg.hwnd, update_rgn_, FALSE);
				if ((paint_device_ = GetDC(msg.hwnd)) != nullptr){
					paint_device_store_id = SaveDC(paint_device_);
					SelectClipRgn(paint_device_, update_rgn_);
				}
			}
			else
				GetClipRgn((paint_device_ = reinterpret_cast<HDC>(msg.wParam)), update_rgn_);
		}

		is_window_context = true;
		result = trigger_event_with_target_<events::paint>(context, target, msg, thread_.get_class_entry_(window_context->get_class_name())).second;
	}
	else//Ignore
		return 0;

	if (auto tree_context = dynamic_cast<ui::tree *>(&context); tree_context != nullptr){
		tree_context->traverse_children_of_<ui::non_window_surface>([&](ui::non_window_surface &child){
			paint_(child, target, msg);
			return true;
		});
	}

	if (is_window_context && msg.message == WM_PAINT && paint_device_ != nullptr){
		RestoreDC(paint_device_, paint_device_store_id);
		ReleaseDC(msg.hwnd, paint_device_);
		paint_device_ = nullptr;
	}

	return result;
}

LRESULT winp::thread::item_manager::draw_item_(item &target, MSG &msg){
	auto window_target = dynamic_cast<ui::window_surface *>(&target);
	if (window_target == nullptr)
		return trigger_event_<events::unhandled>(target, msg, nullptr).second;

	auto draw_info = reinterpret_cast<DRAWITEMSTRUCT *>(msg.lParam);
	if (draw_info->CtlType == ODT_MENU){//Menu item
		menu::item *target_item = nullptr;
		if (auto menu = (((draw_info->itemID == 0u) ? menus_.end() : menus_.find(reinterpret_cast<HMENU>(draw_info->hwndItem)))); menu != menus_.end())
			target_item = menu->second->find_item_(draw_info->itemID, nullptr, false);
		else//Try associated data
			target_item = reinterpret_cast<menu::item *>(draw_info->itemData);

		if (target_item == nullptr)
			return trigger_event_<events::unhandled>(target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;

		if (target_item->events_manager_.get_bound_count<events::draw_item>() == 0u)//Trigger ancestor's event
			return trigger_event_with_target_<events::draw_item>(*target_item->get_first_ancestor_of_<menu::object>(nullptr), *target_item, msg, thread_.get_class_entry_(window_target->get_class_name())).second;

		return trigger_event_<events::draw_item>(*target_item, msg, thread_.get_class_entry_(window_target->get_class_name())).second;
	}

	if (draw_info->CtlType == ODT_BUTTON){
		if (auto draw_target = find_window_(draw_info->hwndItem, false); draw_target != nullptr)
			return trigger_event_<events::draw_item>(*draw_target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;
		return trigger_event_<events::unhandled>(target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;
	}

	return trigger_event_<events::unhandled>(target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;
}

LRESULT winp::thread::item_manager::measure_item_(item &target, MSG &msg){
	auto window_target = dynamic_cast<ui::window_surface *>(&target);
	if (window_target == nullptr)
		return trigger_event_<events::unhandled>(target, msg, nullptr).second;

	auto measure_info = reinterpret_cast<MEASUREITEMSTRUCT *>(msg.lParam);
	if (measure_info->CtlType == ODT_MENU){
		auto target_item = reinterpret_cast<menu::item *>(measure_info->itemData);
		if (target_item == nullptr)
			return trigger_event_<events::unhandled>(target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;

		if (target_item->events_manager_.get_bound_count<events::measure_item>() == 0u)//Trigger ancestor's event
			return trigger_event_with_target_<events::measure_item>(*target_item->get_first_ancestor_of_<menu::object>(nullptr), *target_item, msg, thread_.get_class_entry_(window_target->get_class_name())).second;

		return trigger_event_<events::measure_item>(*target_item, msg, thread_.get_class_entry_(window_target->get_class_name())).second;
	}

	if (measure_info->CtlType == ODT_BUTTON){
		return trigger_event_<events::unhandled>(target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;
	}

	return trigger_event_<events::unhandled>(target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;
}

LRESULT winp::thread::item_manager::position_change_(item &target, MSG &msg, bool changing){
	auto window_target = dynamic_cast<ui::window_surface *>(&target);
	if (window_target == nullptr)
		return trigger_event_<events::unhandled>(target, msg, nullptr).second;

	if (window_target->updating_dimension_)
		return trigger_event_<events::unhandled>(target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;

	LRESULT result = 0;
	auto info = reinterpret_cast<WINDOWPOS *>(msg.lParam);

	if (!changing){
		if ((info->flags & SWP_NOMOVE) == 0u){
			auto relative_position = window_target->convert_position_relative_to_ancestor_<ui::window_surface>(0, 0);
			window_target->position_ = POINT{ (info->x - relative_position.x), (info->y - relative_position.y) };
			window_target->cancel_animation_<POINT>();
		}

		if ((info->flags & SWP_NOSIZE) == 0u){
			window_target->size_ = SIZE{ info->cx, info->cy };
			window_target->cancel_animation_<SIZE>();
		}

		result = trigger_event_<events::position_change>(target, false, msg, ((window_target == nullptr) ? nullptr : thread_.get_class_entry_(window_target->get_class_name()))).second;
		trigger_event_<events::position_updated>(target, info->flags);
		if (!window_target->has_hook_<ui::no_drag_position_updated_hook>())
			trigger_event_<events::non_drag_position_updated>(target, info->flags);
	}
	else{//Changing
		auto result_info = trigger_event_<events::position_change>(target, true, msg, ((window_target == nullptr) ? nullptr : thread_.get_class_entry_(window_target->get_class_name())));
		if ((result_info.first & events::object::state_default_prevented) != 0u)
			info->flags |= (SWP_NOMOVE | SWP_NOSIZE);

		result = result_info.second;
	}

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

LRESULT winp::thread::item_manager::mouse_leave_(item &target, MSG &msg, DWORD position){
	auto window_target = dynamic_cast<ui::window_surface *>(&target);
	if (window_target == nullptr)
		return trigger_event_<events::unhandled>(target, msg, nullptr).second;

	if (!mouse_.full_feature_enabled){
		auto position = GetMessagePos();
		if (window_target->absolute_hit_test(GET_X_LPARAM(position), GET_Y_LPARAM(position)) == HTNOWHERE){
			if (mouse_.tracking_leave == &target)
				mouse_.tracking_leave = nullptr;

			return trigger_event_<events::mouse_leave>(target, events::mouse::button_type_nil, false, msg, thread_.get_class_entry_(window_target->get_class_name())).second;
		}

		return CallWindowProcW(thread_.get_class_entry_(window_target->get_class_name()), msg.hwnd, msg.message, msg.wParam, msg.lParam);
	}

	if (mouse_.tracking_leave != &target)
		return CallWindowProcW(thread_.get_class_entry_(window_target->get_class_name()), msg.hwnd, msg.message, msg.wParam, msg.lParam);

	if (mouse_.dragging != nullptr){//End drag
		trigger_event_<events::mouse_drag_end>(*mouse_.dragging, events::mouse::button_type_nil, false, msg, nullptr);
		mouse_.dragging = nullptr;
	}

	LRESULT result = 0;
	POINT mouse_position{ GET_X_LPARAM(position), GET_Y_LPARAM(position) };

	for (auto mouse_target = mouse_.target; (mouse_.target = mouse_target) != nullptr; mouse_target = mouse_target->get_parent_()){
		auto visible_ancestor = dynamic_cast<ui::visible_surface *>(mouse_target);
		if (visible_ancestor == nullptr || !visible_ancestor->is_visible() || !mouse_target->has_hook_<ui::io_hook>())
			continue;

		if (visible_ancestor->absolute_hit_test_(mouse_position.x, mouse_position.y) != HTNOWHERE)
			break;

		if (mouse_target == &target)
			result = trigger_event_<events::mouse_leave>(*mouse_target, events::mouse::button_type_nil, true, msg, thread_.get_class_entry_(window_target->get_class_name())).second;
		else//Ignore result
			trigger_event_<events::mouse_leave>(*mouse_target, events::mouse::button_type_nil, true, msg, nullptr);
	}

	if (mouse_.target == &target && mouse_.target->has_hook_<ui::io_hook>())//Moved from non-client to client or vice versa
		result = trigger_event_<events::mouse_client_change>(target, events::mouse::button_type_nil, (msg.message == WM_MOUSELEAVE), msg, thread_.get_class_entry_(window_target->get_class_name())).second;

	mouse_.button_down = events::mouse::button_type_nil;
	mouse_.tracking_leave = nullptr;

	return result;
}

LRESULT winp::thread::item_manager::mouse_move_(item &target, MSG &msg, DWORD position){
	auto window_target = dynamic_cast<ui::window_surface *>(&target);
	if (window_target == nullptr)
		return trigger_event_<events::unhandled>(target, msg, nullptr).second;

	if (!mouse_.full_feature_enabled){
		if (mouse_.tracking_leave != &target){
			track_mouse_leave_(msg.hwnd, ((msg.message == WM_MOUSEMOVE) ? 0 : TME_NONCLIENT));
			mouse_.tracking_leave = window_target;
		}

		return trigger_event_<events::mouse_move>(target, events::mouse::button_type_nil, false, msg, thread_.get_class_entry_(window_target->get_class_name())).second;
	}

	POINT mouse_position{ GET_X_LPARAM(position), GET_Y_LPARAM(position) };
	for (auto mouse_target = mouse_.target; mouse_target != nullptr && mouse_target != mouse_.pressed && (mouse_.target = mouse_target) != &target; mouse_target = mouse_target->get_parent_()){
		auto visible_ancestor = dynamic_cast<ui::visible_surface *>(mouse_target);
		if (visible_ancestor == nullptr || !visible_ancestor->is_visible() || !mouse_target->has_hook_<ui::io_hook>())
			continue;

		if (visible_ancestor->absolute_hit_test_(mouse_position.x, mouse_position.y) != HTNOWHERE)
			break;

		if (mouse_.dragging == mouse_target){//End drag
			trigger_event_<events::mouse_drag_end>(*mouse_.dragging, events::mouse::button_type_nil, false, msg, nullptr);
			mouse_.dragging = nullptr;
		}

		trigger_event_<events::mouse_leave>(*mouse_target, events::mouse::button_type_nil, true, msg, nullptr);
		mouse_.button_down = events::mouse::button_type_nil;

		if (mouse_.tracking_leave == mouse_target)
			mouse_.tracking_leave = nullptr;
	}

	if (mouse_.tracking_leave != &target){
		track_mouse_leave_(msg.hwnd, ((msg.message == WM_MOUSEMOVE) ? 0 : TME_NONCLIENT));
		mouse_.tracking_leave = window_target;
	}

	ui::object *deepest_target = nullptr;
	if (mouse_.pressed != mouse_.target && msg.message == WM_MOUSEMOVE){//Find deepest mouse target
		auto mouse_target = ((mouse_.target == nullptr) ? window_target : mouse_.target);
		auto tree_ancestor = mouse_target->get_first_ancestor_of_<ui::tree>(nullptr);

		if (tree_ancestor != nullptr || (tree_ancestor = dynamic_cast<ui::tree *>(mouse_target)) != nullptr){
			auto &children = tree_ancestor->get_children();
			for (auto it = children.rbegin(); it != children.rend(); ++it){
				if ((deepest_target = find_deepest_mouse_target(**it, mouse_position)) != nullptr)
					break;
			}
		}
	}
	else if (mouse_.pressed == mouse_.target)
		deepest_target = mouse_.pressed;

	if (deepest_target == nullptr)
		deepest_target = ((mouse_.target == nullptr || window_target->is_ancestor_(*dynamic_cast<ui::tree *>(mouse_.target))) ? window_target : mouse_.target);

	if (auto tree_mouse_target = dynamic_cast<ui::tree *>(mouse_.target); tree_mouse_target != nullptr && deepest_target != mouse_.target && !deepest_target->is_ancestor_(*tree_mouse_target)){
		if (mouse_.dragging == tree_mouse_target){//End drag
			trigger_event_<events::mouse_drag_end>(*mouse_.dragging, events::mouse::button_type_nil, false, msg, nullptr);
			mouse_.dragging = nullptr;
		}

		trigger_event_<events::mouse_leave>(*tree_mouse_target, events::mouse::button_type_nil, true, msg, nullptr);
		mouse_.target = mouse_.target->get_first_ancestor_of_<ui::tree>(nullptr);
	}

	for (auto mouse_target = deepest_target; mouse_target != nullptr && mouse_target != mouse_.target; mouse_target = mouse_target->get_parent_()){
		if (dynamic_cast<ui::visible_surface *>(mouse_target) != nullptr && mouse_target->has_hook_<ui::io_hook>())
			trigger_event_<events::mouse_enter>(*mouse_target, events::mouse::button_type_nil, (msg.message == WM_NCMOUSEMOVE), msg, nullptr);
	}

	LRESULT result = 0;
	std::pair<unsigned int, LRESULT> result_info;

	auto bubbled_to_target = false;
	for (auto mouse_target = (mouse_.target = deepest_target); mouse_target != nullptr; mouse_target = mouse_target->get_parent_()){
		auto visible_ancestor = dynamic_cast<ui::visible_surface *>(mouse_target);
		if (visible_ancestor == nullptr || !visible_ancestor->is_visible() || !mouse_target->has_hook_<ui::io_hook>())
			continue;

		if (mouse_target == &target){
			result = (result_info = trigger_event_with_target_<events::mouse_move>(*mouse_target, *mouse_.target, events::mouse::button_type_nil, (msg.message == WM_NCMOUSEMOVE), msg, thread_.get_class_entry_(window_target->get_class_name()))).second;
			bubbled_to_target = true;
		}
		else//Ignore result
			result_info = trigger_event_with_target_<events::mouse_move>(*mouse_target, *mouse_.target, events::mouse::button_type_nil, (visible_ancestor->absolute_hit_test_(mouse_position.x, mouse_position.y) != HTCLIENT), msg, nullptr);

		if ((result_info.first & events::object::state_propagation_stopped) != 0u)
			break;//Propagation stopped
	}

	if (mouse_.dragging != nullptr && mouse_.dragging != mouse_.target){//Different target
		if (auto tree_dragging_target = dynamic_cast<ui::tree *>(mouse_.dragging); tree_dragging_target == nullptr || !mouse_.target->is_ancestor_(*tree_dragging_target)){
			trigger_event_<events::mouse_drag_end>(*mouse_.dragging, events::mouse::button_type_nil, false, msg, nullptr);
			mouse_.dragging = nullptr;
		}
	}

	if (msg.message == WM_MOUSEMOVE && mouse_.dragging == nullptr && mouse_.button_down != events::mouse::button_type_nil){//Check for drag begin
		SIZE mouse_delta{
			std::abs(mouse_position.x - mouse_.down_position.x),
			std::abs(mouse_position.y - mouse_.down_position.y)
		};

		if (mouse_.drag_threshold.cx <= mouse_delta.cx || mouse_.drag_threshold.cy <= mouse_delta.cy){//Drag begin
			for (auto mouse_target = mouse_.target; mouse_target != nullptr; mouse_target = mouse_target->get_parent_()){
				auto visible_ancestor = dynamic_cast<ui::visible_surface *>(mouse_target);
				if (visible_ancestor == nullptr || !visible_ancestor->is_visible() || !mouse_target->has_hook_<ui::io_hook>())
					continue;

				result_info = trigger_event_with_target_<events::mouse_drag_begin>(*mouse_target, *mouse_.target, mouse_.down_position, mouse_.button_down, false, msg, nullptr);
				if ((result_info.first & events::object::state_default_prevented) == 0u && result_info.second != FALSE){
					mouse_.dragging = mouse_target;
					break;
				}

				if ((result_info.first & events::object::state_propagation_stopped) != 0u)
					break;//Propagation stopped
			}

			if (mouse_.dragging != nullptr)//Begin drag
				result_info = trigger_event_<events::mouse_drag>(*mouse_.target, mouse_.down_position, mouse_.button_down, false, msg, nullptr);
		}
	}
	else if (mouse_.dragging != nullptr && mouse_.button_down == events::mouse::button_type_nil){//End drag
		trigger_event_<events::mouse_drag_end>(*mouse_.dragging, events::mouse::button_type_nil, false, msg, nullptr);
		mouse_.dragging = nullptr;
	}
	else if (mouse_.dragging != nullptr)//Continue drag
		trigger_event_<events::mouse_drag>(*mouse_.dragging, mouse_.last_position, mouse_.button_down, false, msg, nullptr);

	mouse_.last_position = mouse_position;
	if (auto non_window_mouse_target = dynamic_cast<ui::non_window_surface *>(mouse_.target); non_window_mouse_target != nullptr && non_window_mouse_target->has_hook_<ui::edge_drag_hook>()){
		UINT hit_target;
		if (mouse_.pressed == nullptr || mouse_.dragging != nullptr)
			hit_target = non_window_mouse_target->absolute_hit_test_(mouse_position.x, mouse_position.y);
		else//Use down position
			hit_target = non_window_mouse_target->absolute_hit_test_(mouse_.down_position.x, mouse_.down_position.y);

		if (hit_target != HTCLIENT){
			MSG cursor_msg{ nullptr, 0, 0, MAKEWORD(hit_target, 0) };
			SetCursor(get_default_cursor_(cursor_msg));
		}
	}

	return (bubbled_to_target ? result : CallWindowProcW(thread_.get_class_entry_(window_target->get_class_name()), msg.hwnd, msg.message, msg.wParam, msg.lParam));
}

LRESULT winp::thread::item_manager::mouse_down_(item &target, MSG &msg, DWORD position, unsigned int button, bool is_non_client){
	return mouse_button_<ui::window_surface, events::mouse_down>(target, msg, position, button, is_non_client, thread_, [&]{
		POINT mouse_position{ GET_X_LPARAM(position), GET_Y_LPARAM(position) };
		if (mouse_.button_down == events::mouse::button_type_nil)
			mouse_.down_position = mouse_position;

		mouse_.button_down |= button;
		mouse_.pressed = mouse_.target;

		if (auto non_window_mouse_target = dynamic_cast<ui::non_window_surface *>(mouse_.target); non_window_mouse_target != nullptr && non_window_mouse_target->has_hook_<ui::edge_drag_hook>()){
			auto hit_target = non_window_mouse_target->absolute_hit_test_(mouse_position.x, mouse_position.y);
			if (hit_target != HTCLIENT){
				MSG cursor_msg{ nullptr, 0, 0, MAKEWORD(hit_target, 0) };
				SetCursor(get_default_cursor_(cursor_msg));
			}
		}
	});
}

LRESULT winp::thread::item_manager::mouse_up_(item &target, MSG &msg, DWORD position, unsigned int button, bool is_non_client){
	return mouse_button_<ui::window_surface, events::mouse_up>(target, msg, position, button, is_non_client, thread_, [&]{
		mouse_.button_down &= ~button;
		if (mouse_.button_down == events::mouse::button_type_nil){
			mouse_.pressed = nullptr;
			if (mouse_.dragging != nullptr){
				trigger_event_<events::mouse_drag_end>(*mouse_.dragging, events::mouse::button_type_nil, false, msg, nullptr);
				mouse_.dragging = nullptr;
			}
		}

		POINT mouse_position{ GET_X_LPARAM(position), GET_Y_LPARAM(position) };
		if (auto non_window_mouse_target = dynamic_cast<ui::non_window_surface *>(mouse_.target); non_window_mouse_target != nullptr && non_window_mouse_target->has_hook_<ui::edge_drag_hook>()){
			auto hit_target = non_window_mouse_target->absolute_hit_test_(mouse_position.x, mouse_position.y);
			if (hit_target != HTCLIENT){
				MSG cursor_msg{ nullptr, 0, 0, MAKEWORD(hit_target, 0) };
				SetCursor(get_default_cursor_(cursor_msg));
			}
		}
	});
}

LRESULT winp::thread::item_manager::mouse_dbl_clk_(item &target, MSG &msg, DWORD position, unsigned int button, bool is_non_client){
	return mouse_button_<ui::window_surface, events::mouse_dbl_clk>(target, msg, position, button, is_non_client, thread_, nullptr);
}

LRESULT winp::thread::item_manager::mouse_wheel_(item &target, MSG &msg, DWORD position){
	return mouse_button_<ui::window_surface, events::mouse_wheel>(target, msg, position, events::mouse::button_type_nil, false, thread_, nullptr);
}

LRESULT winp::thread::item_manager::key_(item &target, MSG &msg){
	auto window_target = dynamic_cast<ui::window_surface *>(&target);
	if (!mouse_.full_feature_enabled)
		return ((window_target == nullptr) ? 0 : trigger_key_event_<ui::window_surface>(target, target, true, msg, thread_).second);

	auto key_target = ((mouse_.pressed == nullptr) ? window_target : mouse_.pressed);
	if (key_target == nullptr)
		return trigger_event_<events::unhandled>(target, msg, nullptr).second;

	LRESULT result = 0;
	std::pair<unsigned int, LRESULT> result_info;

	for (auto m_key_target = key_target; m_key_target != nullptr; m_key_target = m_key_target->get_parent_()){
		auto visible_ancestor = dynamic_cast<ui::visible_surface *>(m_key_target);
		if (visible_ancestor == nullptr || !visible_ancestor->is_visible() || !m_key_target->has_hook_<ui::io_hook>())
			continue;

		if (m_key_target == &target)
			result = (result_info = trigger_key_event_<ui::window_surface>(*m_key_target, *key_target, true, msg, thread_)).second;
		else//Ignore result
			result_info = trigger_key_event_<ui::window_surface>(*m_key_target, *key_target, false, msg, thread_);

		if ((result_info.first & events::object::state_propagation_stopped) != 0u)
			break;//Propagation stopped
	}

	return result;
}

LRESULT winp::thread::item_manager::set_focus_(item &target, MSG &msg){
	auto window_target = dynamic_cast<ui::window_surface *>(&target);
	if (window_target == nullptr)
		return trigger_event_<events::unhandled>(target, msg, nullptr).second;

	focused_object_ = window_target;
	if (mouse_.full_feature_enabled && mouse_.pressed != nullptr && mouse_.pressed != focused_object_ && !mouse_.pressed->is_ancestor_(*window_target))
		mouse_.pressed = nullptr;

	return trigger_event_<events::set_focus>(target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;
}

LRESULT winp::thread::item_manager::kill_focus_(item &target, MSG &msg){
	auto window_target = dynamic_cast<ui::window_surface *>(&target);
	if (window_target == nullptr)
		return trigger_event_<events::unhandled>(target, msg, nullptr).second;

	focused_object_ = nullptr;
	if (mouse_.full_feature_enabled && mouse_.pressed != nullptr && mouse_.pressed != window_target){
		auto lost_focus_window = find_window_(reinterpret_cast<HWND>(msg.wParam), false);
		if (lost_focus_window == nullptr || !mouse_.pressed->is_ancestor_(*lost_focus_window))
			mouse_.pressed = nullptr;
	}
	else
		mouse_.pressed = nullptr;

	return trigger_event_<events::kill_focus>(target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;
}

LRESULT winp::thread::item_manager::menu_select_(item &target, MSG &msg){
	auto window_target = dynamic_cast<ui::window_surface *>(&target);
	if (window_target == nullptr)
		return trigger_event_<events::unhandled>(target, msg, nullptr).second;

	auto flags = HIWORD(msg.wParam);
	if ((flags & MF_HILITE) == 0u || menus_.empty())
		return trigger_event_<events::unhandled>(target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;

	menu::item *menu_item_target = nullptr;
	auto menu = menus_.find(reinterpret_cast<HMENU>(msg.lParam));
	
	if (menu == menus_.end())
		return trigger_event_<events::unhandled>(target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;

	if ((flags & MF_POPUP) != 0u)//By index
		menu_item_target = menu->second->get_item_at(static_cast<UINT>(msg.wParam));
	else//By ID
		menu_item_target = menu->second->find_item(static_cast<UINT>(msg.wParam));

	if (menu_item_target == nullptr)//find_sub_menu_item_
		return trigger_event_<events::unhandled>(target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;

	return trigger_event_<events::menu_item_highlight>(*menu_item_target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;
}

LRESULT winp::thread::item_manager::menu_command_(item &target, MSG &msg){
	clear_black_listed_menus_on_exit clear(*this);

	auto window_target = dynamic_cast<ui::window_surface *>(&target);
	if (window_target == nullptr)
		return trigger_event_<events::unhandled>(target, msg, nullptr).second;

	auto menu = menus_.find(reinterpret_cast<HMENU>(msg.lParam));
	if (menu == menus_.end())
		return trigger_event_<events::unhandled>(target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;

	if (auto menu_item = menu->second->get_item_at(static_cast<UINT>(msg.wParam)); menu_item != nullptr)
		return trigger_event_<events::menu_item_select>(*menu_item, msg, thread_.get_class_entry_(window_target->get_class_name())).second;

	auto menu_item_target = menu->second->get_item_at(static_cast<UINT>(msg.wParam));
	if (menu_item_target == nullptr)
		return trigger_event_<events::unhandled>(target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;

	return trigger_event_<events::unhandled>(target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;
}

LRESULT winp::thread::item_manager::system_command_(item &target, MSG &msg){
	auto window_target = dynamic_cast<ui::window_surface *>(&target);
	if (window_target == nullptr)
		return trigger_event_<events::unhandled>(target, msg, nullptr).second;

	auto menu_handle = GetSystemMenu(window_target->handle_, FALSE);
	auto menu = menus_.find(menu_handle);

	if (menu == menus_.end())//System menu not initialized
		return trigger_event_<events::unhandled>(target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;

	if (auto menu_item = menu->second->find_item_((static_cast<UINT>(msg.wParam) & 0xFFF0u), nullptr, true); menu_item != nullptr){
		clear_black_listed_menus_on_exit clear(*this);
		return trigger_event_<events::menu_item_select>(*menu_item, msg, thread_.get_class_entry_(window_target->get_class_name())).second;
	}

	return trigger_event_<events::unhandled>(target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;
}

LRESULT winp::thread::item_manager::context_menu_(item &target, MSG &msg){
	clear_black_listed_modified_menus_();
	if (!black_listed_menus_.empty())
		black_listed_menus_.clear();

	auto window_target = dynamic_cast<ui::window_surface *>(&target);
	if (window_target == nullptr)
		return trigger_event_<events::unhandled>(target, msg, nullptr).second;

	POINT position{ GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam) };
	if ((position.x != -1 || position.y != -1) && window_target->absolute_hit_test(position) != HTCLIENT)//Non-client area
		return trigger_event_<events::unhandled>(target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;

	if (msg.wParam != 0 && find_window_(reinterpret_cast<HWND>(msg.wParam), false) != &target)
		return trigger_event_<events::unhandled>(target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;

	if ((active_context_menu_ = std::make_shared<menu::popup>()) == nullptr || active_context_menu_->create() != utility::error_code::nil)
		return trigger_event_<events::unhandled>(target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;

	ui::object *mouse_target = nullptr;
	std::pair<unsigned int, LRESULT> result_info;
	
	if (mouse_.full_feature_enabled && msg.message == WM_CONTEXTMENU){
		for (mouse_target = mouse_.target; mouse_target != nullptr; mouse_target = mouse_target->get_parent_()){
			auto visible_ancestor = dynamic_cast<ui::visible_surface *>(mouse_target);
			if (visible_ancestor == nullptr || !visible_ancestor->is_visible() || !mouse_target->has_hook_<ui::io_hook>())
				continue;

			result_info = trigger_event_with_target_<events::context_menu>(*mouse_target, *mouse_.target, *active_context_menu_, msg, nullptr);
			if (result_info.second != 0 || (result_info.first & (events::object::state_default_prevented | events::object::state_propagation_stopped)) != 0u || GetMenuItemCount(active_context_menu_->get_handle()) != 0)
				break;//Result set OR Default prevented OR Propagation stopped OR Empty menu
		}
	}
	else if (msg.message == WM_CONTEXTMENU)//Simple
		result_info = trigger_event_<events::context_menu>(*(mouse_target = window_target), *active_context_menu_, msg, thread_.get_class_entry_(window_target->get_class_name()));
	else//Split button menu
		result_info = trigger_event_<events::split_button_menu>(*(mouse_target = window_target), *active_context_menu_, msg, thread_.get_class_entry_(window_target->get_class_name()));

	if ((result_info.first & events::object::state_default_prevented) != 0u)
		return 0;//Default prevented

	if (mouse_target == nullptr || (result_info.second == 0 && GetMenuItemCount(active_context_menu_->get_handle()) == 0))
		return 0;//No response

	if (msg.message == WM_CONTEXTMENU && position.x == -1 && position.y == -1){//Retrieve position
		auto value = trigger_event_with_target_<events::get_context_menu_position>(*mouse_target, *mouse_.target, msg, nullptr).second;
		position = POINT{ GET_X_LPARAM(value), GET_Y_LPARAM(value) };
	}
	else if (msg.message != WM_CONTEXTMENU){//Retrieve position for split button menu
		auto value = trigger_event_with_target_<events::get_split_button_menu_position>(*mouse_target, *mouse_.target, msg, nullptr).second;
		position = POINT{ GET_X_LPARAM(value), GET_Y_LPARAM(value) };
	}

	menu::popup *menu;
	if (result_info.second == 0)
		menu = active_context_menu_.get();
	else//Menu returned
		menu = reinterpret_cast<menu::popup *>(result_info.second);

	if (!menu->is_created_() || menu->get_items_count_() == 0u)
		return 0;//Empty menu

	menu->target_ = mouse_target;
	if (auto window_mouse_target = dynamic_cast<ui::window_surface *>(mouse_target); window_mouse_target != nullptr && menu == &window_mouse_target->system_menu_){
		menu->traverse_items_([&](menu::item &item){
			switch (item.local_id_){
			case SC_RESTORE:
				item.set_enabled_state(window_mouse_target->is_maximized() || window_mouse_target->is_minimized());
				break;
			case SC_MOVE:
			case SC_SIZE:
				item.set_enabled_state(!window_mouse_target->is_maximized() && !window_mouse_target->is_minimized());
				break;
			case SC_MAXIMIZE:
				item.set_enabled_state(!window_mouse_target->is_maximized());
				break;
			case SC_MINIMIZE:
				item.set_enabled_state(!window_mouse_target->is_minimized());
				break;
			case SC_CLOSE:
				item.set_enabled_state(true);
				item.add_states(MFS_DEFAULT);
				break;
			default:
				break;
			}

			return true;
		});

		auto code = TrackPopupMenu(menu->get_handle(), (GetSystemMetrics(SM_MENUDROPALIGNMENT) | TPM_RIGHTBUTTON | TPM_RETURNCMD), position.x, position.y, 0, window_target->handle_, nullptr);
		SendMessageW(window_mouse_target->handle_, WM_SYSCOMMAND, code, 0);
	}
	else//Not a system menu
		TrackPopupMenu(menu->get_handle(), (GetSystemMetrics(SM_MENUDROPALIGNMENT) | TPM_RIGHTBUTTON), position.x, position.y, 0, window_target->handle_, nullptr);
	
	menu->target_ = nullptr;
	return 0;
}

LRESULT winp::thread::item_manager::menu_init_(item &target, MSG &msg){
	clear_black_listed_modified_menus_();
	if (!black_listed_menus_.empty())
		black_listed_menus_.clear();

	auto window_target = dynamic_cast<ui::window_surface *>(&target);
	if (window_target == nullptr)
		return trigger_event_<events::unhandled>(target, msg, nullptr).second;

	menu::popup *menu = nullptr;
	auto menu_handle = reinterpret_cast<HMENU>(msg.wParam);

	if (auto it = menus_.find(menu_handle); it == menus_.end() || (dynamic_cast<menu::wrapped_popup *>(it->second) != nullptr && dynamic_cast<menu::system_popup *>(it->second) == nullptr)){//Wrap menu
		if (auto wrapped_menu = std::make_shared<menu::wrapped_popup>(menu_handle)){
			menu = wrapped_menu.get();
			wrapped_menus_[menu_handle] = wrapped_menu;
		}
	}
	else//Found
		menu = dynamic_cast<menu::popup *>(it->second);

	if (menu == nullptr)
		return trigger_event_<events::unhandled>(target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;

	auto popup_target = menu->get_target_();
	if (popup_target == nullptr)
		popup_target = window_target;

	menu->is_modifying_ = true;
	trigger_event_<events::modify_context_menu>(*popup_target, *menu, msg, nullptr);

	menu->is_modifying_ = false;
	menu->frontend_handle_ = window_target->handle_;

	return trigger_event_<events::menu_init>(*menu, msg, thread_.get_class_entry_(window_target->get_class_name())).second;
}

LRESULT winp::thread::item_manager::menu_uninit_(item &target, MSG &msg){
	auto window_target = dynamic_cast<ui::window_surface *>(&target);
	if (window_target == nullptr)
		return trigger_event_<events::unhandled>(target, msg, nullptr).second;

	auto menu_handle = reinterpret_cast<HMENU>(msg.wParam);
	auto menu = menus_.find(menu_handle);
	if (menu == menus_.end())
		return trigger_event_<events::unhandled>(target, msg, thread_.get_class_entry_(window_target->get_class_name())).second;

	if (menu->second == active_context_menu_.get()){
		black_listed_menus_.push_back(active_context_menu_);
		active_context_menu_ = nullptr;
	}
	else if (auto it = wrapped_menus_.find(menu_handle); it != wrapped_menus_.end()){
		black_listed_menus_.push_back(it->second);
		wrapped_menus_.erase(it);
	}

	menu->second->frontend_handle_ = nullptr;
	if (auto popup_menu = dynamic_cast<menu::popup *>(menu->second); popup_menu != nullptr)
		black_listed_modified_menus_.push_back(popup_menu);

	return trigger_event_<events::menu_uninit>(*menu->second, msg, thread_.get_class_entry_(window_target->get_class_name())).second;
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

void winp::thread::item_manager::clear_black_listed_modified_menus_(){
	if (!black_listed_modified_menus_.empty()){
		for (auto item : black_listed_modified_menus_)
			item->clear_modified_list_();
		black_listed_modified_menus_.clear();
	}
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

LRESULT winp::thread::item_manager::get_result_(const std::pair<unsigned int, LRESULT> &info, LRESULT prevented_result){
	return (((info.first & events::object::state_default_prevented) == 0u) ? info.second : prevented_result);
}

LRESULT CALLBACK winp::thread::item_manager::entry_(HWND handle, UINT message, WPARAM wparam, LPARAM lparam){
	auto current_thread = app::object::get_current_thread();
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
	auto current_thread = app::object::get_current_thread();
	if (current_thread == nullptr)
		return CallNextHookEx(nullptr, code, wparam, lparam);

	auto &manager = current_thread->get_item_manager();
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

void CALLBACK winp::thread::item_manager::timer_entry_(HWND handle, UINT message, UINT_PTR id, DWORD time){
	auto current_thread = app::object::get_current_thread();
	if (current_thread == nullptr)
		return;

	auto &manager = current_thread->get_item_manager();
	if (manager.timers_.empty())
		return;

	if (auto it = manager.timers_.find(static_cast<unsigned __int64>(id)); it != manager.timers_.end())
		it->second();//Call handler
}
