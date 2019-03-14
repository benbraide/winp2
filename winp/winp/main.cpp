#include "app/app_object.h"
#include "window/window_object.h"
#include "non_window/rectangular_non_window.h"
#include "utility/random_bool_generator.h"

#include "menu/menu_item_wrapper.h"
#include "menu/menu_link_item_with_popup.h"

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, LPWSTR cmd_line, int cmd_show){
	winp::app::main_object main_app;

	winp::ui::object_collection<winp::window::object> ws;
	ws.set_caption(L"Test Window");
	ws.set_position(30, 30);
	ws.set_size(600, 400);
	ws.create();
	ws.show(cmd_show);

	auto &smn = ws.get_system_menu();
	smn.add_object([](winp::menu::separator &item){
		return winp::ui::add_result_type::confirm;
	});

	smn.add_object([](winp::menu::action_item &item){
		item.set_text(L"Custom Action Item");
		return winp::ui::add_result_type::confirm;
	});

	smn.add_object([](winp::menu::link_item_with_popup &item){
		item.set_text(L"Link Item");
		item.create();

		item.add_object([](winp::menu::action_item &item){
			item.set_text(L"Link Action Item");
			return winp::ui::add_result_type::confirm;
		});

		return winp::ui::add_result_type::dont_create;
	});

	winp::window::object wsc;
	wsc.set_parent(ws);
	wsc.set_caption(L"Child Window");
	wsc.set_position(30, 200);
	wsc.set_size(300, 120);
	wsc.create();
	wsc.show();

	winp::utility::random_bool_generator rand;
	ws.add_object([&](winp::non_window::rectangle &nwo){
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
		//nwo.create();

		nwo.events().bind([&](winp::events::position_change &e){
			if (!rand)
				e.prevent_default();
		});

		std::thread([&]{
			std::this_thread::sleep_for(std::chrono::seconds(3));
			nwo.set_width(400);
		}).detach();

		return winp::ui::add_result_type::confirm;
	});

	ws.events().bind([&](winp::events::close &e){
		if (!rand)
			e.prevent_default();
	});

	ws.events().bind([&](winp::events::mouse_leave &e){
		e.prevent_default();
	});

	wsc.events().bind([&](winp::events::mouse_leave &e){
		e.prevent_default();
	});

	return main_app.get_thread().run();
}
