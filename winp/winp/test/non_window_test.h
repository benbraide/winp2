#pragma once

#include "../app/app_object.h"

#include "../window/window_object.h"
#include "../non_window/non_window_object.h"

#include "../ui/ui_io_hooks.h"

namespace winp::test{
	struct non_window{
		static void run(int cmd_show = 0);
	};
}
