#include "app/app_object.h"
#include "ui/ui_window_surface.h"

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, LPWSTR cmd_line, int cmd_show){
	winp::app::main_object main_app;

	winp::ui::window_surface ws;
	{//Properties
		ws.set_position(30, 30);
		ws.set_size(600, 400);
		ws.set_styles(WS_OVERLAPPEDWINDOW, false);
		ws.create();
		ws.show(cmd_show);
	}

	return main_app.get_thread().run();
}
