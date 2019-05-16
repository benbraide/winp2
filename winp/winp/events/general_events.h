#pragma once

#include <string>
#include <variant>

#include "event_object.h"

namespace winp::events{
	class unhandled : public object_with_message{
	public:
		template <typename... args_types>
		explicit unhandled(args_types &&... args)
			: object_with_message(std::forward<args_types>(args)...){}
	};

	class create : public object_with_message{
	public:
		template <typename... args_types>
		explicit create(bool is_changing, args_types &&... args)
			: object_with_message(std::forward<args_types>(args)...){}

		virtual bool is_creating() const;
	};

	class destroy : public object_with_message{
	public:
		template <typename... args_types>
		explicit destroy(args_types &&... args)
			: object_with_message(std::forward<args_types>(args)...){}
	};

	class destruct : public object{
	public:
		template <typename... args_types>
		explicit destruct(args_types &&... args)
			: object(std::forward<args_types>(args)...){}
	};

	class close : public object_with_message{
	public:
		template <typename... args_types>
		explicit close(args_types &&... args)
			: object_with_message(std::forward<args_types>(args)...){}
	};

	class index_change : public object{
	public:
		template <typename... args_types>
		explicit index_change(std::size_t value, bool is_changing, args_types &&... args)
			: object(std::forward<args_types>(args)...), value_(value), is_changing_(is_changing){}

		virtual std::size_t get_value() const;

		virtual bool is_changing() const;

	protected:
		std::size_t value_;
		bool is_changing_;
	};

	class parent_change : public object{
	public:
		template <typename... args_types>
		explicit parent_change(ui::tree *value, ui::tree *previous, bool is_changing, args_types &&... args)
			: object(std::forward<args_types>(args)...), value_(value), previous_(previous), is_changing_(is_changing){}

		virtual ui::tree *get_value() const;

		virtual ui::tree *get_previous_value() const;

		virtual bool is_changing() const;

	protected:
		ui::tree *value_;
		ui::tree *previous_;
		bool is_changing_;
	};

	class children_change : public object{
	public:
		enum class action_type{
			insert,
			remove,
			index,
		};

		template <typename... args_types>
		explicit children_change(action_type action, ui::object &value, std::size_t index, bool is_changing, args_types &&... args)
			: object(std::forward<args_types>(args)...), action_(action), value_(value), index_(index), is_changing_(is_changing){}

		virtual action_type get_action() const;

		virtual const ui::object &get_value() const;

		virtual ui::object &get_value();

		virtual std::size_t get_index() const;

		virtual bool is_changing() const;

	protected:
		action_type action_;
		ui::object &value_;
		std::size_t index_;
		bool is_changing_;
	};

	class position_change : public object_with_message{
	public:
		template <typename... args_types>
		explicit position_change(bool is_changing, args_types &&... args)
			: object_with_message(std::forward<args_types>(args)...), is_changing_(is_changing){}

		virtual WINDOWPOS &get_value() const;

		virtual bool is_changing() const;

	protected:
		bool is_changing_;
	};

	class background_brush_change : public object{
	public:
		template <typename... args_types>
		explicit background_brush_change(ID2D1Brush *value, bool is_changing, args_types &&... args)
			: object(std::forward<args_types>(args)...), value_(value), is_changing_(is_changing){}

		virtual ID2D1Brush *get_value() const;

		virtual bool is_changing() const;

	protected:
		ID2D1Brush *value_;
		bool is_changing_;
	};

	class background_color_change : public object{
	public:
		template <typename... args_types>
		explicit background_color_change(const D2D1_COLOR_F &value, bool is_changing, args_types &&... args)
			: object(std::forward<args_types>(args)...), value_(value), is_changing_(is_changing){}

		virtual const D2D1_COLOR_F &get_value() const;

		virtual bool is_changing() const;

	protected:
		D2D1_COLOR_F value_;
		bool is_changing_;
	};

	class visibility_change : public object{
	public:
		template <typename... args_types>
		explicit visibility_change(bool value, bool is_changing, args_types &&... args)
			: object(std::forward<args_types>(args)...), value_(value), is_changing_(is_changing){}

		virtual bool get_value() const;

		virtual bool is_changing() const;

	protected:
		bool value_;
		bool is_changing_;
	};

	class show : public object_with_message{
	public:
		template <typename... args_types>
		explicit show(args_types &&... args)
			: object_with_message(std::forward<args_types>(args)...){}
	};

	class hide : public object_with_message{
	public:
		template <typename... args_types>
		explicit hide(args_types &&... args)
			: object_with_message(std::forward<args_types>(args)...){}
	};

	class background_brush : public object_with_message{
	public:
		template <typename... args_types>
		explicit background_brush(args_types &&... args)
			: object_with_message(std::forward<args_types>(args)...){}

	protected:
		virtual bool should_call_call_default_() const override;

		virtual LRESULT get_called_default_value_() override;
	};

	class background_color : public object_with_message{
	public:
		template <typename... args_types>
		explicit background_color(args_types &&... args)
			: object_with_message(std::forward<args_types>(args)...){}

		using object_with_message::set_result;

		void set_result(const D2D1::ColorF &result);

	protected:
		virtual bool should_call_call_default_() const override;

		virtual LRESULT get_called_default_value_() override;
	};

	class draw : public object_with_message{
	public:
		using m_opt_point_type = std::variant<POINT, D2D1_POINT_2F>;
		using m_opt_size_type = std::variant<SIZE, D2D1_SIZE_F>;
		using m_opt_rect_type = std::variant<RECT, D2D1_RECT_F>;
		using m_opt_color_type = std::variant<COLORREF, D2D1_COLOR_F>;
		using m_opt_paint_type = std::variant<COLORREF, D2D1_COLOR_F, ID2D1Brush *>;

		template <typename... args_types>
		explicit draw(args_types &&... args)
			: object_with_message(std::forward<args_types>(args)...){}

		virtual ~draw();

		virtual utility::error_code begin();

		virtual utility::error_code end();

		virtual ID2D1RenderTarget *get_render_target() const;

		virtual ID2D1SolidColorBrush *get_color_brush() const;

		virtual HDC get_device() const;

		virtual const RECT &get_clip() const;

		virtual void draw_line(const m_opt_point_type &start, const m_opt_point_type &stop, const m_opt_paint_type &paint, float stroke_width = 1.0f, ID2D1StrokeStyle *stroke_style = nullptr);

		virtual void draw_lines(const m_opt_point_type &start, const m_opt_point_type &stop, const m_opt_point_type &step, unsigned int count, const m_opt_paint_type &paint, float stroke_width = 1.0f, ID2D1StrokeStyle *stroke_style = nullptr);

		virtual void draw_rectangle(const m_opt_rect_type &region, const m_opt_paint_type &paint, float stroke_width = 1.0f, ID2D1StrokeStyle *stroke_style = nullptr);

		virtual void draw_rectangles(const m_opt_rect_type &region, const m_opt_rect_type &step, unsigned int count, const m_opt_paint_type &paint, float stroke_width = 1.0f, ID2D1StrokeStyle *stroke_style = nullptr);

		virtual void draw_round_rectangle(const m_opt_rect_type &region, const m_opt_size_type &radius, const m_opt_paint_type &paint, float stroke_width = 1.0f, ID2D1StrokeStyle *stroke_style = nullptr);

		virtual void draw_ellipse(const m_opt_rect_type &region, const m_opt_paint_type &paint, float stroke_width = 1.0f, ID2D1StrokeStyle *stroke_style = nullptr);

		virtual void draw_ellipse(const m_opt_point_type &center, const m_opt_size_type &radius, const m_opt_paint_type &paint, float stroke_width = 1.0f, ID2D1StrokeStyle *stroke_style = nullptr);

		virtual void draw_geometry(ID2D1Geometry *geometry, const m_opt_paint_type &paint, float stroke_width = 1.0f, ID2D1StrokeStyle *stroke_style = nullptr);

		virtual void draw_text(const std::wstring &text, IDWriteTextFormat *format, const m_opt_rect_type &bound, const m_opt_paint_type &paint, D2D1_DRAW_TEXT_OPTIONS options = D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE measuring_mode = DWRITE_MEASURING_MODE_NATURAL);

		virtual void draw_text_layout(const m_opt_point_type &origin, IDWriteTextLayout *layout, const m_opt_paint_type &paint, D2D1_DRAW_TEXT_OPTIONS options = D2D1_DRAW_TEXT_OPTIONS_NONE);

		virtual void fill_rectangle(const m_opt_rect_type &region, const m_opt_paint_type &paint);

		virtual void fill_round_rectangle(const m_opt_rect_type &region, const m_opt_size_type &radius, const m_opt_paint_type &paint);

		virtual void fill_ellipse(const m_opt_rect_type &region, const m_opt_paint_type &paint);

		virtual void fill_ellipse(const m_opt_point_type &center, const m_opt_size_type &radius, const m_opt_paint_type &paint);

		virtual void fill_geometry(ID2D1Geometry *geometry, const m_opt_paint_type &paint);

		virtual void fill_mesh(ID2D1Mesh *mesh, const m_opt_paint_type &paint);

	protected:
		virtual utility::error_code begin_() = 0;

		virtual void end_() = 0;

		virtual D2D1_POINT_2F get_dip_point_(const m_opt_point_type &point);

		virtual D2D1_SIZE_F get_dip_size_(const m_opt_size_type &size);

		virtual D2D1_RECT_F get_dip_rect_(const m_opt_rect_type &rect);

		virtual ID2D1Brush *get_brush_(const m_opt_paint_type &paint);

		ID2D1DCRenderTarget *render_target_ = nullptr;
		ID2D1SolidColorBrush *color_brush_ = nullptr;
		PAINTSTRUCT info_{};
	};

	class erase_background : public draw{
	public:
		template <typename... args_types>
		explicit erase_background(args_types &&... args)
			: draw(std::forward<args_types>(args)...){}

		virtual ~erase_background();

	protected:
		virtual bool should_call_call_default_() const override;

		virtual LRESULT get_called_default_value_() override;

		virtual utility::error_code begin_() override;

		virtual void end_() override;
	};

	class paint : public draw{
	public:
		template <typename... args_types>
		explicit paint(args_types &&... args)
			: draw(std::forward<args_types>(args)...){}

		virtual ~paint();

	protected:
		virtual bool should_call_call_default_() const override;

		virtual utility::error_code begin_() override;

		virtual void end_() override;

		bool began_paint_ = false;
	};

	class owner_draw : public draw{
	public:
		template <typename... args_types>
		explicit owner_draw(args_types &&... args)
			: draw(std::forward<args_types>(args)...){}

		virtual ~owner_draw();

	protected:
		virtual bool should_call_call_default_() const override;

		virtual utility::error_code begin_() override;

		virtual void end_() override;
	};

	class measure_item : public object_with_message{
	public:
		template <typename... args_types>
		explicit measure_item(args_types &&... args)
			: object_with_message(std::forward<args_types>(args)...){}
	};

	class enable : public object_with_message{
	public:
		template <typename... args_types>
		explicit enable(args_types &&... args)
			: object_with_message(std::forward<args_types>(args)...){}

		virtual bool is_enabled() const;
	};
}
