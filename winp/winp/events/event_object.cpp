#include "../app/app_collection.h"
#include "../ui/ui_window_surface.h"
#include "../ui/ui_non_window_surface.h"

winp::events::object::object(thread::item &target, const std::function<void(object &)> &default_handler)
	: object(target, target, default_handler){}

winp::events::object::object(thread::item &target, thread::item &context, const std::function<void(object &)> &default_handler)
	: target_(target), context_(&context), default_handler_(default_handler){}

winp::events::object::~object() = default;

winp::thread::object &winp::events::object::get_thread(){
	return target_.get_thread();
}

winp::thread::item &winp::events::object::get_target(){
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return target_;
}

winp::thread::item &winp::events::object::get_context(){
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return *context_;
}

void winp::events::object::prevent_default(){
	if (target_.get_thread().is_thread_context())
		states_ |= state_default_prevented;
	else
		throw utility::error_code::outside_thread_context;
}

bool winp::events::object::do_default(){
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;

	if ((states_ & (state_default_prevented | state_doing_default | state_default_done)) != 0u)
		return false;//Default prevented or done

	states_ |= state_doing_default;
	if (default_handler_ == nullptr)
		context_->trigger_event_handler_(*this);
	else//Call default handler
		default_handler_(*this);

	states_ |= state_default_done;
	states_ &= ~state_doing_default;

	return true;
}

void winp::events::object::stop_propagation(){
	if (target_.get_thread().is_thread_context())
		states_ |= state_propagation_stopped;
	else
		throw utility::error_code::outside_thread_context;
}

unsigned int winp::events::object::get_states() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return states_;
}

bool winp::events::object::is_thread_context() const{
	return target_.get_thread().is_thread_context();
}

winp::events::object_with_message::~object_with_message() = default;

bool winp::events::object_with_message::do_default(){
	return ((object::do_default() && (states_ & state_default_prevented) == 0u) ? call_default_() : false);
}

const MSG &winp::events::object_with_message::get_message() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return message_;
}

MSG &winp::events::object_with_message::get_message(){
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return message_;
}

bool winp::events::object_with_message::should_call_call_default_() const{
	if (default_callback_ == nullptr || original_message_.hwnd == nullptr || WM_APP <= original_message_.message || (states_ & state_default_prevented) != 0u)
		return false;

	if (auto window_context = dynamic_cast<ui::window_surface *>(context_); window_context != nullptr && window_context->get_handle() == original_message_.hwnd)
		return true;

	return false;
}

bool winp::events::object_with_message::call_default_(){
	if ((states_ & state_default_prevented) == 0u && should_call_call_default_()){//Call default
		if ((states_ & state_result_set) == 0u){//Use result
			states_ |= state_default_result_set;
			result_ = get_called_default_value_();
		}
		else//Discard result
			get_called_default_value_();
	}

	return true;
}

LRESULT winp::events::object_with_message::get_called_default_value_(){
	return CallWindowProcW(default_callback_, original_message_.hwnd, original_message_.message, original_message_.wParam, original_message_.lParam);
}

winp::ui::tree *winp::events::parent_change::get_value() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return value_;
}

bool winp::events::parent_change::is_changing() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return is_changing_;
}

winp::events::children_change::action_type winp::events::children_change::get_action() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return action_;
}

const winp::ui::object &winp::events::children_change::get_value() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return value_;
}

winp::ui::object &winp::events::children_change::get_value(){
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return value_;
}

std::size_t winp::events::children_change::get_index() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return index_;
}

bool winp::events::children_change::is_changing() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return is_changing_;
}

WINDOWPOS &winp::events::position_change::get_value() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return *reinterpret_cast<WINDOWPOS *>(message_.lParam);
}

bool winp::events::position_change::is_changing() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return is_changing_;
}

const D2D1::ColorF &winp::events::background_color_change::get_value() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return *value_;
}

bool winp::events::background_color_change::is_changing() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return is_changing_;
}

bool winp::events::background_transparency_change::get_value() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return value_;
}

bool winp::events::background_transparency_change::is_changing() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return is_changing_;
}

bool winp::events::visibility_change::get_value() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return value_;
}

bool winp::events::visibility_change::is_changing() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return is_changing_;
}

WORD winp::events::cursor::get_hit_target() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return LOWORD(message_.lParam);
}

WORD winp::events::cursor::get_mouse_button() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return HIWORD(message_.lParam);
}

bool winp::events::cursor::call_default_(){
	return true;
}

void winp::events::background_color::set_result(const D2D1::ColorF &result){
	set_result(ui::visible_surface::convert_colorf_to_colorref(result));
}

bool winp::events::background_color::should_call_call_default_() const{
	return ((states_ & state_default_prevented) == 0u);
}

LRESULT winp::events::background_color::get_called_default_value_(){
	if (auto visible_context = dynamic_cast<ui::visible_surface *>(context_); visible_context != nullptr)
		return static_cast<LRESULT>(ui::visible_surface::convert_colorf_to_colorref(visible_context->get_background_color()));
	return 0;
}

winp::events::draw::~draw(){
	end();
}

winp::utility::error_code winp::events::draw::begin(){
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;

	if (render_target_ != nullptr)
		return utility::error_code::nil;

	if (auto error_code = begin_(); error_code != utility::error_code::nil)
		return error_code;

	if ((render_target_ = target_.get_thread().get_device_render_target()) == nullptr){
		end_();
		info_ = PAINTSTRUCT{};
		return utility::error_code::action_could_not_be_completed;
	}

	color_brush_ = target_.get_thread().get_color_brush();
	color_brush_->SetColor(D2D1::ColorF(D2D1::ColorF::Black, 1.0f));

	computed_clip_ = info_.rcPaint;
	SaveDC(info_.hdc);

	if (auto non_window_context = dynamic_cast<ui::non_window_surface *>(context_); non_window_context != nullptr){
		SelectClipRgn(info_.hdc, non_window_context->get_handle());

		auto offset = non_window_context->get_position();
		auto context_dimension = non_window_context->get_dimension();

		for (auto ancestor = non_window_context->get_parent(); ancestor != nullptr; ancestor = ancestor->get_parent()){
			if (auto window_ancestor = dynamic_cast<ui::window_surface *>(ancestor); window_ancestor != nullptr)
				break;

			if (auto surface_ancestor = dynamic_cast<ui::surface *>(ancestor); surface_ancestor != nullptr){
				auto ancestor_position = surface_ancestor->get_position();
				offset.x += ancestor_position.x;
				offset.y += ancestor_position.y;
			}
		}

		OffsetRect(&context_dimension, (offset.x - context_dimension.left), (offset.y - context_dimension.top));//Move relative to offset
		OffsetClipRgn(info_.hdc, offset.x, offset.y);
		IntersectClipRect(info_.hdc, computed_clip_.left, computed_clip_.top, computed_clip_.right, computed_clip_.bottom);

		SetViewportOrgEx(info_.hdc, offset.x, offset.y, nullptr);
		IntersectRect(&computed_clip_, &computed_clip_, &context_dimension);

		OffsetRect(&context_dimension, -offset.x, -offset.y);
		computed_clip_ = context_dimension;
	}

	render_target_->BindDC(info_.hdc, &computed_clip_);
	render_target_->SetTransform(D2D1::IdentityMatrix());
	render_target_->BeginDraw();

	return utility::error_code::nil;
}

winp::utility::error_code winp::events::draw::end(){
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;

	if (render_target_ == nullptr)
		return utility::error_code::nil;

	switch (render_target_->EndDraw()){
	case D2DERR_RECREATE_TARGET:
		target_.get_thread().discard_d2d_resources();
		return utility::error_code::action_could_not_be_completed;
	case S_OK:
		break;
	default:
		return utility::error_code::action_could_not_be_completed;
	}

	RestoreDC(info_.hdc, -1);
	end_();

	render_target_ = nullptr;
	color_brush_ = nullptr;
	info_ = PAINTSTRUCT{};

	return utility::error_code::nil;
}

ID2D1RenderTarget *winp::events::draw::get_render_target() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return render_target_;
}

ID2D1SolidColorBrush *winp::events::draw::get_color_brush() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return color_brush_;
}

HDC winp::events::draw::get_device() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return info_.hdc;
}

const RECT &winp::events::draw::get_clip() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return info_.rcPaint;
}

winp::events::erase_background::~erase_background(){
	end();
}

bool winp::events::erase_background::should_call_call_default_() const{
	return ((states_ & state_default_prevented) == 0u);
}

LRESULT winp::events::erase_background::get_called_default_value_(){
	auto window_context = dynamic_cast<ui::window_surface *>(context_);
	if (window_context != nullptr && window_context->get_thread().get_app().get_class_entry(window_context->get_class_name()) != DefWindowProcW)
		return object_with_message::get_called_default_value_();

	auto background_color = ui::visible_surface::convert_colorref_to_colorf(static_cast<COLORREF>(context_->get_thread().send_message(*context_, WINP_WM_GET_BACKGROUND_COLOR)));
	if (begin() != utility::error_code::nil)
		return 0;

	render_target_->Clear(background_color);
	return 0;
}

winp::utility::error_code winp::events::erase_background::begin_(){
	info_.hdc = reinterpret_cast<HDC>(message_.wParam);
	GetClipBox(info_.hdc, &info_.rcPaint);
	return utility::error_code::nil;
}

void winp::events::erase_background::end_(){}

winp::events::paint::~paint(){
	end();
}

bool winp::events::paint::should_call_call_default_() const{
	return (!began_paint_ && object_with_message::should_call_call_default_());
}

winp::utility::error_code winp::events::paint::begin_(){
	if (original_message_.hwnd == nullptr)
		return utility::error_code::action_could_not_be_completed;

	auto window_context = dynamic_cast<ui::window_surface *>(context_);
	if ((states_ & state_default_done) != 0u || window_context == nullptr || target_.get_thread().get_app().get_class_entry(window_context->get_class_name()) != DefWindowProcW){
		if ((info_.hdc = GetDC(original_message_.hwnd)) == nullptr)
			return utility::error_code::action_could_not_be_completed;
		info_.rcPaint = target_.get_thread().get_item_manager().get_update_rect();
	}
	else{//Begin paint
		began_paint_ = true;
		BeginPaint(original_message_.hwnd, &info_);
	}

	return utility::error_code::nil;
}

void winp::events::paint::end_(){
	if (began_paint_){
		EndPaint(original_message_.hwnd, &info_);
		began_paint_ = false;
	}
	else
		ReleaseDC(original_message_.hwnd, info_.hdc);
}
