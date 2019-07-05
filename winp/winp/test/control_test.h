#pragma once

#include "../app/app_object.h"

#include "../window/window_object.h"
#include "../non_window/rectangular_non_window.h"

#include "../control/push_button_control.h"
#include "../control/split_button_control.h"
#include "../control/command_link_control.h"
#include "../control/check_control.h"
#include "../control/label_control.h"
#include "../control/tab_control.h"
#include "../control/tool_tip_control.h"
#include "../control/control_group.h"

namespace winp::test{
	struct control{
		static void run(int cmd_show = 0);
	};
}
