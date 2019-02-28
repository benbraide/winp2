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
	nwo.events().bind([&](winp::events::paint &e){
		e.begin();
		if (auto drawer = e.get_render_target(); drawer != nullptr){
			e.get_color_brush()->SetColor(D2D1::ColorF(D2D1::ColorF::White));
			auto size = dynamic_cast<winp::ui::surface *>(&e.get_context())->get_size();

			for (auto step = 10; step < size.cx; step += 10)
				drawer->DrawLine(D2D1::Point2F((float)step, 0.f), D2D1::Point2F((float)step, (float)size.cy), e.get_color_brush());
			for (auto step = 10; step < size.cy; step += 10)
				drawer->DrawLine(D2D1::Point2F(0.f, (float)step), D2D1::Point2F((float)size.cx, (float)step), e.get_color_brush());
		}
	});

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
