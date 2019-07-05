#include "control_test.h"

void winp::test::control::run(int cmd_show){
	winp::window::object ws;
	ws.set_caption(L"Test Window");
	ws.set_position(30, 30);
	ws.set_size(900, 500);
	ws.create();
	ws.show(cmd_show);

	winp::control::tool_tip tip;
	ws.add_object([&](winp::control::tab &tab){
		tab.add_object([&](winp::control::tab_page &page){
			page.set_title(L"Buttons");
		});

		tab.add_object([&](winp::control::tab_page &page){
			page.set_title(L"Tooltip");

			tip.add_object([&](winp::control::tool_tip_item &tool){
				tool.set_target(page);
				tool.set_dimension(540, 18, 270, 207);
				tool.set_text(L"Rect Tool");
			});

			page.add_object([&](winp::control::push_button &btn){
				btn.set_text(L"Push Button");
				btn.set_position(30, 20);
				btn.insert_hook<ui::drag_hook>();

				tip.add_object([&](winp::control::tool_tip_item &tool){
					tool.set_target(btn);
					tool.set_text(L"Object Tool");
				});
			});

			page.add_object([&](winp::control::push_button &btn){
				btn.set_text(L"Push Button");
				btn.set_position(160, 20);
				btn.insert_hook<ui::drag_hook>();

				tip.add_object([&](winp::control::tool_tip_item &tool){
					tool.set_target(btn);
					tool.set_title(L"Tool Title");
					tool.set_text(L"Object Tool");
				});
			});

			page.add_object([&](winp::non_window::rectangle &nwo){
				nwo.set_background_color(D2D1::ColorF::Red);
				nwo.set_position(30, 70);
				nwo.set_size(200, 140);
				nwo.insert_hook<ui::drag_hook>();

				tip.add_object([&](winp::control::tool_tip_item &tool){
					tool.set_target(nwo);
					tool.set_text(L"Non-Window Tool");
				});
			});

			std::thread([&]{
				std::this_thread::sleep_for(std::chrono::seconds(3));
				tab.set_active_page(page);
			}).detach();
		});

		tab.add_object([](winp::control::tab_page &page){
			page.set_title(L"Text IO");
		});

		tab.add_object([](winp::control::tab_page &page){
			page.set_title(L"Last Page");
		});
	});

	app::object::run();
}
