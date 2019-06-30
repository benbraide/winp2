#pragma once

#include "../app/app_object.h"
#include "../utility/animation_timing.h"

#include "../window/window_object.h"
#include "../non_window/rectangular_non_window.h"

#include "../menu/menu_action_item.h"
#include "../menu/menu_link_item.h"
#include "../menu/menu_separator.h"

namespace winp::test{
	struct animation{
		static void run(int cmd_show = 0);
	};
}
