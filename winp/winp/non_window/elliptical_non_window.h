#pragma once

#include "../ui/ui_non_window_surface.h"

namespace winp::non_window{
	class ellipsis : public ui::non_window_surface{
	public:
		ellipsis();

		explicit ellipsis(thread::object &thread);

		explicit ellipsis(tree &parent);

		ellipsis(tree &parent, std::size_t index);

		virtual ~ellipsis();

	protected:
		ellipsis(thread::object &thread, bool init_grid);
	};

	class circle : public ellipsis{
	public:
		circle();

		explicit circle(thread::object &thread);

		explicit circle(tree &parent);

		circle(tree &parent, std::size_t index);

		virtual ~circle();

		virtual utility::error_code set_radius(int value, const std::function<void(circle &, utility::error_code)> &callback = nullptr);

		virtual int get_radius(const std::function<void(int)> &callback = nullptr) const;

	protected:
		circle(thread::object &thread, bool init_grid);

		virtual utility::error_code set_dimension_(int x, int y, int width, int height) override;
	};
}
