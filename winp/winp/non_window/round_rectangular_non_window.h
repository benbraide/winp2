#pragma once

#include "../ui/ui_non_window_surface.h"

namespace winp::non_window{
	class round_rectangle : public ui::non_window_surface{
	public:
		round_rectangle();

		explicit round_rectangle(thread::object &thread);

		explicit round_rectangle(tree &parent);

		round_rectangle(tree &parent, std::size_t index);

		virtual ~round_rectangle();

		virtual utility::error_code set_border_curve_size(const SIZE &value, const std::function<void(round_rectangle &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code set_border_curve_size(int width, int height, const std::function<void(round_rectangle &, utility::error_code)> &callback = nullptr);

		virtual const SIZE &get_border_curve_size(const std::function<void(const SIZE &)> &callback = nullptr) const;

	protected:
		round_rectangle(thread::object &thread, bool init_grid);

		virtual utility::error_code set_border_curve_size_(int width, int height);

		SIZE border_curve_size_{};
	};
}
