#pragma once

#include "../ui/ui_non_window_surface.h"

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
	};
}
