#pragma once

#include "../ui/ui_non_window_surface.h"

namespace winp::non_window{
	class rectangle : public ui::non_window_surface{
	public:
		rectangle();

		explicit rectangle(tree &parent);

		rectangle(tree &parent, std::size_t index);

		virtual ~rectangle();
	};
}
