#include "../app/app_collection.h"

#include "ui_window_surface.h"

winp::ui::non_window_surface::non_window_surface()
	: non_window_surface(app::collection::get_main()->get_thread()){}

winp::ui::non_window_surface::non_window_surface(thread::object &thread)
	: non_window_surface(thread, true){}

winp::ui::non_window_surface::non_window_surface(thread::object &thread, bool init_grid)
	: tree(thread){
	background_color_ = convert_colorref_to_colorf(GetSysColor(COLOR_WINDOW), 255);
	if (init_grid){
		is_auto_createable_ = false;
		init_grid_(*this);
		is_auto_createable_ = true;
	}
}

winp::ui::non_window_surface::non_window_surface(tree &parent)
	: non_window_surface(parent, static_cast<std::size_t>(-1)){}

winp::ui::non_window_surface::non_window_surface(tree &parent, std::size_t index)
	: non_window_surface(parent.get_thread()){
	set_parent(&parent, index);
}

winp::ui::non_window_surface::~non_window_surface(){
	destruct();
}

HRGN winp::ui::non_window_surface::get_handle(const std::function<void(HRGN)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, get_handle_());
	}, (callback != nullptr), nullptr);
}

winp::utility::error_code winp::ui::non_window_surface::create_(){
	if (is_created_())
		return utility::error_code::nil;

	if (parent_ != nullptr)
		parent_->auto_create();

	if ((handle_ = create_handle_()) == nullptr)
		return utility::error_code::action_could_not_be_completed;

	if (thread_.send_message(*this, WM_NCCREATE) != FALSE){
		if (visible_)
			redraw_();

		return utility::error_code::nil;
	}

	DeleteObject(handle_);
	handle_ = nullptr;

	return utility::error_code::action_prevented;
}

winp::utility::error_code winp::ui::non_window_surface::destroy_(){
	if (handle_ == nullptr || !is_created_())
		return utility::error_code::nil;

	hide_();
	if (auto error_code = destroy_handle_(); error_code != utility::error_code::nil)
		return error_code;

	handle_ = nullptr;
	thread_.send_message(*this, WM_NCDESTROY);

	return utility::error_code::nil;
}

bool winp::ui::non_window_surface::is_created_() const{
	return (handle_ != nullptr);
}

winp::utility::error_code winp::ui::non_window_surface::set_dimension_(int x, int y, int width, int height){
	previous_dimension_ = get_dimension_();
	if (auto error_code = visible_surface::set_dimension_(x, y, width, height); error_code != utility::error_code::nil)
		return error_code;

	if (!is_created_())
		return utility::error_code::nil;//Do nothing

	auto dimension = get_dimension_();
	if (dimension.left != previous_dimension_.left || dimension.top != previous_dimension_.top || dimension.right != previous_dimension_.right || dimension.bottom != previous_dimension_.bottom)
		update_handle_();

	if (!is_created_() || !is_visible_())
		return utility::error_code::nil;//Do nothing

	POINT offset{};
	auto window_ancestor = get_first_ancestor_of_<window_surface>([&](tree &ancestor){
		if (auto surface_ancestor = dynamic_cast<surface *>(&ancestor); surface_ancestor != nullptr){
			auto ancestor_position = surface_ancestor->get_position();
			auto ancestor_client_offset = surface_ancestor->get_client_offset();

			offset.x += (ancestor_position.x + ancestor_client_offset.x);
			offset.y += (ancestor_position.y + ancestor_client_offset.y);
		}

		return true;
	});

	if (window_ancestor == nullptr)//Do nothing
		return utility::error_code::nil;

	OffsetRect(&previous_dimension_, offset.x, offset.y);
	OffsetRect(&dimension, offset.x, offset.y);

	window_ancestor->redraw(previous_dimension_);
	window_ancestor->redraw(dimension);

	return utility::error_code::nil;
}

winp::utility::error_code winp::ui::non_window_surface::redraw_() const{
	auto dimension = get_dimension_();
	OffsetRect(&dimension, -dimension.left, -dimension.top);
	return redraw_(dimension);
}

winp::utility::error_code winp::ui::non_window_surface::redraw_(const RECT &region) const{
	if (handle_ == nullptr)
		return utility::error_code::object_not_created;

	if (parent_ == nullptr || !parent_->is_created())
		return utility::error_code::parent_not_created;

	if (auto visible_parent = dynamic_cast<visible_surface *>(parent_); visible_parent != nullptr)
		return visible_parent->redraw(RECT{ (region.left + position_.x), (region.top + position_.y), (region.right + position_.x), (region.bottom + position_.y) });

	return utility::error_code::nil;
}

winp::utility::error_code winp::ui::non_window_surface::set_visibility_(bool is_visible, bool redraw){
	if (redraw)
		return (is_visible ? show_() : hide_());

	visible_ = is_visible;
	return utility::error_code::nil;
}

winp::utility::error_code winp::ui::non_window_surface::show_(){
	if (visible_)
		return utility::error_code::nil;

	visible_ = true;
	return ((handle_ == nullptr) ? utility::error_code::nil : redraw_());
}

winp::utility::error_code winp::ui::non_window_surface::hide_(){
	if (!visible_ || handle_ == nullptr || parent_ == nullptr || !parent_->is_created())
		return utility::error_code::nil;

	visible_ = false;
	if (auto visible_parent = dynamic_cast<visible_surface *>(parent_); visible_parent != nullptr)
		return visible_parent->redraw(get_dimension_());

	return utility::error_code::nil;
}

bool winp::ui::non_window_surface::is_visible_() const{
	return visible_;
}

HRGN winp::ui::non_window_surface::get_handle_() const{
	return handle_;
}

HRGN winp::ui::non_window_surface::create_handle_() const{
	return nullptr;
}

winp::utility::error_code winp::ui::non_window_surface::update_handle_(){
	return utility::error_code::not_supported;
}

winp::utility::error_code winp::ui::non_window_surface::destroy_handle_(){
	return ((DeleteObject(handle_) == FALSE) ? utility::error_code::action_could_not_be_completed : utility::error_code::nil);
}
