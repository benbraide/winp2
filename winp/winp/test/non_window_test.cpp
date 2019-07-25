#include "non_window_test.h"

void winp::test::non_window::run(int cmd_show){
	winp::window::object ws;
	ws.set_caption(L"Test Window");
	ws.set_position(30, 30);
	ws.set_size(900, 500);
	ws.create();
	ws.show(cmd_show);

	ws.add_object([&](winp::non_window::object &rect){
		rect.events().bind([&](events::paint &e){
			auto &size = dynamic_cast<ui::surface *>(&e.get_context())->get_size();
			e.draw_lines(POINT{ 10, 0 }, POINT{ 10, size.cy }, POINT{ 10, 0 }, (size.cx / 10), D2D1::ColorF::White);
			e.draw_lines(POINT{ 0, 10 }, POINT{ size.cx, 10 }, POINT{ 0, 10 }, (size.cy / 10), D2D1::ColorF::White);
		});

		rect.insert_hook<ui::drag_hook>();
		rect.set_position(30, 30);
		rect.set_size(200, 100);
		rect.set_background_color(D2D1::ColorF::Red);
	});

	ws.add_object([&](winp::non_window::object &round_rect){
		round_rect.events().bind([&](events::paint &e){
			auto &size = dynamic_cast<ui::surface *>(&e.get_context())->get_size();
			e.draw_lines(POINT{ 10, 0 }, POINT{ 10, size.cy }, POINT{ 10, 0 }, (size.cx / 10), D2D1::ColorF::White);
			e.draw_lines(POINT{ 0, 10 }, POINT{ size.cx, 10 }, POINT{ 0, 10 }, (size.cy / 10), D2D1::ColorF::White);
		});

		round_rect.insert_hook<ui::drag_hook>();
		round_rect.insert_hook<ui::round_rectangular_non_window_hook>(20, 20);

		round_rect.set_position(260, 30);
		round_rect.set_size(200, 100);
		round_rect.set_background_color(D2D1::ColorF::Green);
	});

	ws.add_object([&](winp::non_window::object &ellipsis){
		ellipsis.events().bind([&](events::paint &e){
			auto &size = dynamic_cast<ui::surface *>(&e.get_context())->get_size();
			e.draw_lines(POINT{ 10, 0 }, POINT{ 10, size.cy }, POINT{ 10, 0 }, (size.cx / 10), D2D1::ColorF::White);
			e.draw_lines(POINT{ 0, 10 }, POINT{ size.cx, 10 }, POINT{ 0, 10 }, (size.cy / 10), D2D1::ColorF::White);
		});

		ellipsis.insert_hook<ui::drag_hook>();
		ellipsis.insert_hook<ui::elliptical_non_window_hook>();

		ellipsis.set_position(490, 30);
		ellipsis.set_size(200, 100);
		ellipsis.set_background_color(D2D1::ColorF::Blue);
	});

	ws.add_object([&](winp::non_window::object &circle){
		circle.events().bind([&](events::paint &e){
			auto &size = dynamic_cast<ui::surface *>(&e.get_context())->get_size();
			e.draw_lines(POINT{ 10, 0 }, POINT{ 10, size.cy }, POINT{ 10, 0 }, (size.cx / 10), D2D1::ColorF::White);
			e.draw_lines(POINT{ 0, 10 }, POINT{ size.cx, 10 }, POINT{ 0, 10 }, (size.cy / 10), D2D1::ColorF::White);
		});

		circle.insert_hook<ui::drag_hook>();
		circle.insert_hook<ui::elliptical_non_window_hook>();

		circle.set_position(720, 30);
		circle.set_size(100, 100);
		circle.set_background_color(D2D1::ColorF::Gray);
	});

	ws.add_object([&](winp::non_window::object &custom){
		custom.events().bind([&](events::paint &e){
			auto &size = dynamic_cast<ui::surface *>(&e.get_context())->get_size();
			e.draw_lines(POINT{ 10, 0 }, POINT{ 10, size.cy }, POINT{ 10, 0 }, (size.cx / 10), D2D1::ColorF::White);
			e.draw_lines(POINT{ 0, 10 }, POINT{ size.cx, 10 }, POINT{ 0, 10 }, (size.cy / 10), D2D1::ColorF::White);
		});

		custom.events().bind([&](events::create_non_window_handle &e){
			e.prevent_default();
			e.stop_propagation();

			POINT vertices[] = { POINT{ 0, 0 }, POINT{ 200, 0 }, POINT{ 200, 100 } };
			return CreatePolygonRgn(vertices, 3, WINDING);
		});

		custom.insert_hook<ui::drag_hook>();
		custom.set_position(30, 150);
		custom.set_background_color(D2D1::ColorF::Purple);
	});

	app::object::run();
}
