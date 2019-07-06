#include "../app/app_object.h"

#include "tool_tip_control.h"

winp::control::tool_tip_item::tool_tip_item() = default;

winp::control::tool_tip_item::tool_tip_item(ui::tree &parent)
	: tool_tip_item(parent, static_cast<std::size_t>(-1)){}

winp::control::tool_tip_item::tool_tip_item(ui::tree &parent, std::size_t index){
	set_parent(&parent, index);
}

winp::control::tool_tip_item::~tool_tip_item(){
	destruct_();
}

winp::utility::error_code winp::control::tool_tip_item::set_target(ui::object &value, const std::function<void(tool_tip_item &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=, &value]{
		return pass_return_value_to_callback(callback, *this, set_target_(value));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::ui::object *winp::control::tool_tip_item::get_target(const std::function<void(ui::object *)> &callback) const{
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

	if (is_destructed_())
		return utility::error_code::object_destructed;

	auto tool_tip_parent = dynamic_cast<tool_tip *>(parent_);
	if (tool_tip_parent == nullptr)
		return utility::error_code::parent_not_created;

	tool_tip_parent->auto_create();
	if (!tool_tip_parent->is_created())
		return utility::error_code::parent_not_created;

	if (computed_target_ == nullptr)
		return utility::error_code::tool_tip_target_not_created;

	computed_target_->auto_create();
	auto computed_handle = computed_target_->get_handle();

	if (computed_handle == nullptr)
		return utility::error_code::tool_tip_target_not_created;

	UINT flags = TTF_SUBCLASS;
	if (computed_target_ == target_ && IsRectEmpty(&initial_dimension_) != FALSE){
		flags |= TTF_IDISHWND;
		local_id_ = reinterpret_cast<UINT_PTR>(computed_handle);
	}
	else//Rectangular tip
		local_id_ = static_cast<UINT_PTR>(id_);

	TTTOOLINFOW info{
		sizeof(TTTOOLINFOW),
		flags,
		computed_handle,
		local_id_,
		computed_dimension_,
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
	if (local_id_ != 0u && local_id_ != id_)//Tool covers entire target's surface
		return utility::error_code::not_supported;

	if (auto error_code = ui::surface::set_dimension_(x, y, width, height, flags, false); error_code != utility::error_code::nil)
		return error_code;

	compute_values_(false);
	auto computed_handle = ((computed_target_ == nullptr) ? nullptr : computed_target_->get_handle());
	if (computed_handle == nullptr)
		return utility::error_code::nil;

	TTTOOLINFOW info{
		sizeof(TTTOOLINFOW),
		0u,
		computed_handle,
		local_id_,
		computed_dimension_,
	};

	SendMessageW(handle_, TTM_NEWTOOLRECTW, 0, reinterpret_cast<LPARAM>(&info));
	return utility::error_code::nil;
}

winp::utility::error_code winp::control::tool_tip_item::set_target_(ui::object &value){
	if (&value == target_)//No changes
		return utility::error_code::nil;

	destroy_();
	(target_ = &value)->insert_hook<ui::no_drag_position_updated_hook>();
	compute_values_(true);

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
	auto ancestor = target_->get_first_ancestor_of<ui::window_surface>([&](ui::tree &ancestor){
		if (!ancestor.is_created())
			return false;

		if (auto surface_ancestor = dynamic_cast<surface *>(&ancestor); surface_ancestor != nullptr){
			auto &ancestor_position = surface_ancestor->get_current_position();
			auto ancestor_client_start_offset = surface_ancestor->get_client_start_offset();

			offset.x += (ancestor_position.x + ancestor_client_start_offset.x);
			offset.y += (ancestor_position.y + ancestor_client_start_offset.y);
		}

		return true;
	});

	if (ancestor != nullptr){
		auto ancestor_client_start_offset = ancestor->get_client_start_offset();
		offset.x += ancestor_client_start_offset.x;
		offset.y += ancestor_client_start_offset.y;
	}

	return ancestor;
}

void winp::control::tool_tip_item::compute_values_(bool target_changed){
	if (target_changed && target_ != nullptr)
		unbind_outbound_events_<events::non_drag_position_updated>(target_);

	initial_dimension_ = get_dimension_();
	if ((computed_target_ = dynamic_cast<ui::window_surface *>(target_)) != nullptr){
		computed_dimension_ = initial_dimension_;
		return;
	}

	auto surface_target = dynamic_cast<ui::surface *>(target_);
	if (surface_target == nullptr)
		return;

	computed_offset_ = POINT{};
	if ((computed_target_ = get_target_window_ancestor_(computed_offset_)) == nullptr)
		return;

	if (IsRectEmpty(&initial_dimension_) == FALSE){
		auto &position = surface_target->get_current_position();
		computed_dimension_ = initial_dimension_;
		OffsetRect(&computed_dimension_, position.x, position.y);
	}
	else//Use target's area
		computed_dimension_ = surface_target->get_current_dimension();

	OffsetRect(&computed_dimension_, computed_offset_.x, computed_offset_.y);//Move relative to window ancestor
	if (!target_changed)
		return;

	target_->events().bind([this](events::non_drag_position_updated &e){
		if (handle_ == nullptr || IsRectEmpty(&initial_dimension_) == FALSE || (e.get_flags() & (SWP_NOMOVE | SWP_NOSIZE)) == (SWP_NOMOVE | SWP_NOSIZE))
			return;//No dimension update

		auto computed_handle = ((computed_target_ == nullptr) ? nullptr : computed_target_->get_handle());
		if (computed_handle == nullptr)
			return;

		auto surface_target = dynamic_cast<ui::surface *>(target_);
		if (surface_target == nullptr)
			return;

		if (IsRectEmpty(&initial_dimension_) == FALSE){
			auto &position = surface_target->get_current_position();
			computed_dimension_ = initial_dimension_;
			OffsetRect(&computed_dimension_, position.x, position.y);
		}
		else//Use target's area
			computed_dimension_ = surface_target->get_current_dimension();

		OffsetRect(&computed_dimension_, computed_offset_.x, computed_offset_.y);//Move relative to window ancestor
		TTTOOLINFOW info{
			sizeof(TTTOOLINFOW),
			0u,
			computed_handle,
			local_id_,
			computed_dimension_,
		};

		SendMessageW(handle_, TTM_NEWTOOLRECTW, 0, reinterpret_cast<LPARAM>(&info));
	}, this);
}

winp::control::inplace_tool_tip_item::inplace_tool_tip_item(){
	add_event_handler_([this](events::show &e){
		if (handle_ == nullptr || target_ == nullptr)
			return;

		auto surface_target = dynamic_cast<ui::surface *>(target_);
		if (surface_target == nullptr)
			return;

		RECT target_rect{};
		if (local_id_ == id_)//Rectangular tip
			target_rect = surface_target->convert_dimension_to_absolute_value(get_dimension_());
		else
			target_rect = surface_target->get_absolute_dimension();

		SendMessageW(handle_, TTM_ADJUSTRECT, TRUE, reinterpret_cast<LPARAM>(&target_rect));
		dynamic_cast<tool_tip *>(parent_)->set_absolute_position(target_rect.left, target_rect.top);

		e.set_result_if_not_set(TRUE);
	});
}

winp::control::inplace_tool_tip_item::inplace_tool_tip_item(ui::tree &parent)
	: inplace_tool_tip_item(parent, static_cast<std::size_t>(-1)){}

winp::control::inplace_tool_tip_item::inplace_tool_tip_item(ui::tree &parent, std::size_t index)
	: inplace_tool_tip_item(){
	set_parent(&parent, index);
}

winp::control::inplace_tool_tip_item::~inplace_tool_tip_item() = default;