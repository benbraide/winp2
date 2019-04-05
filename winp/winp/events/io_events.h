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
		virtual bool call_default_() override;
	};

	class hit_test : public object_with_message{
	public:
		template <typename... args_types>
		explicit hit_test(args_types &&... args)
			: object_with_message(std::forward<args_types>(args)...){
			position_.x = GET_X_LPARAM(original_message_.lParam);
			position_.y = GET_Y_LPARAM(original_message_.lParam);
		}

		virtual const POINT &get_position() const;

	protected:
		POINT position_{};
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

		virtual unsigned int get_button() const;

		static const unsigned int button_type_nil = (0 << 0x00);
		static const unsigned int button_type_left = (1 << 0x00);
		static const unsigned int button_type_middle = (1 << 0x01);
		static const unsigned int button_type_right = (1 << 0x02);

	protected:
		unsigned int button_ = button_type_nil;
		POINT position_{};
	};

	class mouse_leave : public mouse{
	public:
		template <typename... args_types>
		explicit mouse_leave(unsigned int button, bool is_non_client, args_types &&... args)
			: mouse(std::forward<args_types>(args)...){}

		virtual bool is_non_client() const override;
	};

	class mouse_move : public mouse{
	public:
		template <typename... args_types>
		explicit mouse_move(unsigned int button, bool is_non_client, args_types &&... args)
			: mouse(std::forward<args_types>(args)...){}

		virtual bool is_non_client() const override;
	};

	class mouse_down : public mouse{
	public:
		template <typename... args_types>
		mouse_down(unsigned int button, bool is_non_client, args_types &&... args)
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
		mouse_up(unsigned int button, bool is_non_client, args_types &&... args)
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
		mouse_dbl_clk(unsigned int button, bool is_non_client, args_types &&... args)
			: mouse(std::forward<args_types>(args)...), is_non_client_(is_non_client){
			button_ = button;
		}

		virtual bool is_non_client() const override;

	protected:
		bool is_non_client_;
	};

	class mouse_wheel : public mouse{
	public:
		template <typename... args_types>
		explicit mouse_wheel(unsigned int button, bool is_non_client, args_types &&... args)
			: mouse(std::forward<args_types>(args)...){}

		virtual bool is_vertical() const;

		virtual SIZE get_delta() const;
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
}
