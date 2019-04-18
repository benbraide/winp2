#include "app/app_object.h"
#include "utility/random_bool_generator.h"

#include "window/window_object.h"
#include "non_window/rectangular_non_window.h"

#include "menu/menu_item_wrapper.h"
#include "menu/menu_link_item_with_popup.h"

#include "control/push_button_control.h"
#include "control/split_button_control.h"
#include "control/check_control.h"
#include "control/control_group.h"
#include "control/label_control.h"
#include "control/tool_tip_control.h"
#include "control/tab_control.h"

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, LPWSTR cmd_line, int cmd_show){
	winp::app::main_object main_app;

	winp::ui::object_collection<winp::window::object> ws;
	ws.set_caption(L"Test Window");
	ws.set_position(30, 30);
	ws.set_size(1600, 900);
	ws.create();
	ws.show(cmd_show);

	ws.events().bind([&](winp::events::context_menu &e){
		e.get_popup().add_object([&](winp::menu::action_item &item){
			if (dynamic_cast<winp::ui::object &>(e.get_target()).is_ancestor(ws))
				item.set_text(L"Offspring Context Menu Action Item");
			else
				item.set_text(L"Context Menu Action Item");

			return winp::ui::add_result_type::confirm;
		});
	});

	ws.events().bind([&](winp::events::allow_context_menu &e){
		e.set_result(true);
	});

	auto &smn = ws.get_system_menu();
	smn.add_object([](winp::menu::separator &item){
		return winp::ui::add_result_type::confirm;
	});

	smn.add_object([](winp::menu::action_item &item){
		item.set_text(L"Custom Action Item");
		return winp::ui::add_result_type::confirm;
	});

	smn.add_object([](winp::menu::link_item_with_popup &link){
		link.set_text(L"Link Item");
		link.create();

		link.add_object([](winp::menu::action_item &item){
			item.set_text(L"Link Action Item");
			item.events().bind([](winp::events::menu_item_select &e){
				auto &target = e.get_target();
				auto &context = e.get_context();
			});
			return winp::ui::add_result_type::confirm;
		});

		return winp::ui::add_result_type::dont_create;
	});

	winp::ui::object_collection<winp::control::tool_tip> ttc;
	ttc.create();

	ws.add_object([](winp::ui::object_collection<winp::control::tab> &tab){
		tab.create();

		tab.insert_hook<winp::ui::parent_fill_hook>();
		tab.insert_hook<winp::ui::placement_hook>(winp::ui::placement_hook::alignment_type::top_left);

		tab.add_object([](winp::ui::object_collection<winp::control::tab_page> &tpg){
			tpg.set_title(L"First Tab");
			tpg.create();

			tpg.get_grid().add_object([](winp::ui::object_collection<winp::grid::row> &row){
				row.add_object([](winp::ui::object_collection<winp::grid::column> &col){
					col.set_background_color(D2D1::ColorF::Red);
					col.add_object([&](winp::ui::object_collection<winp::non_window::rectangle> &nwo){
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

						nwo.set_size(300, 150);
						nwo.set_background_color(D2D1::ColorF::Red);

						nwo.insert_hook<winp::ui::placement_hook>(winp::ui::placement_hook::alignment_type::bottom_right);

						return winp::ui::add_result_type::confirm;
					});

					return winp::ui::add_result_type::confirm;
				});

				row.add_object([](winp::ui::object_collection<winp::grid::proportional_column> &col){
					col.set_proportion(0.6f);
					col.set_background_color(D2D1::ColorF::Green);

					col.add_object([](winp::ui::object_collection<winp::window::object> &wsc){
						wsc.set_caption(L"Child Window");
						wsc.create();
						wsc.show();

						wsc.insert_hook<winp::ui::placement_hook>(winp::ui::placement_hook::alignment_type::center);
						wsc.insert_hook<winp::ui::parent_fill_hook>(D2D1_SIZE_F{ 0.4f, 0.5f });

						return winp::ui::add_result_type::confirm;
					});

					return winp::ui::add_result_type::confirm;
				});

				return winp::ui::add_result_type::confirm;
			});

			tpg.get_grid().add_object([](winp::ui::object_collection<winp::grid::row> &row){
				row.create();
				row.add_object([](winp::ui::object_collection<winp::grid::column> &col){
					col.set_background_color(D2D1::ColorF::Blue);
					return winp::ui::add_result_type::confirm;
				});
				return winp::ui::add_result_type::confirm;
			});

			return winp::ui::add_result_type::dont_create;
		});

		tab.add_object([](winp::ui::object_collection<winp::control::tab_page> &tpg){
			tpg.set_title(L"Second Tab");
			return winp::ui::add_result_type::confirm;
		});

		return winp::ui::add_result_type::dont_create;
	});

	/*winp::utility::random_bool_generator rand;
	ws.add_object([&](winp::ui::object_collection<winp::non_window::rectangle> &nwo){
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

		nwo.add_object([&](winp::control::push_button &btn){
			btn.set_position(10, 10);
			btn.set_text(L"Button");

			btn.events().bind([&](winp::events::click &e){
				auto &t = e.get_target();
			});

			btn.create();
			ttc.add_object([&](winp::control::inplace_tool_tip_item &tti){
				tti.set_target(btn);
				tti.set_text(L"Push button label tip text");
				return winp::ui::add_result_type::confirm;
			});

			return winp::ui::add_result_type::dont_create;
		});

		std::thread([&]{
			std::this_thread::sleep_for(std::chrono::seconds(3));
			nwo.set_width(400);
		}).detach();

		return winp::ui::add_result_type::confirm;
	});

	ws.add_object([&](winp::control::split_button &btn){
		btn.set_position(350, 40);
		btn.set_text(L"Split Button");

		btn.events().bind<winp::events::allow_context_menu>([&](){
			auto &e = winp::app::object::get_current_thread()->get_current_event();
			return true;
		});

		btn.events().bind([&](winp::events::context_menu &e){
			e.get_popup().add_object([&](winp::menu::action_item &item){
				item.set_text(L"Split Button Action Item");
				return winp::ui::add_result_type::confirm;
			});

			e.get_popup().add_object([&](winp::menu::action_item &item){
				item.set_text(L"Split Button Action Item 2");
				return winp::ui::add_result_type::confirm;
			});
		});

		btn.events().bind([&](winp::events::click &e){
			auto &t = e.get_target();
		});

		btn.create();
		ttc.add_object([&](winp::control::tool_tip_item &tti){
			tti.set_target(btn);
			tti.set_text(L"Split button tip text");
			return winp::ui::add_result_type::confirm;
		});

		return winp::ui::add_result_type::dont_create;
	});

	ws.add_object([&](winp::control::label &cmd){
		cmd.set_position(350, 70);
		cmd.set_text(L"Label Text");

		cmd.events().bind([&](winp::events::mouse_move &e){
			auto &t = e.get_target();
		});

		cmd.create();
		ttc.add_object([&](winp::control::tool_tip_item &tti){
			tti.set_target(cmd);
			tti.set_text(L"This should be the label tip text");
			tti.set_title(L"Label tip title");
			return winp::ui::add_result_type::confirm;
		});

		return winp::ui::add_result_type::dont_create;
	});

	ws.events().bind([&](winp::events::close &e){
		//if (!rand)
			//e.prevent_default();
	});

	ws.events().bind([&](winp::events::mouse_leave &e){
		e.prevent_default();
	});

	winp::ui::object_collection<winp::window::object> wsc;
	wsc.set_parent(ws);
	wsc.set_caption(L"Child Window");
	wsc.set_position(30, 200);
	wsc.set_size(300, 120);
	wsc.create();
	wsc.show();

	wsc.events().bind([&](winp::events::mouse_leave &e){
		e.prevent_default();
	});

	wsc.add_object([&](winp::control::check &chk){
		chk.set_position(10, 10);
		chk.set_text(L"Check");

		chk.events().bind([&](winp::events::click &e){
			auto &t = e.get_target();
		});

		chk.events().bind([&](winp::events::item_check &e){
			auto ic = e.is_checked();
			auto &t = e.get_target();
		});

		return winp::ui::add_result_type::confirm;
	});

	wsc.add_object([&](winp::ui::object_collection<winp::control::radio_group> &grp){
		grp.add_object([&](winp::control::check &chk){
			chk.set_position(10, 30);
			chk.set_text(L"Radio 1");

			chk.events().bind([&](winp::events::click &e){
				auto &t = e.get_target();
			});

			chk.events().bind([&](winp::events::item_check &e){
				auto ic = e.is_checked();
				auto &t = e.get_target();
			});

			return winp::ui::add_result_type::confirm;
		});

		grp.add_object([&](winp::control::check &chk){
			chk.set_position(10, 50);
			chk.set_text(L"Radio 2");

			chk.events().bind([&](winp::events::click &e){
				auto &t = e.get_target();
			});

			chk.events().bind([&](winp::events::item_check &e){
				auto ic = e.is_checked();
				auto &t = e.get_target();
			});

			std::thread([&]{
				std::this_thread::sleep_for(std::chrono::seconds(7));
				chk.click();
			}).detach();

			return winp::ui::add_result_type::confirm;
		});

		return winp::ui::add_result_type::confirm;
	});

	wsc.add_object([&](winp::control::three_state_check &chk){
		chk.set_position(90, 10);
		chk.set_text(L"Three State Check");

		chk.events().bind([&](winp::events::click &e){
			auto &t = e.get_target();
		});

		chk.events().bind([&](winp::events::item_check &e){
			auto ic = e.is_checked();
			auto &t = e.get_target();
		});

		return winp::ui::add_result_type::confirm;
	});*/

	return main_app.get_thread().run();
}
