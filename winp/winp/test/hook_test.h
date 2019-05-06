#pragma once

#include "../app/app_object.h"

#include "../window/window_object.h"
#include "../control/push_button_control.h"

namespace winp::test{
	struct hook{
		static void run(int cmd_show = 0);
	};
}