#pragma once

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
		explicit create(args_types &&... args)
			: object_with_message(std::forward<args_types>(args)...){}
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

	class background_color_change : public object{
	public:
		template <typename... args_types>
		explicit background_color_change(const D2D1::ColorF &value, bool is_changing, args_types &&... args)
			: object(std::forward<args_types>(args)...), value_(&value), is_changing_(is_changing){}

		virtual const D2D1::ColorF &get_value() const;

		virtual bool is_changing() const;

	protected:
		const D2D1::ColorF *value_;
		bool is_changing_;
	};

	class background_transparency_change : public object{
	public:
		template <typename... args_types>
		explicit background_transparency_change(bool value, bool is_changing, args_types &&... args)
			: object(std::forward<args_types>(args)...), value_(value), is_changing_(is_changing){}

		virtual bool get_value() const;

		virtual bool is_changing() const;

	protected:
		bool value_;
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

	protected:
		virtual utility::error_code begin_() = 0;

		virtual void end_() = 0;

		ID2D1DCRenderTarget *render_target_ = nullptr;
		ID2D1SolidColorBrush *color_brush_ = nullptr;

		PAINTSTRUCT info_{};
		RECT computed_clip_{};
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
