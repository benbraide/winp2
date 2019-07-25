#pragma once

#include "../ui/ui_non_window_surface.h"
#include "../ui/ui_non_window_hooks.h"

namespace winp::non_window{
	class object : public ui::non_window_surface{
	public:
		object();

		explicit object(tree &parent);

		object(tree &parent, std::size_t index);

		virtual ~object();
	};
}
