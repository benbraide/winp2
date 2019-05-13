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
			e.begin();
			if (auto drawer = e.get_render_target(); drawer != nullptr){
				e.get_color_brush()->SetColor(D2D1::ColorF(D2D1::ColorF::White));
				auto size = dynamic_cast<ui::surface *>(&e.get_context())->get_size();

				for (auto step = 10; step < size.cx; step += 10)
					drawer->DrawLine(D2D1::Point2F((float)step, 0.f), D2D1::Point2F((float)step, (float)size.cy), e.get_color_brush());
				for (auto step = 10; step < size.cy; step += 10)
					drawer->DrawLine(D2D1::Point2F(0.f, (float)step), D2D1::Point2F((float)size.cx, (float)step), e.get_color_brush());
			}
		});

		nwo.set_position(30, 30);
		nwo.set_size(200, 100);
		nwo.set_background_color(D2D1::ColorF::Red);
	});

	ws.add_object([&](winp::non_window::round_rectangle &nwo){
		nwo.events().bind([&](events::paint &e){
			e.begin();
			if (auto drawer = e.get_render_target(); drawer != nullptr){
				e.get_color_brush()->SetColor(D2D1::ColorF(D2D1::ColorF::White));
				auto size = dynamic_cast<ui::surface *>(&e.get_context())->get_size();

				for (auto step = 10; step < size.cx; step += 10)
					drawer->DrawLine(D2D1::Point2F((float)step, 0.f), D2D1::Point2F((float)step, (float)size.cy), e.get_color_brush());
				for (auto step = 10; step < size.cy; step += 10)
					drawer->DrawLine(D2D1::Point2F(0.f, (float)step), D2D1::Point2F((float)size.cx, (float)step), e.get_color_brush());
			}
		});

		nwo.set_position(260, 30);
		nwo.set_size(200, 100);
		nwo.set_background_color(D2D1::ColorF::Green);
		nwo.set_border_curve_size(20, 20);
	});

	ws.add_object([&](winp::non_window::ellipsis &nwo){
		nwo.events().bind([&](events::paint &e){
			e.begin();
			if (auto drawer = e.get_render_target(); drawer != nullptr){
				e.get_color_brush()->SetColor(D2D1::ColorF(D2D1::ColorF::White));
				auto size = dynamic_cast<ui::surface *>(&e.get_context())->get_size();

				for (auto step = 10; step < size.cx; step += 10)
					drawer->DrawLine(D2D1::Point2F((float)step, 0.f), D2D1::Point2F((float)step, (float)size.cy), e.get_color_brush());
				for (auto step = 10; step < size.cy; step += 10)
					drawer->DrawLine(D2D1::Point2F(0.f, (float)step), D2D1::Point2F((float)size.cx, (float)step), e.get_color_brush());
			}
		});

		nwo.set_position(490, 30);
		nwo.set_size(200, 100);
		nwo.set_background_color(D2D1::ColorF::Blue);
	});

	ws.add_object([&](winp::non_window::circle &nwo){
		nwo.events().bind([&](events::paint &e){
			e.begin();
			if (auto drawer = e.get_render_target(); drawer != nullptr){
				e.get_color_brush()->SetColor(D2D1::ColorF(D2D1::ColorF::White));
				auto size = dynamic_cast<ui::surface *>(&e.get_context())->get_size();

				for (auto step = 10; step < size.cx; step += 10)
					drawer->DrawLine(D2D1::Point2F((float)step, 0.f), D2D1::Point2F((float)step, (float)size.cy), e.get_color_brush());
				for (auto step = 10; step < size.cy; step += 10)
					drawer->DrawLine(D2D1::Point2F(0.f, (float)step), D2D1::Point2F((float)size.cx, (float)step), e.get_color_brush());
			}
		});

		nwo.set_position(720, 30);
		nwo.set_radius(60);
		nwo.set_background_color(D2D1::ColorF::Gray);
	});

	ws.add_object([&](winp::non_window::custom &nwo){
		nwo.events().bind([&](events::paint &e){
			e.begin();
			if (auto drawer = e.get_render_target(); drawer != nullptr){
				e.get_color_brush()->SetColor(D2D1::ColorF(D2D1::ColorF::White));
				auto size = dynamic_cast<ui::surface *>(&e.get_context())->get_size();

				for (auto step = 10; step < size.cx; step += 10)
					drawer->DrawLine(D2D1::Point2F((float)step, 0.f), D2D1::Point2F((float)step, (float)size.cy), e.get_color_brush());
				for (auto step = 10; step < size.cy; step += 10)
					drawer->DrawLine(D2D1::Point2F(0.f, (float)step), D2D1::Point2F((float)size.cx, (float)step), e.get_color_brush());
			}
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
