#include "app/app_object.h"

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, LPWSTR cmd_line, int cmd_show){
	winp::app::main_object main_app;
	auto r0 = main_app.get_thread().generate_random_integer(90, 100);
	auto r1 = main_app.get_thread().generate_random_integer(std::numeric_limits<int>::max());
	auto r2 = main_app.get_thread().generate_random_integer<int>();
	return 0;
}
