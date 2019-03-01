#include "../app/app_collection.h"
#include "../ui/ui_window_surface.h"
#include "../ui/ui_non_window_surface.h"

winp::thread::item_manager::item_manager(object &thread)
	: item_manager(thread, thread.get_local_id()){}

winp::thread::item_manager::item_manager(object &thread, DWORD thread_id)
	: thread_(thread){
	hook_handle_ = SetWindowsHookExW(WH_CBT, hook_entry_, nullptr, thread_id);
	mouse_.drag_threshold.cx = GetSystemMetrics(SM_CXDRAG);
	mouse_.drag_threshold.cy = GetSystemMetrics(SM_CYDRAG);
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

const POINT &winp::thread::item_manager::get_last_mouse_position() const{
	if (!is_thread_context())
		throw utility::error_code::outside_thread_context;
	return mouse_.last_position;
}

const POINT &winp::thread::item_manager::get_mouse_down_position() const{
	if (!is_thread_context())
		throw utility::error_code::outside_thread_context;
	return mouse_.down_position;
}

unsigned int winp::thread::item_manager::get_mouse_button_down() const{
	if (!is_thread_context())
		throw utility::error_code::outside_thread_context;
	return mouse_.button_down;
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
		return mouse_leave_(target, msg, GetMessagePos());
	case WM_NCMOUSEMOVE:
	case WM_MOUSEMOVE:
		return mouse_move_(target, msg, GetMessagePos());
	case WM_NCLBUTTONDOWN:
		return mouse_down_(target, msg, GetMessagePos(), events::mouse::button_type_left, true);
	case WM_LBUTTONDOWN:
		return mouse_down_(target, msg, GetMessagePos(), events::mouse::button_type_left, false);
	case WM_NCMBUTTONDOWN:
		return mouse_down_(target, msg, GetMessagePos(), events::mouse::button_type_middle, true);
	case WM_MBUTTONDOWN:
		return mouse_down_(target, msg, GetMessagePos(), events::mouse::button_type_middle, false);
	case WM_NCRBUTTONDOWN:
		return mouse_down_(target, msg, GetMessagePos(), events::mouse::button_type_right, true);
	case WM_RBUTTONDOWN:
		return mouse_down_(target, msg, GetMessagePos(), events::mouse::button_type_right, false);
	case WM_NCLBUTTONUP:
		return mouse_up_(target, msg, GetMessagePos(), events::mouse::button_type_left, true);
	case WM_LBUTTONUP:
		return mouse_up_(target, msg, GetMessagePos(), events::mouse::button_type_left, false);
	case WM_NCMBUTTONUP:
		return mouse_up_(target, msg, GetMessagePos(), events::mouse::button_type_middle, true);
	case WM_MBUTTONUP:
		return mouse_up_(target, msg, GetMessagePos(), events::mouse::button_type_middle, false);
	case WM_NCRBUTTONUP:
		return mouse_up_(target, msg, GetMessagePos(), events::mouse::button_type_right, true);
	case WM_RBUTTONUP:
		return mouse_up_(target, msg, GetMessagePos(), events::mouse::button_type_right, false);
	case WM_NCLBUTTONDBLCLK:
		return mouse_dbl_clk_(target, msg, GetMessagePos(), events::mouse::button_type_left, true);
	case WM_LBUTTONDBLCLK:
		return mouse_dbl_clk_(target, msg, GetMessagePos(), events::mouse::button_type_left, false);
	case WM_NCMBUTTONDBLCLK:
		return mouse_dbl_clk_(target, msg, GetMessagePos(), events::mouse::button_type_middle, true);
	case WM_MBUTTONDBLCLK:
		return mouse_dbl_clk_(target, msg, GetMessagePos(), events::mouse::button_type_middle, false);
	case WM_NCRBUTTONDBLCLK:
		return mouse_dbl_clk_(target, msg, GetMessagePos(), events::mouse::button_type_right, true);
	case WM_RBUTTONDBLCLK:
		return mouse_dbl_clk_(target, msg, GetMessagePos(), events::mouse::button_type_right, false);
	case WM_MOUSEWHEEL:
	case WM_MOUSEHWHEEL:
		return mouse_wheel_(target, msg, GetMessagePos());
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

LRESULT winp::thread::item_manager::mouse_leave_(item &context, MSG &msg, DWORD position){
	auto window_context = dynamic_cast<ui::window_surface *>(&context);
	if (window_context == nullptr)//Window surface required
		return 0;

	POINT mouse_position{ GET_X_LPARAM(position), GET_Y_LPARAM(position) };
	MSG mouse_msg{ nullptr, WM_NCMOUSELEAVE }, client_mouse_msg{ nullptr, WM_MOUSELEAVE };

	for (auto object_mouse_target = dynamic_cast<ui::object *>(mouse_.target); object_mouse_target != nullptr && object_mouse_target != &context; object_mouse_target = object_mouse_target->get_parent()){
		trigger_event_<events::mouse_leave>(*object_mouse_target, client_mouse_msg, nullptr);
		trigger_event_<events::mouse_leave>(*object_mouse_target, mouse_msg, nullptr);
	}

	LRESULT result = 0;
	auto mouse_leave = false;

	if (msg.message == WM_MOUSELEAVE){
		result = trigger_event_<events::mouse_leave>(context, msg, thread_.get_app().get_class_entry(window_context->get_class_name())).second;
		if (window_context->absolute_hit_test(mouse_position) == HTNOWHERE){//Mouse leave
			trigger_event_<events::mouse_leave>(*window_context, mouse_msg, nullptr);
			mouse_.target = dynamic_cast<ui::interactive_surface *>(window_context->get_parent());
			mouse_leave = true;
		}
		else{//Inside non-client
			mouse_.target = window_context;
			track_mouse_leave_(msg.hwnd, TME_NONCLIENT);
		}
	}
	else if (window_context->absolute_hit_test(mouse_position) == HTCLIENT){//Inside client
		mouse_.target = window_context;
		client_mouse_msg.message = WINP_WM_MOUSEENTER;
		trigger_event_<events::mouse_enter>(context, client_mouse_msg, nullptr);
		track_mouse_leave_(msg.hwnd, 0);
	}
	else{//Mouse leave
		trigger_event_<events::mouse_leave>(*window_context, client_mouse_msg, nullptr);
		result = trigger_event_<events::mouse_enter>(context, msg, thread_.get_app().get_class_entry(window_context->get_class_name())).second;
		mouse_.target = dynamic_cast<ui::interactive_surface *>(window_context->get_parent());
		mouse_leave = true;
	}

	if (mouse_.target != window_context && mouse_.target != nullptr){//Mouse leave
		client_mouse_msg.message = WM_MOUSELEAVE;
		for (auto object_mouse_target = dynamic_cast<ui::object *>(mouse_.target); object_mouse_target != nullptr;){
			trigger_event_<events::mouse_leave>(*object_mouse_target, client_mouse_msg, nullptr);
			trigger_event_<events::mouse_leave>(*object_mouse_target, mouse_msg, nullptr);
			mouse_.target = dynamic_cast<ui::interactive_surface *>(object_mouse_target = object_mouse_target->get_parent());
		}
	}

	if (mouse_leave && mouse_.dragging != nullptr){
		MSG mouse_msg{ nullptr, WINP_WM_MOUSEDRAGEND };
		trigger_event_<events::mouse_drag_end>(*dynamic_cast<ui::object *>(mouse_.dragging), mouse_msg, nullptr);
		mouse_.dragging = nullptr;
	}

	return result;
}

LRESULT winp::thread::item_manager::mouse_move_(item &context, MSG &msg, DWORD position){
	auto window_context = dynamic_cast<ui::window_surface *>(&context);
	if (window_context == nullptr)//Window surface required
		return 0;

	auto mouse_enter = false;
	POINT mouse_position{ GET_X_LPARAM(position), GET_Y_LPARAM(position) };

	if (msg.message == WM_MOUSEMOVE){//Resolve moused
		MSG mouse_msg{ nullptr, WM_NCMOUSELEAVE }, client_mouse_msg{ nullptr, WM_MOUSELEAVE };
		while (mouse_.target != nullptr && mouse_.target != window_context && mouse_.target->absolute_hit_test_(mouse_position.x, mouse_position.y) == HTNOWHERE){
			auto object_mouse_target = dynamic_cast<ui::object *>(mouse_.target);
			if (object_mouse_target == nullptr)
				break;

			trigger_event_<events::mouse_leave>(*object_mouse_target, client_mouse_msg, nullptr);
			trigger_event_<events::mouse_leave>(*object_mouse_target, mouse_msg, nullptr);

			mouse_.target = dynamic_cast<ui::interactive_surface *>(object_mouse_target->get_parent());
		}

		mouse_msg.message = WINP_WM_NCMOUSEENTER;
		client_mouse_msg.message = WINP_WM_MOUSEENTER;

		if (auto tree_mouse_target = dynamic_cast<ui::tree *>(mouse_.target); tree_mouse_target == nullptr || dynamic_cast<ui::object *>(&context)->is_ancestor(*tree_mouse_target)){//Mouse enter
			trigger_event_<events::mouse_enter>(context, mouse_msg, nullptr);
			if ((mouse_.target = window_context)->absolute_hit_test_(mouse_position.x, mouse_position.y) == HTCLIENT)
				trigger_event_<events::mouse_enter>(context, client_mouse_msg, nullptr);

			track_mouse_leave_(msg.hwnd, 0);
			mouse_enter = true;
		}

		if (auto deepest_mouse_target = mouse_.target->find_deepest_moused_(mouse_position); deepest_mouse_target != nullptr){//Trigger mouse enter
			for (auto mouse_target = deepest_mouse_target; mouse_target != mouse_.target; ){
				auto object_mouse_target = dynamic_cast<ui::object *>(mouse_target);
				if (object_mouse_target == nullptr)
					break;

				trigger_event_<events::mouse_enter>(*object_mouse_target, mouse_msg, nullptr);
				trigger_event_<events::mouse_enter>(*object_mouse_target, client_mouse_msg, nullptr);

				mouse_target = dynamic_cast<ui::interactive_surface *>(object_mouse_target->get_parent());
			}

			mouse_.target = deepest_mouse_target;
		}
	}
	else if (mouse_.target == nullptr){//Mouse enter
		MSG mouse_msg{ nullptr, WINP_WM_NCMOUSEENTER }, client_mouse_msg{ nullptr, WINP_WM_MOUSEENTER };
		trigger_event_<events::mouse_enter>(context, mouse_msg, nullptr);
		if ((mouse_.target = window_context)->absolute_hit_test_(mouse_position.x, mouse_position.y) == HTCLIENT)
			trigger_event_<events::mouse_enter>(context, client_mouse_msg, nullptr);

		track_mouse_leave_(msg.hwnd, TME_NONCLIENT);
		mouse_enter = true;
	}

	LRESULT result = 0;
	std::pair<unsigned int, LRESULT> result_info;

	MSG mouse_msg{ nullptr, WM_MOUSEMOVE };
	for (auto object_mouse_target = dynamic_cast<ui::object *>(mouse_.target), target = object_mouse_target; object_mouse_target != nullptr; object_mouse_target = object_mouse_target->get_parent()){
		if (object_mouse_target == &context){
			result_info = trigger_event_with_target_<events::mouse_move>(*object_mouse_target, *target, msg, thread_.get_app().get_class_entry(window_context->get_class_name()));
			result = result_info.second;
		}
		else//Ignore result
			result_info = trigger_event_with_target_<events::mouse_move>(*object_mouse_target, *target, mouse_msg, nullptr);

		if ((result_info.first & events::object::state_propagation_stopped) != 0u)
			break;//Propagation stopped
	}

	if (msg.message == WM_MOUSEMOVE && !mouse_enter && mouse_.dragging == nullptr && mouse_.button_down != events::mouse::button_type_nil){//Check fro drag begin
		SIZE mouse_delta{
			std::abs(mouse_position.x - mouse_.down_position.x),
			std::abs(mouse_position.y - mouse_.down_position.y)
		};

		if (mouse_.drag_threshold.cx <= mouse_delta.cx || mouse_.drag_threshold.cy <= mouse_delta.cy){//Drag begin
			mouse_msg.message = WINP_WM_MOUSEDRAGBEGIN;
			auto object_mouse_target = dynamic_cast<ui::object *>(mouse_.target), target = object_mouse_target;

			for (; object_mouse_target != nullptr; object_mouse_target = object_mouse_target->get_parent()){
				result_info = trigger_event_with_target_<events::mouse_drag_begin>(*object_mouse_target, *target, mouse_msg, nullptr);
				if ((result_info.first & events::object::state_default_prevented) == 0u && result_info.second != FALSE){
					mouse_.dragging = dynamic_cast<ui::interactive_surface *>(object_mouse_target);
					break;
				}

				if ((result_info.first & events::object::state_propagation_stopped) != 0u)
					break;//Propagation stopped
			}

			if (mouse_.dragging != nullptr){
				mouse_msg.message = WINP_WM_MOUSEDRAG;
				trigger_event_<events::mouse_drag>(*object_mouse_target, true, mouse_msg, nullptr);
			}
		}
	}
	else if (auto object_dragging = dynamic_cast<ui::object *>(mouse_.dragging); object_dragging != nullptr){
		mouse_msg.message = WINP_WM_MOUSEDRAG;
		trigger_event_<events::mouse_drag>(*object_dragging, false, mouse_msg, nullptr);
	}

	mouse_.last_position = mouse_position;
	return result;
}

LRESULT winp::thread::item_manager::mouse_down_(item &context, MSG &msg, DWORD position, unsigned int button, bool is_non_client){
	auto window_context = dynamic_cast<ui::window_surface *>(&context);
	if (window_context == nullptr)//Window surface required
		return 0;

	if (!is_non_client){
		if (mouse_.button_down == events::mouse::button_type_nil)
			mouse_.down_position = POINT{ GET_X_LPARAM(position), GET_Y_LPARAM(position) };
		mouse_.button_down |= button;
	}

	LRESULT result = 0;
	std::pair<unsigned int, LRESULT> result_info;

	MSG mouse_msg{ nullptr, WINP_WM_MOUSEDOWN };
	for (auto object_mouse_target = dynamic_cast<ui::object *>(mouse_.target), target = object_mouse_target; object_mouse_target != nullptr; object_mouse_target = object_mouse_target->get_parent()){
		if (object_mouse_target == &context){
			result_info = trigger_event_with_target_<events::mouse_down>(*object_mouse_target, *target, button, is_non_client, mouse_msg, msg, thread_.get_app().get_class_entry(window_context->get_class_name()));
			result = result_info.second;
		}
		else//Ignore result
			result_info = trigger_event_with_target_<events::mouse_down>(*object_mouse_target, *target, button, is_non_client, mouse_msg, nullptr);

		if ((result_info.first & events::object::state_propagation_stopped) != 0u)
			break;//Propagation stopped
	}

	return result;
}

LRESULT winp::thread::item_manager::mouse_up_(item &context, MSG &msg, DWORD position, unsigned int button, bool is_non_client){
	auto window_context = dynamic_cast<ui::window_surface *>(&context);
	if (window_context == nullptr)//Window surface required
		return 0;

	if (!is_non_client){
		mouse_.button_down &= ~button;
		if (mouse_.button_down == events::mouse::button_type_nil && mouse_.dragging != nullptr){
			MSG mouse_msg{ nullptr, WINP_WM_MOUSEDRAGEND };
			trigger_event_<events::mouse_drag_end>(*dynamic_cast<ui::object *>(mouse_.dragging), mouse_msg, nullptr);
			mouse_.dragging = nullptr;
		}
	}

	LRESULT result = 0;
	std::pair<unsigned int, LRESULT> result_info;

	MSG mouse_msg{ nullptr, WINP_WM_MOUSEUP };
	for (auto object_mouse_target = dynamic_cast<ui::object *>(mouse_.target), target = object_mouse_target; object_mouse_target != nullptr; object_mouse_target = object_mouse_target->get_parent()){
		if (object_mouse_target == &context){
			result_info = trigger_event_with_target_<events::mouse_up>(*object_mouse_target, *target, button, is_non_client, mouse_msg, msg, thread_.get_app().get_class_entry(window_context->get_class_name()));
			result = result_info.second;
		}
		else//Ignore result
			result_info = trigger_event_with_target_<events::mouse_up>(*object_mouse_target, *target, button, is_non_client, mouse_msg, nullptr);

		if ((result_info.first & events::object::state_propagation_stopped) != 0u)
			break;//Propagation stopped
	}

	return result;
}

LRESULT winp::thread::item_manager::mouse_dbl_clk_(item &context, MSG &msg, DWORD position, unsigned int button, bool is_non_client){
	auto window_context = dynamic_cast<ui::window_surface *>(&context);
	if (window_context == nullptr)//Window surface required
		return 0;

	LRESULT result = 0;
	std::pair<unsigned int, LRESULT> result_info;

	MSG mouse_msg{ nullptr, WINP_WM_MOUSEDBLCLK };
	for (auto object_mouse_target = dynamic_cast<ui::object *>(mouse_.target), target = object_mouse_target; object_mouse_target != nullptr; object_mouse_target = object_mouse_target->get_parent()){
		if (object_mouse_target == &context){
			result_info = trigger_event_with_target_<events::mouse_dbl_clk>(*object_mouse_target, *target, button, is_non_client, mouse_msg, msg, thread_.get_app().get_class_entry(window_context->get_class_name()));
			result = result_info.second;
		}
		else//Ignore result
			result_info = trigger_event_with_target_<events::mouse_dbl_clk>(*object_mouse_target, *target, button, is_non_client, mouse_msg, nullptr);

		if ((result_info.first & events::object::state_propagation_stopped) != 0u)
			break;//Propagation stopped
	}

	return result;
}

LRESULT winp::thread::item_manager::mouse_wheel_(item &context, MSG &msg, DWORD position){
	auto window_context = dynamic_cast<ui::window_surface *>(&context);
	if (window_context == nullptr)//Window surface required
		return 0;

	LRESULT result = 0;
	std::pair<unsigned int, LRESULT> result_info;

	for (auto object_mouse_target = dynamic_cast<ui::object *>(mouse_.target), target = object_mouse_target; object_mouse_target != nullptr; object_mouse_target = object_mouse_target->get_parent()){
		if (object_mouse_target == &context){
			result_info = trigger_event_with_target_<events::mouse_wheel>(*object_mouse_target, *target, msg, thread_.get_app().get_class_entry(window_context->get_class_name()));
			result = result_info.second;
		}
		else//Ignore result
			result_info = trigger_event_with_target_<events::mouse_wheel>(*object_mouse_target, *target, msg, nullptr);

		if ((result_info.first & events::object::state_propagation_stopped) != 0u)
			break;//Propagation stopped
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
