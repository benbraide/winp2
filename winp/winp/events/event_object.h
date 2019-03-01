#pragma once

#include "../thread/thread_queue.h"

namespace winp::thread{
	class item;
}

namespace winp::ui{
	class object;
	class tree;
}

namespace winp::events{
	class object{
	public:
		explicit object(thread::item &target, const std::function<void(object &)> &default_handler = nullptr);

		object(thread::item &target, thread::item &context, const std::function<void(object &)> &default_handler = nullptr);

		virtual ~object();

		object(const object &) = delete;

		object &operator =(const object &) = delete;

		virtual thread::object &get_thread();

		virtual thread::item &get_target();

		virtual thread::item &get_context();

		template <typename result_type>
		void set_result(const result_type &result){
			if (!is_thread_context())
				throw utility::error_code::outside_thread_context;

			states_ |= state_result_set;
			result_ = result;
		}

		template <typename result_type>
		void set_result_if_not_set(const result_type &result){
			if (!is_thread_context())
				throw utility::error_code::outside_thread_context;

			if ((states_ & state_result_set) == 0u){
				states_ |= state_result_set;
				result_ = result;
			}
		}

		template <typename target_type = LRESULT>
		target_type get_result() const{
			return (target_type)result_;
		}

		virtual void prevent_default();

		virtual bool do_default();

		virtual void stop_propagation();

		virtual unsigned int get_states() const;

		virtual bool is_thread_context() const;

		static constexpr unsigned int state_nil							= (0 << 0x0000);
		static constexpr unsigned int state_default_prevented			= (1 << 0x0000);
		static constexpr unsigned int state_propagation_stopped			= (1 << 0x0001);
		static constexpr unsigned int state_doing_default				= (1 << 0x0002);
		static constexpr unsigned int state_default_done				= (1 << 0x0003);
		static constexpr unsigned int state_result_set					= (1 << 0x0004);
		static constexpr unsigned int state_default_result_set			= (1 << 0x0005);

	protected:
		friend class thread::item;

		thread::item &target_;
		thread::item *context_;

		LRESULT result_ = 0;
		unsigned int states_ = state_nil;
		std::function<void(object &)> default_handler_;
	};

	class object_with_message : public object{
	public:
		template <typename... args_types>
		explicit object_with_message(MSG &message, WNDPROC default_callback, args_types &&... args)
			: object_with_message(message, message, default_callback, args...){}

		template <typename... args_types>
		explicit object_with_message(MSG &message, MSG &original_message, WNDPROC default_callback, args_types &&... args)
			: object(std::forward<args_types>(args)...), message_(message), original_message_(original_message), default_callback_(default_callback){}

		virtual ~object_with_message();

		virtual bool do_default() override;

		virtual const MSG &get_message() const;

		virtual MSG &get_message();

	protected:
		virtual bool should_call_call_default_() const;

		virtual bool call_default_();

		virtual LRESULT get_called_default_value_();

		MSG &message_;
		MSG &original_message_;
		WNDPROC default_callback_;
	};

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

	class parent_change : public object{
	public:
		template <typename... args_types>
		explicit parent_change(ui::tree *value, bool is_changing, args_types &&... args)
			: object(std::forward<args_types>(args)...), value_(value), is_changing_(is_changing){}

		virtual ui::tree *get_value() const;

		virtual bool is_changing() const;

	protected:
		ui::tree *value_;
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

	class cursor : public object_with_message{
	public:
		template <typename... args_types>
		explicit cursor(args_types &&... args)
			: object_with_message(std::forward<args_types>(args)...){}

		virtual WORD get_hit_target() const;

		virtual WORD get_mouse_button() const;

	protected:
		virtual bool call_default_() override;
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

	class mouse : public object_with_message{
	public:
		template <typename... args_types>
		explicit mouse(args_types &&... args)
			: object_with_message(std::forward<args_types>(args)...){
			auto position = ::GetMessagePos();
			position_.x = GET_X_LPARAM(position);
			position_.y = GET_Y_LPARAM(position);
		}

		virtual bool is_non_client() const;

		virtual const POINT &get_position() const;

		virtual POINT get_offset() const;

		virtual unsigned int get_button() const;

		static const unsigned int button_type_nil				= (0 << 0x00);
		static const unsigned int button_type_left				= (1 << 0x00);
		static const unsigned int button_type_middle			= (1 << 0x01);
		static const unsigned int button_type_right				= (1 << 0x02);

	protected:
		unsigned int button_ = button_type_nil;
		POINT position_{};
	};

	class mouse_leave : public mouse{
	public:
		template <typename... args_types>
		explicit mouse_leave(args_types &&... args)
			: mouse(std::forward<args_types>(args)...){}

		virtual bool is_non_client() const override;

	protected:
		virtual bool should_call_call_default_() const override;
	};

	class mouse_enter : public mouse{
	public:
		template <typename... args_types>
		explicit mouse_enter(args_types &&... args)
			: mouse(std::forward<args_types>(args)...){}

		virtual bool is_non_client() const override;

	protected:
		virtual bool should_call_call_default_() const override;
	};

	class mouse_move : public mouse{
	public:
		template <typename... args_types>
		explicit mouse_move(args_types &&... args)
			: mouse(std::forward<args_types>(args)...){}

		virtual bool is_non_client() const override;
	};

	class mouse_drag_begin : public mouse{
	public:
		template <typename... args_types>
		explicit mouse_drag_begin(args_types &&... args)
			: mouse(std::forward<args_types>(args)...){}

		virtual unsigned int get_button() const override;
	};

	class mouse_drag : public mouse{
	public:
		template <typename... args_types>
		explicit mouse_drag(bool is_initial, args_types &&... args)
			: mouse(std::forward<args_types>(args)...), is_initial_(is_initial){}

		virtual POINT get_offset() const override;

		virtual unsigned int get_button() const override;

	protected:
		bool is_initial_;
	};

	class mouse_drag_end : public mouse{
	public:
		template <typename... args_types>
		explicit mouse_drag_end(args_types &&... args)
			: mouse(std::forward<args_types>(args)...){}
	};

	class mouse_down : public mouse{
	public:
		template <typename... args_types>
		explicit mouse_down(unsigned int button, bool is_non_client, args_types &&... args)
			: mouse(std::forward<args_types>(args)...), is_non_client_(is_non_client){
			button_ = button;
		}

		virtual bool is_non_client() const override;

	protected:
		bool is_non_client_;
	};

	class mouse_up : public mouse{
	public:
		template <typename... args_types>
		explicit mouse_up(unsigned int button, bool is_non_client, args_types &&... args)
			: mouse(std::forward<args_types>(args)...), is_non_client_(is_non_client){
			button_ = button;
		}

		virtual bool is_non_client() const override;

	protected:
		bool is_non_client_;
	};

	class mouse_dbl_clk : public mouse{
	public:
		template <typename... args_types>
		explicit mouse_dbl_clk(unsigned int button, bool is_non_client, args_types &&... args)
			: mouse(std::forward<args_types>(args)...), is_non_client_(is_non_client){
			button_ = button;
		}

		virtual bool is_non_client() const override;

	protected:
		bool is_non_client_;
	};
}
