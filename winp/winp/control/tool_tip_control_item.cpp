#include "../app/app_object.h"

#include "tool_tip_control.h"

winp::control::tool_tip_item::tool_tip_item()
	: tool_tip_item(app::object::get_thread()){}

winp::control::tool_tip_item::tool_tip_item(thread::object &thread)
	: object(thread){}

winp::control::tool_tip_item::tool_tip_item(ui::tree &parent)
	: tool_tip_item(parent, static_cast<std::size_t>(-1)){}

winp::control::tool_tip_item::tool_tip_item(ui::tree &parent, std::size_t index)
	: tool_tip_item(parent.get_thread()){
	set_parent(&parent, index);
}

winp::control::tool_tip_item::~tool_tip_item(){
	destruct();
}

winp::utility::error_code winp::control::tool_tip_item::set_target(ui::window_surface &value, const std::function<void(tool_tip_item &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=, &value]{
		return pass_return_value_to_callback(callback, *this, set_target_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::ui::window_surface *winp::control::tool_tip_item::get_target(const std::function<void(ui::window_surface *)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, target_);
	}, (callback != nullptr), nullptr);
}

winp::utility::error_code winp::control::tool_tip_item::set_max_width(int value, const std::function<void(tool_tip_item &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_max_width_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

int winp::control::tool_tip_item::get_max_width(const std::function<void(int)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, get_max_width_());
	}, (callback != nullptr), 0);
}

winp::utility::error_code winp::control::tool_tip_item::set_title(const std::wstring &value, const std::function<void(tool_tip_item &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_title_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

const std::wstring &winp::control::tool_tip_item::get_title(const std::function<void(const std::wstring &)> &callback) const{
	return *compute_or_post_task_inside_thread_context([=]{
		return &pass_return_ref_value_to_callback(callback, &title_);
	}, (callback != nullptr), &title_);
}

winp::utility::error_code winp::control::tool_tip_item::set_text(const std::wstring &value, const std::function<void(tool_tip_item &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_text_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

const std::wstring &winp::control::tool_tip_item::get_text(const std::function<void(const std::wstring &)> &callback) const{
	return *compute_or_post_task_inside_thread_context([=]{
		return &pass_return_ref_value_to_callback(callback, &text_);
	}, (callback != nullptr), &text_);
}

winp::utility::error_code winp::control::tool_tip_item::set_font(HFONT value, const std::function<void(tool_tip_item &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_font_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

HFONT winp::control::tool_tip_item::get_font(const std::function<void(HFONT)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, get_font_());
	}, (callback != nullptr), nullptr);
}

winp::utility::error_code winp::control::tool_tip_item::create_(){
	if (handle_ != nullptr)
		return utility::error_code::nil;

	if (is_destructed_)
		return utility::error_code::object_destructed;

	auto tool_tip_parent = dynamic_cast<tool_tip *>(parent_);
	if (tool_tip_parent == nullptr || !tool_tip_parent->is_created())
		return utility::error_code::parent_not_created;

	if (!target_->is_created())
		return utility::error_code::tool_tip_target_not_created;

	auto target_handle = target_->get_handle();
	auto target_rect = get_dimension_();

	UINT flags = TTF_SUBCLASS;
	if (IsRectEmpty(&target_rect) != FALSE){
		flags |= TTF_IDISHWND;
		local_id_ = reinterpret_cast<UINT_PTR>(target_handle);
	}
	else//Rectangular tip
		local_id_ = static_cast<UINT_PTR>(id_);

	TTTOOLINFOW info{
		sizeof(TTTOOLINFOW),
		flags,
		target_handle,
		local_id_,
		target_rect,
		nullptr,
		LPSTR_TEXTCALLBACKW,
		reinterpret_cast<LPARAM>(this)
	};

	auto tool_tip_handle = tool_tip_parent->get_handle_();
	if (SendMessageW(tool_tip_handle, TTM_ADDTOOLW, 0, reinterpret_cast<LPARAM>(&info)) == FALSE)
		return utility::error_code::action_could_not_be_completed;

	handle_ = tool_tip_handle;
	if (thread_.send_message(*this, WM_NCCREATE) == FALSE){
		destroy_();
		return utility::error_code::action_prevented;
	}

	thread_.send_message(*this, WM_CREATE);
	return utility::error_code::nil;
}

winp::utility::error_code winp::control::tool_tip_item::destroy_(){
	if (handle_ == nullptr)
		return utility::error_code::nil;

	TTTOOLINFOW info{
		sizeof(TTTOOLINFOW),
		((local_id_ == id_) ? 0u : TTF_IDISHWND),
		handle_,
		local_id_
	};

	SendMessageW(handle_, TTM_DELTOOLW, 0, reinterpret_cast<LPARAM>(&info));
	handle_ = nullptr;
	thread_.send_message(*this, WM_NCDESTROY);

	return utility::error_code::nil;
}

bool winp::control::tool_tip_item::is_created_() const{
	return (handle_ != nullptr);
}

winp::utility::error_code winp::control::tool_tip_item::set_parent_value_(ui::tree *value, bool changing){
	if (changing && value != nullptr && dynamic_cast<tool_tip *>(value) == nullptr)
		return utility::error_code::invalid_parent;
	return object::set_parent_value_(value, changing);
}

winp::utility::error_code winp::control::tool_tip_item::set_dimension_(int x, int y, int width, int height, UINT flags, bool allow_animation){
	if (auto error_code = ui::surface::set_dimension_(x, y, width, height, flags, false); error_code != utility::error_code::nil)
		return error_code;

	if (handle_ == nullptr)
		return utility::error_code::nil;

	if (local_id_ != id_){
		position_ = POINT{};
		size_ = SIZE{};
		return utility::error_code::nil;
	}

	TTTOOLINFOW info{
		sizeof(TTTOOLINFOW),
		0u,
		handle_,
		local_id_,
		get_dimension_(),
	};

	SendMessageW(handle_, TTM_NEWTOOLRECTW, 0, reinterpret_cast<LPARAM>(&info));
	return utility::error_code::nil;
}

winp::utility::error_code winp::control::tool_tip_item::set_target_(ui::window_surface &value){
	destroy_();
	target_ = &value;
	return utility::error_code::nil;
}

winp::utility::error_code winp::control::tool_tip_item::set_max_width_(int value){
	max_width_ = value;
	return utility::error_code::nil;
}

int winp::control::tool_tip_item::get_max_width_() const{
	if (0 < max_width_)
		return max_width_;

	if (auto tool_tip_parent = dynamic_cast<tool_tip *>(parent_); tool_tip_parent != nullptr)
		return tool_tip_parent->max_width_;

	return 0;
}

winp::utility::error_code winp::control::tool_tip_item::set_title_(const std::wstring &value){
	title_ = value;
	return utility::error_code::nil;
}

winp::utility::error_code winp::control::tool_tip_item::set_text_(const std::wstring &value){
	text_ = value;
	return utility::error_code::nil;
}

winp::utility::error_code winp::control::tool_tip_item::set_font_(HFONT value){
	font_ = value;
	return utility::error_code::nil;
}

HFONT winp::control::tool_tip_item::get_font_() const{
	if (font_ != nullptr)
		return font_;

	if (auto tool_tip_parent = dynamic_cast<tool_tip *>(parent_); tool_tip_parent != nullptr)
		return tool_tip_parent->font_;

	return nullptr;
}

LRESULT winp::control::tool_tip_item::need_text_(NMTTDISPINFOW &info) const{
	std::wstring text;
	auto result = trigger_event_<events::get_text>(text);
	if ((result.first & events::object::state_default_prevented) != 0u || (text.empty() && text_.empty()))
		return 0;//Do nothing

	if (text.empty())
		info.lpszText = const_cast<wchar_t *>(text_.data());
	else
		info.lpszText = const_cast<wchar_t *>(text.data());

	auto max_width = get_max_width_();
	SendMessageW(handle_, TTM_SETMAXTIPWIDTH, 0, ((0 < max_width) ? max_width : -1));

	result = trigger_event_<events::get_font>();
	auto font = get_font_();

	if ((result.first & events::object::state_default_prevented) == 0u && (result.second != 0 || font != nullptr))
		SendMessageW(handle_, WM_SETFONT, ((result.second == 0) ? reinterpret_cast<WPARAM>(font) : static_cast<WPARAM>(result.second)), FALSE);
	else
		SendMessageW(handle_, WM_SETFONT, 0, FALSE);

	std::wstring title;
	result = trigger_event_<events::get_title>(title);

	if ((result.first & events::object::state_default_prevented) == 0u && (!title.empty() || !title_.empty())){
		result = trigger_event_<events::get_image>();
		if ((result.first & events::object::state_default_prevented) == 0u && (result.second != 0 || image_ != nullptr))
			SendMessageW(handle_, TTM_SETTITLEW, ((result.second == 0) ? reinterpret_cast<WPARAM>(image_) : static_cast<WPARAM>(result.second)), reinterpret_cast<LPARAM>(title.empty() ? title_.data() : title.data()));
		else
			SendMessageW(handle_, TTM_SETTITLEW, TTI_NONE, reinterpret_cast<LPARAM>(title.empty() ? title_.data() : title.data()));
	}
	else{//No title
		title = L"";
		SendMessageW(handle_, TTM_SETTITLEW, TTI_NONE, reinterpret_cast<LPARAM>(title.data()));
	}

	return 0;
}

LRESULT winp::control::tool_tip_item::showing_() const{
	MSG msg{ nullptr, WINP_WM_TOOLTIP_SHOW };
	return trigger_event_<events::show>(msg, nullptr).second;
}

LRESULT winp::control::tool_tip_item::popping_() const{
	MSG msg{ nullptr, WINP_WM_TOOLTIP_HIDE };
	return trigger_event_<events::hide>(msg, nullptr).second;
}

LRESULT winp::control::tool_tip_item::link_clicked_() const{
	return trigger_event_<events::link_clicked>().second;
}

winp::ui::window_surface *winp::control::tool_tip_item::get_target_window_ancestor_(POINT &offset) const{
	offset = POINT{};
	return dynamic_cast<ui::object *>(target_)->get_first_ancestor_of<ui::window_surface>([&](ui::tree &ancestor){
		if (!ancestor.is_created())
			return false;

		if (auto surface_ancestor = dynamic_cast<surface *>(&ancestor); surface_ancestor != nullptr){
			auto ancestor_position = surface_ancestor->get_position();
			offset.x += ancestor_position.x;
			offset.y += ancestor_position.y;
		}

		return true;
	});
}

winp::control::inplace_tool_tip_item::inplace_tool_tip_item()
	: inplace_tool_tip_item(app::object::get_thread()){}

winp::control::inplace_tool_tip_item::inplace_tool_tip_item(thread::object &thread)
	: tool_tip_item(thread){
	add_event_handler_([this](events::show &e){
		if (handle_ == nullptr || target_ == nullptr)
			return;

		RECT target_rect{};
		if (local_id_ == id_)
			target_rect = target_->convert_dimension_to_absolute_value(get_dimension_());
		else
			target_rect = target_->get_absolute_dimension();

		SendMessageW(handle_, TTM_ADJUSTRECT, TRUE, reinterpret_cast<LPARAM>(&target_rect));
		dynamic_cast<tool_tip *>(parent_)->set_absolute_position(target_rect.left, target_rect.top);

		e.set_result_if_not_set(TRUE);
	});
}

winp::control::inplace_tool_tip_item::inplace_tool_tip_item(ui::tree &parent)
	: inplace_tool_tip_item(parent, static_cast<std::size_t>(-1)){}

winp::control::inplace_tool_tip_item::inplace_tool_tip_item(ui::tree &parent, std::size_t index)
	: inplace_tool_tip_item(parent.get_thread()){
	set_parent(&parent, index);
}

winp::control::inplace_tool_tip_item::~inplace_tool_tip_item() = default;