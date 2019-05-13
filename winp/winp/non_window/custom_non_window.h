#pragma once

#include "../ui/ui_non_window_surface.h"

namespace winp::events{
	class create_non_window_handle : public object{
	public:
		template <typename... args_types>
		explicit create_non_window_handle(args_types &&... args)
			: object(std::forward<args_types>(args)...){}
	};

	class update_non_window_handle : public object{
	public:
		template <typename... args_types>
		explicit update_non_window_handle(HRGN handle, args_types &&... args)
			: object(std::forward<args_types>(args)...), handle_(handle){}

		virtual HRGN get_handle() const;

	protected:
		HRGN handle_ = nullptr;
	};

	class destroy_non_window_handle : public object{
	public:
		template <typename... args_types>
		explicit destroy_non_window_handle(HRGN handle, args_types &&... args)
			: object(std::forward<args_types>(args)...), handle_(handle){}

		virtual HRGN get_handle() const;

	protected:
		HRGN handle_ = nullptr;
	};
}

namespace winp::non_window{
	class custom : public ui::non_window_surface{
	public:
		custom();

		explicit custom(thread::object &thread);

		explicit custom(tree &parent);

		custom(tree &parent, std::size_t index);

		virtual ~custom();

	protected:
		custom(thread::object &thread, bool init_grid);

		virtual utility::error_code create_() override;

		virtual HRGN create_handle_() const override;

		virtual utility::error_code update_handle_() override;

		virtual utility::error_code destroy_handle_() override;
	};
}
