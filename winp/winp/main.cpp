#include "app/app_object.h"
#include "window/window_object.h"
#include "utility/random_bool_generator.h"

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, LPWSTR cmd_line, int cmd_show){
	winp::app::main_object main_app;

	winp::window::object ws;
	{//Properties
		ws.set_caption(L"Test Window");
		ws.set_position(30, 30);
		ws.set_size(600, 400);
		ws.create();
		ws.show(cmd_show);
	}

	winp::utility::random_bool_generator rand;
	ws.events().bind([&](winp::events::close &e){
		if (!rand)
			e.prevent_default();
	});

	return main_app.get_thread().run();
}
