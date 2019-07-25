#pragma once

#include "../app/app_object.h"

#include "../window/window_object.h"
#include "../control/push_button_control.h"

#include "../ui/ui_io_hooks.h"
#include "../ui/ui_size_and_position_hooks.h"

namespace winp::test{
	struct hook{
		static void run(int cmd_show = 0);
	};
}