#include "../app/app_object.h"
#include "../ui/ui_io_hooks.h"

#include "ui_window_surface.h"

winp::ui::window_surface::window_surface()
	: system_menu_(*this), menu_bar_(*this){
	styles_ = (WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	background_color_ = convert_colorref_to_colorf(GetSysColor(COLOR_WINDOW), 255);
	insert_hook_<io_hook>();

	add_event_handler_([this](events::get_context_menu_position &e){
		auto position = get_context_menu_position_();
		e.set_result_if_not_set(MAKELONG(position.x, position.y));
	});
}

winp::ui::window_surface::window_surface(tree &parent)
	: window_surface(parent, static_cast<std::size_t>(-1)){}

winp::ui::window_surface::window_surface(tree &parent, std::size_t index)
	: window_surface(){
	set_parent(&parent, index);
}

winp::ui::window_surface::~window_surface(){
	destruct_();
}

winp::utility::error_code winp::ui::window_surface::show(int how, const std::function<void(window_surface &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, show_(how));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::window_surface::maximize(const std::function<void(window_surface &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, maximize_());
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::window_surface::toggle_maximized(const std::function<void(window_surface &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, (is_maximized_() ? restore_maximized_() : maximize_()));
	}, (callback != nullptr), utility::error_code::nil);
}

bool winp::ui::window_surface::is_maximized(const std::function<void(bool)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, is_maximized_());
	}, (callback != nullptr), false);
}

winp::utility::error_code winp::ui::window_surface::minimize(const std::function<void(window_surface &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, minimize_());
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::window_surface::toggle_minimized(const std::function<void(window_surface &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, (is_minimized_() ? restore_minimized_() : minimize_()));
	}, (callback != nullptr), utility::error_code::nil);
}

bool winp::ui::window_surface::is_minimized(const std::function<void(bool)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, is_minimized_());
	}, (callback != nullptr), false);
}

winp::utility::error_code winp::ui::window_surface::set_styles(DWORD value, bool is_extended, const std::function<void(window_surface &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_styles_(value, is_extended));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::window_surface::add_styles(DWORD value, bool is_extended, const std::function<void(window_surface &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_styles_(((is_extended ? extended_styles_ : styles_) | value), is_extended));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::utility::error_code winp::ui::window_surface::remove_styles(DWORD value, bool is_extended, const std::function<void(window_surface &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_styles_(((is_extended ? extended_styles_ : styles_) & ~value), is_extended));
	}, (callback != nullptr), utility::error_code::nil);
}

DWORD winp::ui::window_surface::get_styles(bool is_extended, const std::function<void(DWORD)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, get_styles_(is_extended));
	}, (callback != nullptr), 0);
}

bool winp::ui::window_surface::has_styles(DWORD value, bool is_extended, bool has_all, const std::function<void(bool)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, has_styles_(value, is_extended, has_all));
	}, (callback != nullptr), false);
}

HWND winp::ui::window_surface::get_handle(const std::function<void(HWND)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, get_handle_());
	}, (callback != nullptr), nullptr);
}

winp::ui::window_surface::popup_menu_type &winp::ui::window_surface::get_system_menu(const std::function<void(popup_menu_type &)> &callback) const{
	return *compute_or_post_task_inside_thread_context([=]{
		return &pass_return_ref_value_to_callback(callback, &system_menu_);
	}, (callback != nullptr), &system_menu_);
}

winp::menu::bar &winp::ui::window_surface::get_menu_bar(const std::function<void(menu::bar &)> &callback) const{
	return *compute_or_post_task_inside_thread_context([=]{
		return &pass_return_ref_value_to_callback(callback, &menu_bar_);
	}, (callback != nullptr), &menu_bar_);
}

const std::wstring &winp::ui::window_surface::get_class_name(const std::function<void(const std::wstring &)> &callback) const{
	return *compute_or_post_task_inside_thread_context([=]{
		return &pass_return_ref_value_to_callback(callback, &get_class_name_());
	}, (callback != nullptr), &app::object::get_class_name());
}

const wchar_t *winp::ui::window_surface::get_theme_id(const std::function<void(const wchar_t *)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, get_theme_id_());
	}, (callback != nullptr), nullptr);
}

void winp::ui::window_surface::traverse_child_windows(const std::function<bool(window_surface &)> &callback, bool block) const{
	execute_or_post_task_inside_thread_context([&]{
		traverse_child_windows_(*this, callback);
	}, !block);
}

void winp::ui::window_surface::traverse_all_child_windows(const std::function<void(window_surface &)> &callback, bool block) const{
	traverse_child_windows([callback](window_surface &value){
		callback(value);
		return true;
	}, block);
}

winp::utility::error_code winp::ui::window_surface::create_(){
	if (handle_ != nullptr)
		return utility::error_code::nil;

	if (is_destructed_())
		return utility::error_code::object_destructed;

	auto window_ancestor_and_offset = get_first_ancestor_and_relative_offset_<ui::window_surface>();
	if (window_ancestor_and_offset.first != nullptr){
		window_ancestor_and_offset.first->auto_create();
		if (!window_ancestor_and_offset.first->is_created())
			return utility::error_code::parent_not_created;
	}

	auto ancestor_handle = ((window_ancestor_and_offset.first == nullptr) ? nullptr : window_ancestor_and_offset.first->handle_);
	if (parent_ != nullptr && ancestor_handle == nullptr)
		return utility::error_code::parent_not_created;

	handle_ = thread_.get_item_manager().create_window(
		*this,
		get_styles_(true),
		get_class_name_().data(),
		get_window_text_(),
		get_styles_(false),
		(window_ancestor_and_offset.second.x + position_.x),
		(window_ancestor_and_offset.second.y + position_.y),
		size_.cx,
		size_.cy,
		ancestor_handle,
		nullptr,
		get_instance_()
	);

	if (handle_ == nullptr)
		return utility::error_code::action_could_not_be_completed;

	RECT window_rect{};
	if (GetWindowRect(handle_, &window_rect) != FALSE){
		if (size_.cx < (window_rect.right - window_rect.left))
			size_.cx = (window_rect.right - window_rect.left);

		if (size_.cy < (window_rect.bottom - window_rect.top))
			size_.cy = (window_rect.bottom - window_rect.top);
	}

	system_menu_.create();
	return utility::error_code::nil;
}

winp::utility::error_code winp::ui::window_surface::destroy_(){
	if (handle_ == nullptr)
		return utility::error_code::nil;

	if (DestroyWindow(handle_) == FALSE || handle_ != nullptr)
		return utility::error_code::action_could_not_be_completed;

	system_menu_.destroy();
	return utility::error_code::nil;
}

bool winp::ui::window_surface::is_created_() const{
	return (handle_ != nullptr);
}

HTHEME winp::ui::window_surface::get_theme_() const{
	return OpenThemeData(handle_, get_theme_id_());
}

std::pair<HDC, HWND> winp::ui::window_surface::get_device_context_() const{
	return std::make_pair(GetDC(handle_), handle_);
}

winp::utility::error_code winp::ui::window_surface::redraw_() const{
	if (handle_ == nullptr)
		return utility::error_code::object_not_created;
	return ((InvalidateRect(handle_, nullptr, TRUE) == FALSE) ? utility::error_code::action_could_not_be_completed : utility::error_code::nil);
}

winp::utility::error_code winp::ui::window_surface::redraw_(const RECT &region) const{
	if (handle_ == nullptr)
		return utility::error_code::object_not_created;
	return ((InvalidateRect(handle_, &region, TRUE) == FALSE) ? utility::error_code::action_could_not_be_completed : utility::error_code::nil);
}

winp::utility::error_code winp::ui::window_surface::show_(){
	return show_(SW_SHOW);
}

winp::utility::error_code winp::ui::window_surface::hide_(){
	return show_(SW_HIDE);
}

bool winp::ui::window_surface::is_visible_() const{
	return ((handle_ == nullptr) ? has_styles_(WS_VISIBLE, false, true) : (IsWindowVisible(handle_) != FALSE));
}

SIZE winp::ui::window_surface::get_client_size_() const{
	if (handle_ == nullptr)
		return get_size_();

	RECT dimension{};
	GetClientRect(handle_, &dimension);

	return SIZE{ (dimension.right - dimension.left), (dimension.bottom - dimension.top) };
}

SIZE winp::ui::window_surface::get_current_client_size_() const{
	return get_client_size_();
}

POINT winp::ui::window_surface::get_client_offset_() const{
	if (handle_ == nullptr)
		return visible_surface::get_client_offset_();

	RECT dimension{};
	GetWindowRect(handle_, &dimension);

	POINT offset{};
	ClientToScreen(handle_, &offset);

	return POINT{ (offset.x - dimension.left), (offset.y - dimension.top) };
}

POINT winp::ui::window_surface::get_absolute_position_() const{
	if (handle_ == nullptr)
		return visible_surface::get_absolute_position_();

	RECT dimension{};
	GetWindowRect(handle_, &dimension);

	return POINT{ dimension.left, dimension.top };
}

winp::utility::error_code winp::ui::window_surface::update_dimension_(const RECT &previous_dimension, int x, int y, int width, int height, UINT flags){
	if (handle_ == nullptr)
		return utility::error_code::nil;

	if ((flags & SWP_NOMOVE) == 0u){
		auto relative_position = convert_position_relative_to_ancestor_<window_surface>(x, y);
		x = relative_position.x;
		y = relative_position.y;
	}

	updating_dimension_ = true;
	auto error_code = ((SetWindowPos(handle_, nullptr, x, y, width, height, (flags | SWP_NOZORDER | SWP_NOACTIVATE)) == FALSE) ? utility::error_code::action_could_not_be_completed : utility::error_code::nil);

	updating_dimension_ = false;
	trigger_event_<events::position_updated>(flags);

	if (!has_hook_<ui::no_drag_position_updated_hook>())
		trigger_event_<events::non_drag_position_updated>(flags);

	return error_code;
}

RECT winp::ui::window_surface::get_absolute_dimension_() const{
	if (handle_ == nullptr)
		return visible_surface::get_absolute_dimension_();

	RECT dimension{};
	GetWindowRect(handle_, &dimension);

	return dimension;
}

POINT winp::ui::window_surface::convert_position_from_absolute_value_(int x, int y) const{
	if (handle_ == nullptr)
		return visible_surface::convert_position_from_absolute_value_(x, y);

	POINT p{ x, y };
	ScreenToClient(handle_, &p);

	return p;
}

POINT winp::ui::window_surface::convert_position_to_absolute_value_(int x, int y) const{
	if (handle_ == nullptr)
		return visible_surface::convert_position_to_absolute_value_(x, y);

	POINT p{ x, y };
	ClientToScreen(handle_, &p);

	return p;
}

UINT winp::ui::window_surface::absolute_hit_test_(int x, int y) const{
	return ((handle_ == nullptr) ? visible_surface::absolute_hit_test_(x, y) : static_cast<UINT>(SendMessageW(handle_, WM_NCHITTEST, 0, MAKELONG(x, y))));
}

bool winp::ui::window_surface::is_dialog_message_(MSG &msg) const{
	return (handle_ != nullptr && IsDialogMessageW(handle_, &msg) != FALSE);
}

void winp::ui::window_surface::update_position_() const{
	if (handle_ == nullptr)
		return;

	auto &current_position = get_current_position_();
	auto relative_position = convert_position_relative_to_ancestor_<window_surface>(current_position.x, current_position.y);

	SetWindowPos(handle_, nullptr, relative_position.x, relative_position.y, 0, 0, (SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE));
}

winp::utility::error_code winp::ui::window_surface::show_(int how){
	if (handle_ != nullptr)
		return ((ShowWindow(handle_, how) == FALSE) ? utility::error_code::action_could_not_be_completed : utility::error_code::nil);

	switch (how){
	case SW_HIDE:
		return set_styles_((styles_ & ~WS_VISIBLE), false);
	case SW_MAXIMIZE:
		return set_styles_((styles_ | WS_MAXIMIZE), false);
	case SW_MINIMIZE:
		return set_styles_((styles_ | WS_MINIMIZE), false);
	default:
		break;
	}

	return set_styles_((styles_ | WS_VISIBLE), false);
}

winp::utility::error_code winp::ui::window_surface::maximize_(){
	if (handle_ == nullptr)
		return set_styles_((styles_ | WS_MAXIMIZE), false);

	if (IsZoomed(handle_) == FALSE)
		return ((ShowWindow(handle_, SW_MAXIMIZE) == FALSE) ? utility::error_code::action_could_not_be_completed : utility::error_code::nil);

	return utility::error_code::nil;
}

winp::utility::error_code winp::ui::window_surface::restore_maximized_(){
	if (handle_ == nullptr)
		return set_styles_((styles_ & ~WS_MAXIMIZE), false);

	if (IsZoomed(handle_) != FALSE)
		return ((ShowWindow(handle_, SW_RESTORE) == FALSE) ? utility::error_code::action_could_not_be_completed : utility::error_code::nil);

	return utility::error_code::nil;
}

bool winp::ui::window_surface::is_maximized_() const{
	return ((handle_ == nullptr) ? has_styles_(WS_MAXIMIZE, false, true) : IsZoomed(handle_) != FALSE);
}

winp::utility::error_code winp::ui::window_surface::minimize_(){
	if (handle_ == nullptr)
		return set_styles_((styles_ | WS_MINIMIZE), false);

	if (IsIconic(handle_) == FALSE)
		return ((ShowWindow(handle_, SW_MINIMIZE) == FALSE) ? utility::error_code::action_could_not_be_completed : utility::error_code::nil);

	return utility::error_code::nil;
}

winp::utility::error_code winp::ui::window_surface::restore_minimized_(){
	if (handle_ == nullptr)
		return set_styles_((styles_ & ~WS_MINIMIZE), false);

	if (IsIconic(handle_) != FALSE)
		return ((ShowWindow(handle_, SW_RESTORE) == FALSE) ? utility::error_code::action_could_not_be_completed : utility::error_code::nil);

	return utility::error_code::nil;
}

bool winp::ui::window_surface::is_minimized_() const{
	return ((handle_ == nullptr) ? has_styles_(WS_MINIMIZE, false, true) : IsIconic(handle_) != FALSE);
}

winp::utility::error_code winp::ui::window_surface::set_styles_(DWORD value, bool is_extended){
	value &= ~get_filtered_styles_(is_extended);
	if (value == (is_extended ? extended_styles_ : styles_))
		return utility::error_code::nil;//No changes

	(is_extended ? extended_styles_ : styles_) = value;
	if (handle_ != nullptr){//Update window
		SetWindowLongPtrW(handle_, (is_extended ? GWL_EXSTYLE : GWL_STYLE), value);
		SetWindowPos(handle_, nullptr, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
	}

	return utility::error_code::nil;
}

DWORD winp::ui::window_surface::get_styles_(bool is_extended) const{
	return (((is_extended ? extended_styles_ : styles_) & ~get_filtered_styles_(is_extended)) | get_persistent_styles_(is_extended));
}

bool winp::ui::window_surface::has_styles_(DWORD value, bool is_extended, bool has_all) const{
	return (has_all ? ((get_styles_(is_extended) & value) == value) : ((get_styles_(is_extended) & value) != 0u));
}

DWORD winp::ui::window_surface::get_persistent_styles_(bool is_extended) const{
	return (is_extended ?  0u : (((parent_ == nullptr) ? 0u : WS_CHILD) | WS_CLIPCHILDREN | WS_CLIPSIBLINGS));
}

DWORD winp::ui::window_surface::get_filtered_styles_(bool is_extended) const{
	return (is_extended ? 0u : ((parent_ == nullptr) ? WS_CHILD : (WS_CHILD | WS_POPUP)));
}

HWND winp::ui::window_surface::get_handle_() const{
	return handle_;
}

POINT winp::ui::window_surface::get_context_menu_position_() const{
	return convert_position_to_absolute_value_(0, 0);
}

const std::wstring &winp::ui::window_surface::get_class_name_() const{
	return app::object::get_class_name();
}

const wchar_t *winp::ui::window_surface::get_window_text_() const{
	return L"";
}

const wchar_t *winp::ui::window_surface::get_theme_id_() const{
	return L"WINDOW";
}

HINSTANCE winp::ui::window_surface::get_instance_() const{
	return GetModuleHandleW(nullptr);
}

bool winp::ui::window_surface::is_top_level_() const{
	return (parent_ == nullptr);
}

bool winp::ui::window_surface::traverse_child_windows_(const tree &parent, const std::function<bool(window_surface &)> &callback) const{
	auto &children = parent.get_children();
	if (children.empty())
		return true;

	for (auto child : children){
		if (auto item_child = dynamic_cast<window_surface *>(child); item_child != nullptr){
			if (!callback(*item_child))
				return false;
		}
		else if (auto tree_child = dynamic_cast<tree *>(child); tree_child != nullptr && !traverse_child_windows_(*tree_child, callback))
			return false;
	}

	return true;
}
