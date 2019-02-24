#include "../app/app_collection.h"

#include "ui_window_surface.h"

winp::ui::window_surface::window_surface()
	: window_surface(app::collection::get_main()->get_thread()){}

winp::ui::window_surface::window_surface(thread::object &thread)
	: tree(thread){}

winp::ui::window_surface::window_surface(tree &parent)
	: window_surface(parent, static_cast<std::size_t>(-1)){}

winp::ui::window_surface::window_surface(tree &parent, std::size_t index)
	: window_surface(parent.get_thread()){
	set_parent(&parent, index);
}

winp::ui::window_surface::~window_surface() = default;

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

const std::wstring &winp::ui::window_surface::get_class_name(const std::function<void(const std::wstring &)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, get_class_name_());
	}, (callback != nullptr), thread_.get_app().get_class_name());
}

winp::utility::error_code winp::ui::window_surface::create_(){
	if (handle_ != nullptr)
		return utility::error_code::nil;

	HWND parent_handle = nullptr;
	auto position = position_;

	for (auto ancestor = parent_; ancestor != nullptr; ancestor = ancestor->get_parent()){
		if (auto window_ancestor = dynamic_cast<window_surface *>(ancestor); window_ancestor != nullptr){
			parent_handle = window_ancestor->handle_;
			break;
		}

		if (auto surface_ancestor = dynamic_cast<surface *>(ancestor); surface_ancestor != nullptr){
			auto ancestor_position = surface_ancestor->get_position();
			position.x += ancestor_position.x;
			position.y += ancestor_position.y;
		}
	}

	handle_ = CreateWindowExW(
		get_styles_(true),
		get_class_name_().data(),
		get_window_text_(),
		get_styles_(false),
		position.x,
		position.y,
		size_.cx,
		size_.cy,
		parent_handle,
		nullptr,
		get_instance_(),
		this
	);

	if (handle_ == nullptr)
		return utility::error_code::action_could_not_be_completed;

	trigger_event_<events::create>();
	return utility::error_code::nil;
}

winp::utility::error_code winp::ui::window_surface::destroy_(){
	if (handle_ == nullptr)
		return utility::error_code::nil;

	if (DestroyWindow(handle_) == FALSE)
		return utility::error_code::action_could_not_be_completed;

	return utility::error_code::nil;
}

bool winp::ui::window_surface::is_created_() const{
	return (handle_ != nullptr);
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

POINT winp::ui::window_surface::get_absolute_position_() const{
	if (handle_ == nullptr)
		return interactive_surface::get_absolute_position_();

	RECT dimension{};
	GetWindowRect(handle_, &dimension);

	return POINT{ dimension.left, dimension.top };
}

winp::utility::error_code winp::ui::window_surface::set_dimension_(int x, int y, int width, int height){
	if (handle_ == nullptr)
		return interactive_surface::set_dimension_(x, y, width, height);
	return ((SetWindowPos(handle_, nullptr, x, y, width, height, (SWP_NOZORDER | SWP_NOACTIVATE)) == FALSE) ? utility::error_code::action_could_not_be_completed : utility::error_code::nil);
}

RECT winp::ui::window_surface::get_absolute_dimension_() const{
	if (handle_ == nullptr)
		return interactive_surface::get_absolute_dimension_();

	RECT dimension{};
	GetWindowRect(handle_, &dimension);

	return dimension;
}

POINT winp::ui::window_surface::convert_position_from_absolute_value_(int x, int y) const{
	if (handle_ == nullptr)
		return interactive_surface::convert_position_from_absolute_value_(x, y);

	POINT p{ x, y };
	ScreenToClient(handle_, &p);

	return p;
}

POINT winp::ui::window_surface::convert_position_to_absolute_value_(int x, int y) const{
	if (handle_ == nullptr)
		return interactive_surface::convert_position_to_absolute_value_(x, y);

	POINT p{ x, y };
	ClientToScreen(handle_, &p);

	return p;
}

UINT winp::ui::window_surface::absolute_hit_test_(int x, int y) const{
	return ((handle_ == nullptr) ? interactive_surface::absolute_hit_test_(x, y) : static_cast<UINT>(SendMessageW(handle_, WM_NCHITTEST, 0, MAKELONG(x, y))));
}

bool winp::ui::window_surface::is_dialog_message_(MSG &msg) const{
	return (handle_ != nullptr && IsDialogMessageW(handle_, &msg) != FALSE);
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

DWORD winp::ui::window_surface::get_styles_(bool is_extended_) const{
	return ((is_extended_ ? extended_styles_ : styles_) | get_persistent_styles_(is_extended_));
}

bool winp::ui::window_surface::has_styles_(DWORD value, bool is_extended, bool has_all) const{
	return (has_all ? ((get_styles_(is_extended) & value) == value) : ((get_styles_(is_extended) & value) != 0u));
}

DWORD winp::ui::window_surface::get_persistent_styles_(bool is_extended) const{
	return (is_extended ?  0u : (((parent_ == nullptr) ? WS_POPUP : WS_CHILD) | WS_CLIPCHILDREN | WS_CLIPSIBLINGS));
}

DWORD winp::ui::window_surface::get_filtered_styles_(bool is_extended) const{
	return (is_extended ? 0u : ((parent_ == nullptr) ? WS_POPUP : WS_CHILD));
}

const std::wstring &winp::ui::window_surface::get_class_name_() const{
	return thread_.get_app().get_class_name();
}

const wchar_t *winp::ui::window_surface::get_window_text_() const{
	return L"";
}

HINSTANCE winp::ui::window_surface::get_instance_() const{
	return GetModuleHandleW(nullptr);
}
