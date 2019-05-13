#pragma once

#include "../app/app_object.h"

#include "../window/window_object.h"
#include "../non_window/rectangular_non_window.h"
#include "../non_window/round_rectangular_non_window.h"
#include "../non_window/elliptical_non_window.h"
#include "../non_window/custom_non_window.h"

namespace winp::test{
	struct non_window{
		static void run(int cmd_show = 0);
	};
}
