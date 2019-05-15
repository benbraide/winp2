#include "non_window_test.h"

void winp::test::non_window::run(int cmd_show){
	app::main_object main_app;

	ui::object_collection<winp::window::object> ws;
	ws.set_caption(L"Test Window");
	ws.set_position(30, 30);
	ws.set_size(900, 500);
	ws.create();
	ws.show(cmd_show);

	ws.add_object([&](winp::non_window::rectangle &nwo){
		nwo.events().bind([&](events::paint &e){
			auto &size = dynamic_cast<ui::surface *>(&e.get_context())->get_size();
			e.draw_lines(POINT{ 10, 0 }, POINT{ 10, size.cy }, POINT{ 10, 0 }, (size.cx / 10), D2D1::ColorF::White);
			e.draw_lines(POINT{ 0, 10 }, POINT{ size.cx, 10 }, POINT{ 0, 10 }, (size.cy / 10), D2D1::ColorF::White);
		});

		nwo.set_position(30, 30);
		nwo.set_size(200, 100);
		nwo.set_background_color(D2D1::ColorF::Red);
	});

	ws.add_object([&](winp::non_window::round_rectangle &nwo){
		nwo.events().bind([&](events::paint &e){
			auto &size = dynamic_cast<ui::surface *>(&e.get_context())->get_size();
			e.draw_lines(POINT{ 10, 0 }, POINT{ 10, size.cy }, POINT{ 10, 0 }, (size.cx / 10), D2D1::ColorF::White);
			e.draw_lines(POINT{ 0, 10 }, POINT{ size.cx, 10 }, POINT{ 0, 10 }, (size.cy / 10), D2D1::ColorF::White);
		});

		nwo.set_position(260, 30);
		nwo.set_size(200, 100);
		nwo.set_background_color(D2D1::ColorF::Green);
		nwo.set_border_curve_size(20, 20);
	});

	ws.add_object([&](winp::non_window::ellipsis &nwo){
		nwo.events().bind([&](events::paint &e){
			auto &size = dynamic_cast<ui::surface *>(&e.get_context())->get_size();
			e.draw_lines(POINT{ 10, 0 }, POINT{ 10, size.cy }, POINT{ 10, 0 }, (size.cx / 10), D2D1::ColorF::White);
			e.draw_lines(POINT{ 0, 10 }, POINT{ size.cx, 10 }, POINT{ 0, 10 }, (size.cy / 10), D2D1::ColorF::White);
		});

		nwo.set_position(490, 30);
		nwo.set_size(200, 100);
		nwo.set_background_color(D2D1::ColorF::Blue);
	});

	ws.add_object([&](winp::non_window::circle &nwo){
		nwo.events().bind([&](events::paint &e){
			auto &size = dynamic_cast<ui::surface *>(&e.get_context())->get_size();
			e.draw_lines(POINT{ 10, 0 }, POINT{ 10, size.cy }, POINT{ 10, 0 }, (size.cx / 10), D2D1::ColorF::White);
			e.draw_lines(POINT{ 0, 10 }, POINT{ size.cx, 10 }, POINT{ 0, 10 }, (size.cy / 10), D2D1::ColorF::White);
		});

		nwo.set_position(720, 30);
		nwo.set_radius(60);
		nwo.set_background_color(D2D1::ColorF::Gray);
	});

	ws.add_object([&](winp::non_window::custom &nwo){
		nwo.events().bind([&](events::paint &e){
			auto &size = dynamic_cast<ui::surface *>(&e.get_context())->get_size();
			e.draw_lines(POINT{ 10, 0 }, POINT{ 10, size.cy }, POINT{ 10, 0 }, (size.cx / 10), D2D1::ColorF::White);
			e.draw_lines(POINT{ 0, 10 }, POINT{ size.cx, 10 }, POINT{ 0, 10 }, (size.cy / 10), D2D1::ColorF::White);
		});

		nwo.events().bind([&](events::create_non_window_handle &e){
			POINT vertices[] = { POINT{ 0, 0 }, POINT{ 200, 0 }, POINT{ 200, 100 } };
			return CreatePolygonRgn(vertices, 3, WINDING);
		});

		nwo.set_position(30, 150);
		nwo.set_background_color(D2D1::ColorF::Purple);
	});

	main_app.get_thread().run();
}
