#pragma once

#include "../app/app_object.h"
#include "../utility/animation_timing.h"

#include "../window/window_object.h"
#include "../non_window/non_window_object.h"

#include "../ui/ui_io_hooks.h"
#include "../ui/ui_general_hooks.h"

#include "../menu/menu_action_item.h"
#include "../menu/menu_link_item.h"
#include "../menu/menu_separator.h"

namespace winp::test{
	struct animation{
		static void run(int cmd_show = 0);
	};
}
