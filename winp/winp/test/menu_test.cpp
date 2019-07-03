#include "menu_test.h"

void winp::test::menu::run(int cmd_show){
	winp::window::object ws;
	ws.get_menu_bar([](winp::menu::bar &mb){
		mb.add_object([](winp::menu::action_item &item){
			item.set_text(L"First MBI");
			item.events().bind([&](events::menu_item_select &e){
				
			});
		});

		mb.add_object([](winp::menu::action_item &item){
			item.set_text(L"Second MBI");
			item.events().bind([&](events::menu_item_select &e){

			});
		});

		mb.add_object([](winp::menu::link_item &link){
			link.set_text(L"MBI Link");

			link.add_object([](winp::menu::action_item &item){
				item.set_text(L"First MBLI");
				item.events().bind([](events::menu_item_select &e){

				});
			});

			link.add_object([](winp::menu::action_item &item){
				item.set_text(L"Second MBLI");
				item.events().bind([](events::menu_item_select &e){

				});
			});
		});

		mb.add_object([](winp::menu::action_item &item){
			item.set_text(L"Last MBI");
			item.events().bind([](events::menu_item_select &e){

			});
		});
	});

	ws.insert_hook<ui::system_menu_as_context_menu>();
	ws.set_caption(L"Test Window");
	ws.set_position(30, 30);
	ws.set_size(900, 500);
	ws.create();
	ws.show(cmd_show);

	ws.add_object([](non_window::rectangle &nwo){
		nwo.insert_hook<ui::io_hook>();
		nwo.set_position(30, 30);
		nwo.set_size(200, 100);
		nwo.set_background_color(D2D1::ColorF::Red);

		nwo.events().bind([](events::context_menu &e){
			e.get_popup().add_object([&](winp::menu::action_item &item){
				item.set_text(L"NW CM First Item");
				item.events().bind([](events::menu_item_select &e){

				});
			});

			e.get_popup().add_object([](winp::menu::action_item &item){
				item.set_text(L"NW CM Last Item");
				item.events().bind([](events::menu_item_select &e){

				});
			});
		});

		nwo.events().bind([](events::modify_context_menu &e){
			e.get_popup().add_object([&](winp::menu::separator &item){});

			e.get_popup().add_object([](winp::menu::action_item &item){
				item.set_text(L"NW CM Appended Item");
				item.events().bind([](events::menu_item_select &e){

				});
			});
		});
	});

	app::object::run();
}
