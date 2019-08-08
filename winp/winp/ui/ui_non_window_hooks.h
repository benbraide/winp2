#pragma once

#include "ui_hook.h"

namespace winp::ui{
	class non_window_surface;

	class non_window_shape_hook : public typed_hook<non_window_surface>{
	public:
		using base_type = typed_hook<non_window_surface>;

		explicit non_window_shape_hook(object &target);

		virtual ~non_window_shape_hook();

	protected:
		virtual bool setup_() override;
	};

	class rectangular_non_window_hook : public non_window_shape_hook{
	public:
		explicit rectangular_non_window_hook(object &target);

		virtual ~rectangular_non_window_hook();
	};

	class round_rectangular_non_window_hook : public non_window_shape_hook{
	public:
		explicit round_rectangular_non_window_hook(object &target);

		round_rectangular_non_window_hook(object &target, const SIZE &border_curve_size);

		round_rectangular_non_window_hook(object &target, int border_curve_width, int border_curve_height);

		virtual ~round_rectangular_non_window_hook();

		virtual utility::error_code set_border_curve_size(const SIZE &value, const std::function<void(round_rectangular_non_window_hook &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code set_border_curve_size(int width, int height, const std::function<void(round_rectangular_non_window_hook &, utility::error_code)> &callback = nullptr);

		virtual const SIZE &get_border_curve_size(const std::function<void(const SIZE &)> &callback = nullptr) const;

	protected:
		virtual utility::error_code set_border_curve_size_(int width, int height);

		SIZE border_curve_size_{};
	};

	class elliptical_non_window_hook : public non_window_shape_hook{
	public:
		explicit elliptical_non_window_hook(object &target);

		virtual ~elliptical_non_window_hook();
	};

	class non_window_non_client_hook : public typed_hook<non_window_surface>{
	public:
		using base_type = typed_hook<non_window_surface>;

		explicit non_window_non_client_hook(object &target);

		virtual ~non_window_non_client_hook();

		virtual utility::error_code set_caption(const std::wstring &value, const std::function<void(non_window_non_client_hook &, utility::error_code)> &callback = nullptr);

		virtual const std::wstring &get_caption(const std::function<void(const std::wstring &)> &callback = nullptr) const;

	protected:
		friend class non_window_surface;

		friend class events::draw;
		friend class events::non_client_paint;

		virtual utility::error_code set_caption_(const std::wstring &value);

		virtual const std::wstring &get_caption_() const;

		utility::rgn handle_;
		RECT padding_{};
		std::wstring caption_;
	};

	class non_window_non_client_shape_hook : public typed_hook<non_window_surface>{
	public:
		using base_type = typed_hook<non_window_surface>;

		explicit non_window_non_client_shape_hook(object &target);

		virtual ~non_window_non_client_shape_hook();

	protected:
		virtual bool setup_() override;
	};

	class rectangular_non_window_non_client_hook : public non_window_non_client_shape_hook{
	public:
		explicit rectangular_non_window_non_client_hook(object &target);

		virtual ~rectangular_non_window_non_client_hook();
	};

	class round_rectangular_non_window_non_client_hook : public non_window_non_client_shape_hook{
	public:
		explicit round_rectangular_non_window_non_client_hook(object &target);

		round_rectangular_non_window_non_client_hook(object &target, const SIZE &border_curve_size);

		round_rectangular_non_window_non_client_hook(object &target, int border_curve_width, int border_curve_height);

		virtual ~round_rectangular_non_window_non_client_hook();

		virtual utility::error_code set_border_curve_size(const SIZE &value, const std::function<void(round_rectangular_non_window_non_client_hook &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code set_border_curve_size(int width, int height, const std::function<void(round_rectangular_non_window_non_client_hook &, utility::error_code)> &callback = nullptr);

		virtual const SIZE &get_border_curve_size(const std::function<void(const SIZE &)> &callback = nullptr) const;

	protected:
		virtual utility::error_code set_border_curve_size_(int width, int height);

		SIZE border_curve_size_{};
	};

	class elliptical_non_window_non_client_hook : public non_window_non_client_shape_hook{
	public:
		explicit elliptical_non_window_non_client_hook(object &target);

		virtual ~elliptical_non_window_non_client_hook();
	};
}
