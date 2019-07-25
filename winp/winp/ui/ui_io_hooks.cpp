#include "../app/app_object.h"

#include "../menu/menu_action_item.h"
#include "../menu/menu_separator.h"

#include "ui_io_hooks.h"
#include "ui_window_surface.h"

winp::ui::io_hook::io_hook(object &target)
	: hook(target){}

winp::ui::io_hook::~io_hook() = default;

winp::ui::drag_hook::drag_hook(object &target)
	: hook(target){
	target_.insert_hook<io_hook>();
	target_.insert_hook<no_drag_position_updated_hook>();

	target_.events().bind([](events::mouse_drag_begin &e){
		return true;
	}, this);

	target_.events().bind([this](events::mouse_drag &e){
		if (auto surface_target = dynamic_cast<surface *>(&target_); surface_target != nullptr){
			auto offset = e.get_offset();
			surface_target->set_position_((surface_target->position_.x + offset.x), (surface_target->position_.y + offset.y), false);
		}
	}, this);
}

winp::ui::drag_hook::~drag_hook() = default;

winp::ui::no_drag_position_updated_hook::no_drag_position_updated_hook(object &target)
	: hook(target){
	target_.events().bind([this](events::mouse_drag &e){
		is_dragging_ = true;
	}, this);

	target_.events().bind([this](events::mouse_drag_end &e){
		is_dragging_ = false;
		trigger_event_of_<events::non_drag_position_updated>(target_, SWP_NOSIZE);
	}, this);

	target_.events().bind([this](events::position_updated &e){
		if (!is_dragging_)
			trigger_event_of_<events::non_drag_position_updated>(target_, e.get_flags());
		else if ((e.get_flags() & SWP_NOSIZE) == 0u)//Size updated
			trigger_event_of_<events::non_drag_position_updated>(target_, SWP_NOMOVE);
	}, this);
}

winp::ui::no_drag_position_updated_hook::~no_drag_position_updated_hook() = default;

winp::ui::mouse_hover_hook::mouse_hover_hook(object &target, const std::chrono::milliseconds &delay)
	: hook(target), delay_(delay){
	target_.insert_hook<io_hook>();
	target_.events().bind([this](events::mouse_move &e){
		remove_hover_();
		if (!e.is_non_client())
			bind_timer_();
	}, this);

	target_.events().bind([this](events::mouse_leave &e){
		remove_hover_();
	}, this);

	target_.events().bind([this](events::mouse_down &e){
		remove_hover_();
		if (!e.is_non_client())
			bind_timer_();
	}, this);

	target_.events().bind([this](events::mouse_up &e){
		remove_hover_();
		if (!e.is_non_client())
			bind_timer_();
	}, this);

	target_.events().bind([this](events::mouse_wheel &e){
		remove_hover_();
		bind_timer_();
	}, this);
}

winp::ui::mouse_hover_hook::~mouse_hover_hook() = default;

winp::utility::error_code winp::ui::mouse_hover_hook::set_delay(const std::chrono::milliseconds &value, const std::function<void(mouse_hover_hook &, utility::error_code)> &callback){
	return target_.compute_or_post_task_inside_thread_context([=]{
		return target_.pass_return_value_to_callback(callback, *this, set_delay_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

const std::chrono::milliseconds &winp::ui::mouse_hover_hook::get_delay(const std::function<void(const std::chrono::milliseconds &)> &callback) const{
	return *target_.compute_or_post_task_inside_thread_context([=]{
		return &target_.pass_return_ref_value_to_callback(callback, &delay_);
	}, (callback != nullptr), &delay_);
}

winp::utility::error_code winp::ui::mouse_hover_hook::set_delay_(const std::chrono::milliseconds &value){
	delay_ = value;
	return utility::error_code::nil;
}

void winp::ui::mouse_hover_hook::bind_timer_(){
	auto state = ++state_;
	unbind_outbound_events_<events::timer>(&target_);

	target_.events().bind([=](events::timer &e){
		if (state_ != state){
			e.prevent_default();
			return 0ll;
		}

		if (e.needs_duration())
			return delay_.count();

		if (!is_hovered_)
			target_.trigger_event_<events::mouse_hover>(is_hovered_ = true);

		return 0ll;
	}, this);
}

void winp::ui::mouse_hover_hook::remove_hover_(){
	++state_;
	if (is_hovered_)
		target_.trigger_event_<events::mouse_hover>(is_hovered_ = false);
}

winp::ui::auto_hide_cursor_hook::auto_hide_cursor_hook(object &target, const std::chrono::milliseconds &delay)
	: hook(target), delay_(delay){
	if (auto hk = target_.insert_hook<mouse_hover_hook>(); hk != nullptr)
		hk->set_delay(delay_);

	target_.events().bind([this](events::mouse_hover &e){
		if (e.is_hovered() && !is_hidden_){
			ShowCursor(FALSE);
			is_hidden_ = true;
		}
		else if (!e.is_hovered() && is_hidden_){
			ShowCursor(TRUE);
			is_hidden_ = false;
		}
	}, this);
}

winp::ui::auto_hide_cursor_hook::~auto_hide_cursor_hook(){
	if (is_hidden_){
		ShowCursor(TRUE);
		is_hidden_ = false;
	}
}

winp::utility::error_code winp::ui::auto_hide_cursor_hook::set_delay(const std::chrono::milliseconds &value, const std::function<void(auto_hide_cursor_hook &, utility::error_code)> &callback){
	return target_.compute_or_post_task_inside_thread_context([=]{
		return target_.pass_return_value_to_callback(callback, *this, set_delay_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

const std::chrono::milliseconds &winp::ui::auto_hide_cursor_hook::get_delay(const std::function<void(const std::chrono::milliseconds &)> &callback) const{
	return *target_.compute_or_post_task_inside_thread_context([=]{
		return &target_.pass_return_ref_value_to_callback(callback, &delay_);
	}, (callback != nullptr), &delay_);
}

winp::utility::error_code winp::ui::auto_hide_cursor_hook::set_delay_(const std::chrono::milliseconds &value){
	delay_ = value;
	if (auto hk = target_.find_hook<mouse_hover_hook>(); hk != nullptr)
		hk->set_delay(delay_);

	return utility::error_code::nil;
}

winp::ui::fullscreen_hook::fullscreen_hook(object &target)
	: hook(target){
	target_.insert_hook<io_hook>();
	if (dynamic_cast<window_surface *>(&target_) != nullptr){
		target_.events().bind([this](events::modify_context_menu &e){
			if (!e.get_popup().is_system())
				return;

			if (auto window_target = dynamic_cast<window_surface *>(&target_); window_target != nullptr && window_target->is_minimized())
				return;

			if (is_fullscreen_){
				e.get_popup().add_object([this](menu::separator &item){});

				e.get_popup().add_object([this](menu::action_item &item){
					item.set_text(L"Exit Full Screen");
					item.events().bind([this](events::menu_item_select &e){
						escape_fullscreen_();
					});
				});
			}
			else{//Normal window
				e.get_popup().add_object([this](menu::separator &item){});

				e.get_popup().add_object([this](menu::action_item &item){
					item.set_text(L"Enter Full Screen");
					item.events().bind([this](events::menu_item_select &e){
						enter_fullscreen_();
					});
				});
			}
		}, this);

		target_.events().bind([this](events::mouse_dbl_clk &e){
			toggle_fullscreen_();
		}, this);

		target_.events().bind([this](events::key_down &e){
			switch (e.get_virtual_code()){
			case VK_ESCAPE:
				escape_fullscreen_();
				break;
			case 'F':
				toggle_fullscreen_();
				break;
			default:
				break;
			}
		}, this);

		target_.events().bind([this](events::key_up &e){
			switch (e.get_virtual_code()){
			case VK_ESCAPE:
				escape_fullscreen_();
				break;
			default:
				break;
			}
		}, this);
	}
}

winp::ui::fullscreen_hook::~fullscreen_hook(){
	escape_fullscreen_();
}

void winp::ui::fullscreen_hook::enter_fullscreen_(){
	auto window_target = dynamic_cast<window_surface *>(&target_);
	if (is_fullscreen_ || window_target == nullptr || !window_target->is_created())
		return;

	if ((saved_.is_maximized = window_target->is_maximized()))
		window_target->toggle_maximized();

	saved_.dimension = window_target->get_dimension();
	saved_.styles = window_target->get_styles(false);
	saved_.extended_styles = window_target->get_styles(true);

	auto hmon = MonitorFromWindow(window_target->get_handle(), MONITOR_DEFAULTTONEAREST);
	MONITORINFO info{ sizeof(MONITORINFO) };

	if (GetMonitorInfoW(hmon, &info) == FALSE)
		return;

	window_target->set_styles((WS_POPUP | WS_VISIBLE), false);
	window_target->set_styles(0, true);

	window_target->set_absolute_position(info.rcMonitor.left, info.rcMonitor.top);
	window_target->set_size((info.rcMonitor.right - info.rcMonitor.left), (info.rcMonitor.bottom - info.rcMonitor.top));

	is_fullscreen_ = true;
}

void winp::ui::fullscreen_hook::escape_fullscreen_(){
	auto window_target = dynamic_cast<window_surface *>(&target_);
	if (!is_fullscreen_ || window_target == nullptr)
		return;

	window_target->set_styles((saved_.styles | WS_VISIBLE), false);
	window_target->set_styles(saved_.extended_styles, true);

	window_target->set_position(saved_.dimension.left, saved_.dimension.top);
	window_target->set_size((saved_.dimension.right - saved_.dimension.left), (saved_.dimension.bottom - saved_.dimension.top));

	if (saved_.is_maximized)
		window_target->maximize();

	is_fullscreen_ = false;
}

void winp::ui::fullscreen_hook::toggle_fullscreen_(){
	if (is_fullscreen_)
		escape_fullscreen_();
	else
		enter_fullscreen_();
}

winp::ui::system_menu_as_context_menu::system_menu_as_context_menu(object &target)
	: hook(target){
	target_.events().bind([this](events::context_menu &e) -> menu::popup *{
		if (auto window_target = dynamic_cast<window_surface *>(&target_); window_target != nullptr)
			return &window_target->get_system_menu();
		return nullptr;
	}, this);
}

winp::ui::system_menu_as_context_menu::~system_menu_as_context_menu() = default;
