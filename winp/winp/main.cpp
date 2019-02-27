#include "app/app_object.h"
#include "window/window_object.h"
#include "non_window/non_window_object.h"
#include "utility/random_bool_generator.h"

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, LPWSTR cmd_line, int cmd_show){
	winp::app::main_object main_app;

	winp::window::object ws;
	ws.set_caption(L"Test Window");
	ws.set_position(30, 30);
	ws.set_size(600, 400);
	ws.create();
	ws.show(cmd_show);

	winp::non_window::object nwo(ws);
	nwo.set_position(30, 30);
	nwo.set_size(300, 150);
	nwo.set_background_color(D2D1::ColorF::Red);
	nwo.create();

	winp::utility::random_bool_generator rand;
	nwo.events().bind([&](winp::events::position_change &e){
		if (!rand)
			e.prevent_default();
	});

	std::thread([&]{
		std::this_thread::sleep_for(std::chrono::seconds(3));
		nwo.set_width(400);
	}).detach();

	ws.events().bind([&](winp::events::close &e){
		if (!rand)
			e.prevent_default();
	});

	return main_app.get_thread().run();
}
