#pragma once

#include "../app/app_object.h"
#include "../utility/animation_timing.h"

#include "../window/window_object.h"
#include "../menu/menu_item_wrapper.h"
#include "../menu/menu_link_item_with_popup.h"

namespace winp::test{
	struct animation{
		static void run(int cmd_show = 0);
	};
}
