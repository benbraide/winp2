#include "test/hook_test.h"
#include "test/animation_test.h"
#include "test/non_window_test.h"
#include "test/grid_test.h"
#include "test/menu_test.h"

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, LPWSTR cmd_line, int cmd_show){
	winp::test::menu::run(cmd_show);
	return 0;
}
