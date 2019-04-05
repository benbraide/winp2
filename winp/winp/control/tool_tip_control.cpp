#include "../app/app_collection.h"

#include "tool_tip_control.h"

winp::control::tool_tip::tool_tip()
	: tool_tip(app::collection::get_main()->get_thread()){}

winp::control::tool_tip::tool_tip(thread::object &thread)
	: object(thread, TOOLTIPS_CLASSW, ICC_TAB_CLASSES){}

winp::control::tool_tip::tool_tip(tree &parent)
	: tool_tip(parent, static_cast<std::size_t>(-1)){}

winp::control::tool_tip::tool_tip(tree &parent, std::size_t index)
	: tool_tip(parent.get_thread()){
	set_parent(&parent, index);
}

winp::control::tool_tip::~tool_tip() = default;

winp::utility::error_code winp::control::tool_tip::set_max_width(int value, const std::function<void(tool_tip &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_max_width_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

int winp::control::tool_tip::get_max_width(const std::function<void(int)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, max_width_);
	}, (callback != nullptr), 0);
}

winp::utility::error_code winp::control::tool_tip::set_font(HFONT value, const std::function<void(tool_tip &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_font_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

HFONT winp::control::tool_tip::get_font(const std::function<void(HFONT)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, font_);
	}, (callback != nullptr), nullptr);
}

DWORD winp::control::tool_tip::get_persistent_styles_(bool is_extended) const{
	return (object::get_persistent_styles_(is_extended) | (is_extended ? WS_EX_TOOLWINDOW : (WS_POPUP | TTS_USEVISUALSTYLE)));
}

bool winp::control::tool_tip::is_top_level_() const{
	return false;
}

LRESULT winp::control::tool_tip::dispatch_notification_(MSG &msg) const{
	switch (reinterpret_cast<NMHDR *>(msg.lParam)->code){
	case TTN_NEEDTEXT:
	{
		auto info = reinterpret_cast<NMTTDISPINFOW *>(msg.lParam);
		if (auto target = reinterpret_cast<tool_tip_item *>(info->lParam); target != nullptr)
			target->need_text_(*info);
		break;
	}
	case TTN_SHOW:
	{
		if (auto current_item = get_current_item_(); current_item != nullptr)
			return (current_item->showing_() ? TRUE : FALSE);
		break;
	}
	case TTN_POP:
	{
		if (auto current_item = get_current_item_(); current_item != nullptr)
			current_item->popping_();
		break;
	}
	case TTN_LINKCLICK:
	{
		if (auto current_item = get_current_item_(); current_item != nullptr)
			current_item->link_clicked_();
		break;
	}
	default:
		break;
	}

	return 0;
}

winp::utility::error_code winp::control::tool_tip::set_max_width_(int value){
	max_width_ = value;
	return utility::error_code::nil;
}

winp::utility::error_code winp::control::tool_tip::set_font_(HFONT value){
	font_ = value;
	return utility::error_code::nil;
}

winp::control::tool_tip_item *winp::control::tool_tip::get_current_item_() const{
	if (handle_ == nullptr)
		return nullptr;

	TTTOOLINFOW info{
		sizeof(TTTOOLINFOW)
	};

	if (SendMessageW(handle_, TTM_GETCURRENTTOOL, 0, reinterpret_cast<LPARAM>(&info)) == FALSE)
		return nullptr;

	return reinterpret_cast<tool_tip_item *>(info.lParam);
}
