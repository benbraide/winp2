#pragma once

#include "event_object.h"

namespace winp::events{
	class cursor : public object_with_message{
	public:
		template <typename... args_types>
		explicit cursor(args_types &&... args)
			: object_with_message(std::forward<args_types>(args)...){}

		virtual WORD get_hit_target() const;

		virtual WORD get_mouse_button() const;

	protected:
		virtual void call_default_() override;
	};

	class hit_test : public object_with_message{
	public:
		template <typename... args_types>
		explicit hit_test(args_types &&... args)
			: object_with_message(std::forward<args_types>(args)...){
			position_.x = GET_X_LPARAM(message_info_.lParam);
			position_.y = GET_Y_LPARAM(message_info_.lParam);
		}

		virtual const POINT &get_position() const;

	protected:
		POINT position_{};
	};

	class mouse : public object_with_message{
	public:
		template <typename... args_types>
		explicit mouse(unsigned int button, bool is_non_client, args_types &&... args)
			: object_with_message(std::forward<args_types>(args)...), button_(button), is_non_client_(is_non_client){
			auto position = ::GetMessagePos();
			position_.x = GET_X_LPARAM(position);
			position_.y = GET_Y_LPARAM(position);
		}

		virtual bool is_non_client() const;

		virtual const POINT &get_position() const;

		virtual unsigned int get_button() const;

		static const unsigned int button_type_nil			= (0 << 0x00);
		static const unsigned int button_type_left			= (1 << 0x00);
		static const unsigned int button_type_middle		= (1 << 0x01);
		static const unsigned int button_type_right			= (1 << 0x02);

	protected:
		unsigned int button_;
		bool is_non_client_;
		POINT position_{};
	};

	class mouse_leave : public mouse{
	public:
		template <typename... args_types>
		explicit mouse_leave(args_types &&... args)
			: mouse(std::forward<args_types>(args)...){}
	};

	class mouse_client_change : public mouse{
	public:
		template <typename... args_types>
		explicit mouse_client_change(args_types &&... args)
			: mouse(std::forward<args_types>(args)...){}
	};

	class mouse_enter : public mouse{
	public:
		template <typename... args_types>
		explicit mouse_enter(args_types &&... args)
			: mouse(std::forward<args_types>(args)...){}
	};

	class mouse_move : public mouse{
	public:
		template <typename... args_types>
		explicit mouse_move(args_types &&... args)
			: mouse(std::forward<args_types>(args)...){}
	};

	class mouse_down : public mouse{
	public:
		template <typename... args_types>
		explicit mouse_down(args_types &&... args)
			: mouse(std::forward<args_types>(args)...){}
	};

	class mouse_up : public mouse{
	public:
		template <typename... args_types>
		explicit mouse_up(args_types &&... args)
			: mouse(std::forward<args_types>(args)...){}
	};

	class mouse_dbl_clk : public mouse{
	public:
		template <typename... args_types>
		explicit mouse_dbl_clk(args_types &&... args)
			: mouse(std::forward<args_types>(args)...){}
	};

	class mouse_wheel : public mouse{
	public:
		template <typename... args_types>
		explicit mouse_wheel(args_types &&... args)
			: mouse(std::forward<args_types>(args)...){}

		virtual bool is_vertical() const;

		virtual SIZE get_delta() const;
	};

	class mouse_drag_begin : public mouse{
	public:
		template <typename... args_types>
		explicit mouse_drag_begin(args_types &&... args)
			: mouse(std::forward<args_types>(args)...){}
	};

	class mouse_drag : public mouse{
	public:
		template <typename... args_types>
		explicit mouse_drag(const POINT &mouse_down_position, args_types &&... args)
			: mouse(std::forward<args_types>(args)...), mouse_down_position_(mouse_down_position){}

		virtual POINT get_offset() const;

	protected:
		POINT mouse_down_position_;
	};

	class mouse_drag_end : public mouse{
	public:
		template <typename... args_types>
		explicit mouse_drag_end(args_types &&... args)
			: mouse(std::forward<args_types>(args)...){}
	};

	class key : public object_with_message{
	public:
		template <typename... args_types>
		explicit key(args_types &&... args)
			: object_with_message(std::forward<args_types>(args)...){}

		virtual unsigned short get_virtual_code() const;

		virtual wchar_t get_scan_code() const;

		virtual bool is_extended() const;

		virtual bool check_key_state(unsigned short key) const;

		static const std::size_t key_states_size = 0x100;

	protected:
		mutable std::unique_ptr<BYTE[]> key_states_;
	};

	class key_down : public key{
	public:
		template <typename... args_types>
		explicit key_down(args_types &&... args)
			: key(std::forward<args_types>(args)...){}

		virtual bool is_first_down() const;

		virtual WORD get_repeat_count() const;
	};

	class key_up : public key{
	public:
		template <typename... args_types>
		explicit key_up(args_types &&... args)
			: key(std::forward<args_types>(args)...){}
	};

	class key_press : public key{
	public:
		template <typename... args_types>
		explicit key_press(args_types &&... args)
			: key(std::forward<args_types>(args)...){}

		virtual bool is_first_down() const;

		virtual bool is_being_released() const;
	};

	class set_focus : public object_with_message{
	public:
		template <typename... args_types>
		explicit set_focus(args_types &&... args)
			: object_with_message(std::forward<args_types>(args)...){}
	};

	class kill_focus : public object_with_message{
	public:
		template <typename... args_types>
		explicit kill_focus(args_types &&... args)
			: object_with_message(std::forward<args_types>(args)...){}
	};

	class activate : public object_with_message{
	public:
		template <typename... args_types>
		explicit activate(args_types &&... args)
			: object_with_message(std::forward<args_types>(args)...){}
	};

	class mouse_activate : public object_with_message{
	public:
		template <typename... args_types>
		explicit mouse_activate(args_types &&... args)
			: object_with_message(std::forward<args_types>(args)...){}
	};

	class click : public object_with_message{
	public:
		template <typename... args_types>
		explicit click(args_types &&... args)
			: object_with_message(std::forward<args_types>(args)...){}
	};

	class item_check : public object_with_message{
	public:
		template <typename... args_types>
		explicit item_check(args_types &&... args)
			: object_with_message(std::forward<args_types>(args)...){}

		virtual bool is_checked() const;

		virtual bool is_indeterminate() const;
	};

	class mouse_hover : public object{
	public:
		template <typename... args_types>
		explicit mouse_hover(bool hovered, args_types &&... args)
			: object(std::forward<args_types>(args)...), hovered_(hovered){}

		virtual bool is_hovered() const;

	protected:
		bool hovered_;
	};
}
