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

	class activity : public object{
	public:
		template <typename... args_types>
		explicit activity(unsigned __int64 key, std::size_t previous_count, std::size_t current_count, args_types &&... args)
			: object(std::forward<args_types>(args)...), key_(key), previous_count_(previous_count), current_count_(current_count){}

		virtual unsigned __int64 get_key() const;

		virtual std::size_t get_previous_count() const;

		virtual std::size_t get_current_count() const;

	protected:
		unsigned __int64 key_;
		std::size_t previous_count_;
		std::size_t current_count_;
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

	class create_non_window_handle : public object{
	public:
		template <typename... args_types>
		explicit create_non_window_handle(HRGN handle, const SIZE &current_size, args_types &&... args)
			: object(std::forward<args_types>(args)...), handle_(handle), current_size_(current_size){}

		virtual HRGN get_handle() const;

		virtual const SIZE &get_current_size() const;

	protected:
		HRGN handle_;
		SIZE current_size_;
	};

	class create_non_window_non_client_handle : public create_non_window_handle{
	public:
		template <typename... args_types>
		explicit create_non_window_non_client_handle(args_types &&... args)
			: create_non_window_handle(std::forward<args_types>(args)...){}
	};

	class destroy_non_window_handle : public object{
	public:
		template <typename... args_types>
		explicit destroy_non_window_handle(HRGN handle, args_types &&... args)
			: object(std::forward<args_types>(args)...), handle_(handle){}

		virtual HRGN get_handle() const;

	protected:
		HRGN handle_;
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

	class position_updated : public object{
	public:
		template <typename... args_types>
		explicit position_updated(UINT flags, args_types &&... args)
			: object(std::forward<args_types>(args)...), flags_(flags){}

		virtual UINT get_flags() const;

	protected:
		UINT flags_;
	};

	class non_drag_position_updated : public object{
	public:
		template <typename... args_types>
		explicit non_drag_position_updated(UINT flags, args_types &&... args)
			: object(std::forward<args_types>(args)...), flags_(flags){}

		virtual UINT get_flags() const;

	protected:
		UINT flags_;
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
		virtual bool should_call_default_() const override;

		virtual LRESULT get_called_default_value_() override;
	};

	class background_color : public object_with_message{
	public:
		template <typename... args_types>
		explicit background_color(args_types &&... args)
			: object_with_message(std::forward<args_types>(args)...){}

		using object_with_message::set_result;

	protected:
		virtual bool set_result_untyped_(const std::any &result) override;

		virtual bool should_call_default_() const override;

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

		virtual HTHEME get_theme() const;

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

		virtual void draw_themed_background(int part_id, int state_id, const RECT &region);

		virtual void draw_themed_text(int part_id, int state_id, const std::wstring &text, DWORD format_flags, const RECT &region);

	protected:
		virtual utility::error_code begin_() = 0;

		virtual void end_() = 0;

		virtual D2D1_POINT_2F get_dip_point_(const m_opt_point_type &point);

		virtual D2D1_SIZE_F get_dip_size_(const m_opt_size_type &size);

		virtual D2D1_RECT_F get_dip_rect_(const m_opt_rect_type &rect);

		virtual ID2D1Brush *get_brush_(const m_opt_paint_type &paint);

		virtual HTHEME get_theme_() const;

		ID2D1DCRenderTarget *render_target_ = nullptr;
		ID2D1SolidColorBrush *color_brush_ = nullptr;

		PAINTSTRUCT info_{};
		mutable HTHEME theme_ = nullptr;
	};

	class erase_background : public draw{
	public:
		template <typename... args_types>
		explicit erase_background(args_types &&... args)
			: draw(std::forward<args_types>(args)...){}

		virtual ~erase_background();

	protected:
		virtual bool should_call_default_() const override;

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
		virtual bool should_call_default_() const override;

		virtual utility::error_code begin_() override;

		virtual void end_() override;

		bool began_paint_ = false;
	};

	class non_client_paint : public draw{
	public:
		template <typename... args_types>
		explicit non_client_paint(args_types &&... args)
			: draw(std::forward<args_types>(args)...){}

		virtual ~non_client_paint();

		virtual utility::error_code begin() override;

	protected:
		virtual bool should_call_default_() const override;

		virtual LRESULT get_called_default_value_() override;

		virtual utility::error_code begin_() override;

		virtual void end_() override;

		HWND dc_owner_ = nullptr;
		RECT viewport_{};
	};

	class draw_item : public draw{
	public:
		template <typename... args_types>
		explicit draw_item(args_types &&... args)
			: draw(std::forward<args_types>(args)...){}

		virtual ~draw_item();

	protected:
		virtual bool should_call_default_() const override;

		virtual utility::error_code begin_() override;

		virtual void end_() override;
	};

	class measure_item : public object_with_message{
	public:
		template <typename... args_types>
		explicit measure_item(args_types &&... args)
			: object_with_message(std::forward<args_types>(args)...){}

		virtual ~measure_item();

		virtual HTHEME get_theme() const;

		virtual SIZE measure_text(const std::wstring &text, IDWriteTextFormat *format, D2D1_DRAW_TEXT_OPTIONS options = D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE measuring_mode = DWRITE_MEASURING_MODE_NATURAL);

		virtual SIZE measure_text(const std::wstring &text, HFONT font, DWORD format_flags);

		virtual SIZE measure_themed_text(int part_id, int state_id, const std::wstring &text, DWORD format_flags);

	protected:
		virtual bool set_result_untyped_(const std::any &result) override;

		virtual HTHEME get_theme_() const;

		virtual void cache_device_context_();

		mutable HTHEME theme_ = nullptr;
		std::pair<HDC, HWND> device_context_ = std::make_pair<HDC, HWND>(nullptr, nullptr);
	};

	class enable : public object_with_message{
	public:
		template <typename... args_types>
		explicit enable(args_types &&... args)
			: object_with_message(std::forward<args_types>(args)...){}

		virtual bool is_enabled() const;
	};

	class timer : public object{
	public:
		template <typename... args_types>
		explicit timer(bool needs_duration, args_types &&... args)
			: object(std::forward<args_types>(args)...), needs_duration_(needs_duration){
			if (!needs_duration_)
				states_ |= state_unbind_on_exit;
		}

		virtual bool needs_duration() const;

	protected:
		virtual bool set_result_untyped_(const std::any &result) override;

		bool needs_duration_;
	};

	class interval : public timer{
	public:
		template <typename... args_types>
		explicit interval(args_types &&... args)
			: timer(std::forward<args_types>(args)...){
			states_ &= ~state_unbind_on_exit;
		}
	};

	class animation : public object{
	public:
		using key_type = unsigned __int64;

		enum class progress_type{
			nil,
			begin,
			step,
			end,
			cancel,
		};

		template <typename... args_types>
		explicit animation(key_type type, progress_type progress, args_types &&... args)
			: object(std::forward<args_types>(args)...), type_(type), progress_(progress){}

		virtual key_type get_type() const;

		template <typename target_type>
		bool is_type() const{
			return (get_type() == reinterpret_cast<key_type>(&typeid(target_type)));
		}

		virtual progress_type get_progress() const;

	protected:
		key_type type_;
		progress_type progress_;
	};
}
