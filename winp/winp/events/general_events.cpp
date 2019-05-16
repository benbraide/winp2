#include "../app/app_object.h"
#include "../ui/ui_window_surface.h"
#include "../ui/ui_non_window_surface.h"

bool winp::events::create::is_creating() const{
	return (original_message_.message == WM_NCCREATE);
}

std::size_t winp::events::index_change::get_value() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return value_;
}

bool winp::events::index_change::is_changing() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return is_changing_;
}

winp::ui::tree *winp::events::parent_change::get_value() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return value_;
}

winp::ui::tree *winp::events::parent_change::get_previous_value() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return previous_;
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

ID2D1Brush *winp::events::background_brush_change::get_value() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return value_;
}

bool winp::events::background_brush_change::is_changing() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return is_changing_;
}

const D2D1_COLOR_F &winp::events::background_color_change::get_value() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return value_;
}

bool winp::events::background_color_change::is_changing() const{
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

bool winp::events::background_brush::should_call_call_default_() const{
	return ((states_ & state_default_prevented) == 0u);
}

LRESULT winp::events::background_brush::get_called_default_value_(){
	if (auto visible_context = dynamic_cast<ui::visible_surface *>(context_); visible_context != nullptr){
		auto brush = visible_context->get_background_brush();
		if (brush == nullptr){
			if (auto color_brush = target_.get_thread().get_color_brush(); color_brush != nullptr){
				auto color = ui::visible_surface::convert_colorref_to_colorf(static_cast<COLORREF>(context_->get_thread().send_message(*context_, WINP_WM_GET_BACKGROUND_COLOR)));
				if (color.a != 0.0f){//Non-transparent
					color_brush->SetColor(color);
					brush = color_brush;
				}
			}
		}
		
		return reinterpret_cast<LRESULT>(brush);
	}

	return 0;
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
	SaveDC(info_.hdc);

	if (auto non_window_context = dynamic_cast<ui::non_window_surface *>(context_); non_window_context != nullptr){
		SelectClipRgn(info_.hdc, non_window_context->get_handle());

		POINT offset{};
		auto context_dimension = non_window_context->get_dimension();

		auto window_ancestor = non_window_context->get_first_ancestor_of<ui::window_surface>([&](ui::tree &ancestor){
			if (auto surface_ancestor = dynamic_cast<ui::surface *>(&ancestor); surface_ancestor != nullptr){
				auto ancestor_position = surface_ancestor->get_position();
				auto ancestor_client_offset = surface_ancestor->get_client_offset();

				offset.x += (ancestor_position.x + ancestor_client_offset.x);
				offset.y += (ancestor_position.y + ancestor_client_offset.y);
			}

			return true;
		});

		if (window_ancestor != nullptr){
			auto ancestor_client_start_offset = window_ancestor->get_client_start_offset();
			offset.x += ancestor_client_start_offset.x;
			offset.y += ancestor_client_start_offset.y;
		}

		OffsetRect(&context_dimension, offset.x, offset.y);//Move relative to offset
		OffsetClipRgn(info_.hdc, context_dimension.left, context_dimension.top);

		IntersectClipRect(info_.hdc, info_.rcPaint.left, info_.rcPaint.top, info_.rcPaint.right, info_.rcPaint.bottom);
		SetViewportOrgEx(info_.hdc, context_dimension.left, context_dimension.top, nullptr);

		IntersectRect(&info_.rcPaint, &info_.rcPaint, &context_dimension);
		OffsetRect(&info_.rcPaint, -context_dimension.left, -context_dimension.top);
	}

	RECT window_client_rect{};
	GetClientRect(original_message_.hwnd, &window_client_rect);

	render_target_->BindDC(info_.hdc, &window_client_rect);
	render_target_->SetTransform(D2D1::IdentityMatrix());
	render_target_->BeginDraw();

	return utility::error_code::nil;
}

winp::utility::error_code winp::events::draw::end(){
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;

	if (render_target_ == nullptr)
		return utility::error_code::nil;

	auto error_code = utility::error_code::nil;
	switch (render_target_->EndDraw()){
	case D2DERR_RECREATE_TARGET:
		target_.get_thread().discard_d2d_resources();
		error_code = utility::error_code::action_could_not_be_completed;
		break;
	case S_OK:
		break;
	default:
		error_code = utility::error_code::action_could_not_be_completed;
		break;
	}

	RestoreDC(info_.hdc, -1);
	end_();

	render_target_ = nullptr;
	color_brush_ = nullptr;
	info_ = PAINTSTRUCT{};

	return error_code;
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

void winp::events::draw::draw_line(const m_opt_point_type &start, const m_opt_point_type &stop, const m_opt_paint_type &paint, float stroke_width, ID2D1StrokeStyle *stroke_style){
	if (begin() != utility::error_code::nil)
		return;//Failed to initialize

	if (auto brush = get_brush_(paint); brush != nullptr)
		render_target_->DrawLine(get_dip_point_(start), get_dip_point_(stop), brush, stroke_width, stroke_style);
}

void winp::events::draw::draw_lines(const m_opt_point_type &start, const m_opt_point_type &stop, const m_opt_point_type &step, unsigned int count, const m_opt_paint_type &paint, float stroke_width, ID2D1StrokeStyle *stroke_style){
	if (begin() != utility::error_code::nil)
		return;//Failed to initialize

	auto brush = get_brush_(paint);
	if (brush == nullptr)
		return;

	auto dip_start = get_dip_point_(start);
	auto dip_stop = get_dip_point_(stop);
	auto dip_step = get_dip_point_(step);

	for (auto index = 0u; index < count; ++index){
		render_target_->DrawLine(dip_start, dip_stop, brush, stroke_width, stroke_style);
		dip_start = D2D1::Point2F((dip_start.x + dip_step.x), (dip_start.y + dip_step.y));
		dip_stop = D2D1::Point2F((dip_stop.x + dip_step.x), (dip_stop.y + dip_step.y));
	}
}

void winp::events::draw::draw_rectangle(const m_opt_rect_type &region, const m_opt_paint_type &paint, float stroke_width, ID2D1StrokeStyle *stroke_style){
	if (begin() != utility::error_code::nil)
		return;//Failed to initialize

	if (auto brush = get_brush_(paint); brush != nullptr)
		render_target_->DrawRectangle(get_dip_rect_(region), brush, stroke_width, stroke_style);
}

void winp::events::draw::draw_rectangles(const m_opt_rect_type &region, const m_opt_rect_type &step, unsigned int count, const m_opt_paint_type &paint, float stroke_width, ID2D1StrokeStyle *stroke_style){
	if (begin() != utility::error_code::nil)
		return;//Failed to initialize

	auto brush = get_brush_(paint);
	if (brush == nullptr)
		return;

	auto dip_region = get_dip_rect_(region);
	auto dip_step = get_dip_rect_(step);

	for (auto index = 0u; index < count; ++index){
		render_target_->DrawRectangle(dip_region, brush, stroke_width, stroke_style);
		dip_region = D2D1::RectF((dip_region.left + dip_step.left), (dip_region.top + dip_step.top), (dip_region.right - dip_step.right), (dip_region.bottom - dip_step.bottom));
	}
}

void winp::events::draw::draw_round_rectangle(const m_opt_rect_type &region, const m_opt_size_type &radius, const m_opt_paint_type &paint, float stroke_width, ID2D1StrokeStyle *stroke_style){
	if (begin() != utility::error_code::nil)
		return;//Failed to initialize

	auto brush = get_brush_(paint);
	if (brush == nullptr)
		return;

	auto dip_region = get_dip_rect_(region);
	auto dip_radius = get_dip_size_(radius);

	render_target_->DrawRoundedRectangle(D2D1::RoundedRect(dip_region, dip_radius.width, dip_radius.height), brush, stroke_width, stroke_style);
}

void winp::events::draw::draw_ellipse(const m_opt_rect_type &region, const m_opt_paint_type &paint, float stroke_width, ID2D1StrokeStyle *stroke_style){
	auto dip_region = get_dip_rect_(region);
	D2D1_SIZE_F size{ (dip_region.right - dip_region.left), (dip_region.bottom - dip_region.top) };
	draw_ellipse(D2D1::Point2F((dip_region.left + size.width), (dip_region.top + size.height)), D2D1::SizeF((size.width / 2), (size.height / 2)), paint, stroke_width, stroke_style);
}

void winp::events::draw::draw_ellipse(const m_opt_point_type &center, const m_opt_size_type &radius, const m_opt_paint_type &paint, float stroke_width, ID2D1StrokeStyle *stroke_style){
	if (begin() != utility::error_code::nil)
		return;//Failed to initialize

	auto brush = get_brush_(paint);
	if (brush == nullptr)
		return;

	auto dip_center = get_dip_point_(center);
	auto dip_radius = get_dip_size_(radius);

	render_target_->DrawEllipse(D2D1::Ellipse(dip_center, dip_radius.width, dip_radius.height), brush, stroke_width, stroke_style);
}

void winp::events::draw::draw_geometry(ID2D1Geometry *geometry, const m_opt_paint_type &paint, float stroke_width, ID2D1StrokeStyle *stroke_style){
	if (begin() != utility::error_code::nil)
		return;//Failed to initialize

	if (auto brush = get_brush_(paint); brush != nullptr)
		render_target_->DrawGeometry(geometry, brush, stroke_width, stroke_style);
}

void winp::events::draw::draw_text(const std::wstring &text, IDWriteTextFormat *format, const m_opt_rect_type &bound, const m_opt_paint_type &paint, D2D1_DRAW_TEXT_OPTIONS options, DWRITE_MEASURING_MODE measuring_mode){
	if (begin() != utility::error_code::nil)
		return;//Failed to initialize

	if (auto brush = get_brush_(paint); brush != nullptr)
		render_target_->DrawTextW(text.data(), static_cast<UINT32>(text.size()), format, get_dip_rect_(bound), brush, options, measuring_mode);
}

void winp::events::draw::draw_text_layout(const m_opt_point_type &origin, IDWriteTextLayout *layout, const m_opt_paint_type &paint, D2D1_DRAW_TEXT_OPTIONS options){
	if (begin() != utility::error_code::nil)
		return;//Failed to initialize

	if (auto brush = get_brush_(paint); brush != nullptr)
		render_target_->DrawTextLayout(get_dip_point_(origin), layout, brush, options);
}

void winp::events::draw::fill_rectangle(const m_opt_rect_type &region, const m_opt_paint_type &paint){
	if (begin() != utility::error_code::nil)
		return;//Failed to initialize

	if (auto brush = get_brush_(paint); brush != nullptr)
		render_target_->FillRectangle(get_dip_rect_(region), brush);
}

void winp::events::draw::fill_round_rectangle(const m_opt_rect_type &region, const m_opt_size_type &radius, const m_opt_paint_type &paint){
	if (begin() != utility::error_code::nil)
		return;//Failed to initialize

	auto brush = get_brush_(paint);
	if (brush == nullptr)
		return;

	auto dip_region = get_dip_rect_(region);
	auto dip_radius = get_dip_size_(radius);

	render_target_->FillRoundedRectangle(D2D1::RoundedRect(dip_region, dip_radius.width, dip_radius.height), brush);
}

void winp::events::draw::fill_ellipse(const m_opt_rect_type &region, const m_opt_paint_type &paint){
	auto dip_region = get_dip_rect_(region);
	D2D1_SIZE_F size{ (dip_region.right - dip_region.left), (dip_region.bottom - dip_region.top) };
	fill_ellipse(D2D1::Point2F((dip_region.left + size.width), (dip_region.top + size.height)), D2D1::SizeF((size.width / 2), (size.height / 2)), paint);
}

void winp::events::draw::fill_ellipse(const m_opt_point_type &center, const m_opt_size_type &radius, const m_opt_paint_type &paint){
	if (begin() != utility::error_code::nil)
		return;//Failed to initialize

	auto brush = get_brush_(paint);
	if (brush == nullptr)
		return;

	auto dip_center = get_dip_point_(center);
	auto dip_radius = get_dip_size_(radius);

	render_target_->FillEllipse(D2D1::Ellipse(dip_center, dip_radius.width, dip_radius.height), brush);
}

void winp::events::draw::fill_geometry(ID2D1Geometry *geometry, const m_opt_paint_type &paint){
	if (begin() != utility::error_code::nil)
		return;//Failed to initialize

	if (auto brush = get_brush_(paint); brush != nullptr)
		render_target_->FillGeometry(geometry, brush, nullptr);
}

void winp::events::draw::fill_mesh(ID2D1Mesh *mesh, const m_opt_paint_type &paint){
	if (begin() != utility::error_code::nil)
		return;//Failed to initialize

	if (auto brush = get_brush_(paint); brush != nullptr)
		render_target_->FillMesh(mesh, brush);
}

D2D1_POINT_2F winp::events::draw::get_dip_point_(const m_opt_point_type &point){
	if (std::holds_alternative<D2D1_POINT_2F>(point))
		return std::get<D2D1_POINT_2F>(point);

	auto &ddp_point = std::get<POINT>(point);
	return D2D1_POINT_2F{
		target_.get_thread().convert_pixel_to_dip_x(ddp_point.x),
		target_.get_thread().convert_pixel_to_dip_y(ddp_point.y)
	};
}

D2D1_SIZE_F winp::events::draw::get_dip_size_(const m_opt_size_type &size){
	if (std::holds_alternative<D2D1_SIZE_F>(size))
		return std::get<D2D1_SIZE_F>(size);

	auto &ddp_size = std::get<SIZE>(size);
	return D2D1_SIZE_F{
		target_.get_thread().convert_pixel_to_dip_x(ddp_size.cx),
		target_.get_thread().convert_pixel_to_dip_y(ddp_size.cy)
	};
}

D2D1_RECT_F winp::events::draw::get_dip_rect_(const m_opt_rect_type &rect){
	if (std::holds_alternative<D2D1_RECT_F>(rect))
		return std::get<D2D1_RECT_F>(rect);

	auto &ddp_region = std::get<RECT>(rect);
	return D2D1_RECT_F{
		target_.get_thread().convert_pixel_to_dip_x(ddp_region.left),
		target_.get_thread().convert_pixel_to_dip_y(ddp_region.top),
		target_.get_thread().convert_pixel_to_dip_x(ddp_region.right),
		target_.get_thread().convert_pixel_to_dip_y(ddp_region.bottom)
	};
}

ID2D1Brush *winp::events::draw::get_brush_(const m_opt_paint_type &paint){
	if (std::holds_alternative<ID2D1Brush *>(paint))
		return std::get<ID2D1Brush *>(paint);

	if (color_brush_ != nullptr)
		color_brush_->SetColor(std::holds_alternative<D2D_COLOR_F>(paint) ? std::get<D2D_COLOR_F>(paint) : ui::visible_surface::convert_colorref_to_colorf(std::get<COLORREF>(paint), 255));

	return color_brush_;
}

winp::events::erase_background::~erase_background(){
	end();
}

bool winp::events::erase_background::should_call_call_default_() const{
	return ((states_ & state_default_prevented) == 0u);
}

LRESULT winp::events::erase_background::get_called_default_value_(){
	if (dynamic_cast<ui::visible_surface *>(context_) == nullptr)
		return object_with_message::get_called_default_value_();

	if (auto window_context = dynamic_cast<ui::window_surface *>(context_); window_context != nullptr && window_context->get_thread().get_class_entry(window_context->get_class_name()) != DefWindowProcW)
		return object_with_message::get_called_default_value_();

	auto background_brush = reinterpret_cast<ID2D1Brush *>(context_->get_thread().send_message(*context_, WINP_WM_GET_BACKGROUND_BRUSH));
	if (background_brush != nullptr && begin() == utility::error_code::nil)
		fill_rectangle(info_.rcPaint, background_brush);

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
	if (message_.message == WM_PRINTCLIENT){
		info_.hdc = reinterpret_cast<HDC>(message_.wParam);
		GetClipBox(info_.hdc, &info_.rcPaint);
	}
	else if ((states_ & state_default_done) != 0u || window_context == nullptr){
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
	else if (message_.message != WM_PRINTCLIENT)
		ReleaseDC(original_message_.hwnd, info_.hdc);
}

winp::events::owner_draw::~owner_draw(){
	end();
}

bool winp::events::owner_draw::should_call_call_default_() const{
	return false;
}

winp::utility::error_code winp::events::owner_draw::begin_(){
	if (original_message_.message == WM_DRAWITEM){
		auto info = reinterpret_cast<DRAWITEMSTRUCT *>(original_message_.lParam);
		info_.hdc = info->hDC;
		info_.rcPaint = info->rcItem;
	}

	return utility::error_code::nil;
}

void winp::events::owner_draw::end_(){}

bool winp::events::enable::is_enabled() const{
	if (!target_.get_thread().is_thread_context())
		throw utility::error_code::outside_thread_context;
	return (original_message_.wParam != FALSE);
}
