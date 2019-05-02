#include "animation_test.h"

void winp::test::animation::run(int cmd_show){
	app::main_object main_app;

	ui::object_collection<winp::window::object> ws;
	ws.set_caption(L"Test Window");
	ws.set_position(30, 30);
	ws.set_size(700, 400);
	ws.create();
	ws.show(cmd_show);

	ws.get_system_menu([&](winp::ui::window_surface::system_menu_type &smn){
		smn.add_object([](winp::menu::separator &item){});

		smn.add_object([&](winp::menu::link_item_with_popup &link){
			link.set_text(L"Animations");
			link.create();

			link.add_object([&](winp::menu::link_item_with_popup &link){
				link.set_text(L"Linear");
				link.create();

				link.add_object([&](winp::menu::action_item &item){
					item.set_text(L"Ease");
					item.events().bind([&](winp::events::menu_item_select &e){
						if (ws.get_x_position() == 30)
							ws.animate_x_position(320, winp::utility::animation_timing::linear::ease, std::chrono::milliseconds(1000));
						else
							ws.animate_x_position(30, winp::utility::animation_timing::linear::ease, std::chrono::milliseconds(1000));
					});
				});
			});

			link.add_object([&](winp::menu::link_item_with_popup &link){
				link.set_text(L"Back");
				link.create();

				link.add_object([&](winp::menu::action_item &item){
					item.set_text(L"Ease In");
					item.events().bind([&](winp::events::menu_item_select &e){
						if (ws.get_x_position() == 30)
							ws.animate_x_position(320, winp::utility::animation_timing::back::ease_in, std::chrono::milliseconds(1000));
						else
							ws.animate_x_position(30, winp::utility::animation_timing::back::ease_in, std::chrono::milliseconds(1000));
					});
				});

				link.add_object([&](winp::menu::action_item &item){
					item.set_text(L"Ease Out");
					item.events().bind([&](winp::events::menu_item_select &e){
						if (ws.get_x_position() == 30)
							ws.animate_x_position(320, winp::utility::animation_timing::back::ease_out, std::chrono::milliseconds(1000));
						else
							ws.animate_x_position(30, winp::utility::animation_timing::back::ease_out, std::chrono::milliseconds(1000));
					});
				});

				link.add_object([&](winp::menu::action_item &item){
					item.set_text(L"Ease In-Out");
					item.events().bind([&](winp::events::menu_item_select &e){
						if (ws.get_x_position() == 30)
							ws.animate_x_position(320, winp::utility::animation_timing::back::ease_in_out, std::chrono::milliseconds(1000));
						else
							ws.animate_x_position(30, winp::utility::animation_timing::back::ease_in_out, std::chrono::milliseconds(1000));
					});
				});
			});

			link.add_object([&](winp::menu::link_item_with_popup &link){
				link.set_text(L"Bounce");
				link.create();

				link.add_object([&](winp::menu::action_item &item){
					item.set_text(L"Ease In");
					item.events().bind([&](winp::events::menu_item_select &e){
						if (ws.get_x_position() == 30)
							ws.animate_x_position(320, winp::utility::animation_timing::bounce::ease_in, std::chrono::milliseconds(1000));
						else
							ws.animate_x_position(30, winp::utility::animation_timing::bounce::ease_in, std::chrono::milliseconds(1000));
					});
				});

				link.add_object([&](winp::menu::action_item &item){
					item.set_text(L"Ease Out");
					item.events().bind([&](winp::events::menu_item_select &e){
						if (ws.get_x_position() == 30)
							ws.animate_x_position(320, winp::utility::animation_timing::bounce::ease_out, std::chrono::milliseconds(1000));
						else
							ws.animate_x_position(30, winp::utility::animation_timing::bounce::ease_out, std::chrono::milliseconds(1000));
					});
				});

				link.add_object([&](winp::menu::action_item &item){
					item.set_text(L"Ease In-Out");
					item.events().bind([&](winp::events::menu_item_select &e){
						if (ws.get_x_position() == 30)
							ws.animate_x_position(320, winp::utility::animation_timing::bounce::ease_in_out, std::chrono::milliseconds(1000));
						else
							ws.animate_x_position(30, winp::utility::animation_timing::bounce::ease_in_out, std::chrono::milliseconds(1000));
					});
				});
			});

			link.add_object([&](winp::menu::link_item_with_popup &link){
				link.set_text(L"Circle");
				link.create();

				link.add_object([&](winp::menu::action_item &item){
					item.set_text(L"Ease In");
					item.events().bind([&](winp::events::menu_item_select &e){
						if (ws.get_x_position() == 30)
							ws.animate_x_position(320, winp::utility::animation_timing::circle::ease_in, std::chrono::milliseconds(1000));
						else
							ws.animate_x_position(30, winp::utility::animation_timing::circle::ease_in, std::chrono::milliseconds(1000));
					});
				});

				link.add_object([&](winp::menu::action_item &item){
					item.set_text(L"Ease Out");
					item.events().bind([&](winp::events::menu_item_select &e){
						if (ws.get_x_position() == 30)
							ws.animate_x_position(320, winp::utility::animation_timing::circle::ease_out, std::chrono::milliseconds(1000));
						else
							ws.animate_x_position(30, winp::utility::animation_timing::circle::ease_out, std::chrono::milliseconds(1000));
					});
				});

				link.add_object([&](winp::menu::action_item &item){
					item.set_text(L"Ease In-Out");
					item.events().bind([&](winp::events::menu_item_select &e){
						if (ws.get_x_position() == 30)
							ws.animate_x_position(320, winp::utility::animation_timing::circle::ease_in_out, std::chrono::milliseconds(1000));
						else
							ws.animate_x_position(30, winp::utility::animation_timing::circle::ease_in_out, std::chrono::milliseconds(1000));
					});
				});
			});

			link.add_object([&](winp::menu::link_item_with_popup &link){
				link.set_text(L"Cubic");
				link.create();

				link.add_object([&](winp::menu::action_item &item){
					item.set_text(L"Ease In");
					item.events().bind([&](winp::events::menu_item_select &e){
						if (ws.get_x_position() == 30)
							ws.animate_x_position(320, winp::utility::animation_timing::cubic::ease_in, std::chrono::milliseconds(1000));
						else
							ws.animate_x_position(30, winp::utility::animation_timing::cubic::ease_in, std::chrono::milliseconds(1000));
					});
				});

				link.add_object([&](winp::menu::action_item &item){
					item.set_text(L"Ease Out");
					item.events().bind([&](winp::events::menu_item_select &e){
						if (ws.get_x_position() == 30)
							ws.animate_x_position(320, winp::utility::animation_timing::cubic::ease_out, std::chrono::milliseconds(1000));
						else
							ws.animate_x_position(30, winp::utility::animation_timing::cubic::ease_out, std::chrono::milliseconds(1000));
					});
				});

				link.add_object([&](winp::menu::action_item &item){
					item.set_text(L"Ease In-Out");
					item.events().bind([&](winp::events::menu_item_select &e){
						if (ws.get_x_position() == 30)
							ws.animate_x_position(320, winp::utility::animation_timing::cubic::ease_in_out, std::chrono::milliseconds(1000));
						else
							ws.animate_x_position(30, winp::utility::animation_timing::cubic::ease_in_out, std::chrono::milliseconds(1000));
					});
				});
			});

			link.add_object([&](winp::menu::link_item_with_popup &link){
				link.set_text(L"Elastic");
				link.create();

				link.add_object([&](winp::menu::action_item &item){
					item.set_text(L"Ease In");
					item.events().bind([&](winp::events::menu_item_select &e){
						if (ws.get_x_position() == 30)
							ws.animate_x_position(320, winp::utility::animation_timing::elastic::ease_in, std::chrono::milliseconds(1000));
						else
							ws.animate_x_position(30, winp::utility::animation_timing::elastic::ease_in, std::chrono::milliseconds(1000));
					});
				});

				link.add_object([&](winp::menu::action_item &item){
					item.set_text(L"Ease Out");
					item.events().bind([&](winp::events::menu_item_select &e){
						if (ws.get_x_position() == 30)
							ws.animate_x_position(320, winp::utility::animation_timing::elastic::ease_out, std::chrono::milliseconds(1000));
						else
							ws.animate_x_position(30, winp::utility::animation_timing::elastic::ease_out, std::chrono::milliseconds(1000));
					});
				});

				link.add_object([&](winp::menu::action_item &item){
					item.set_text(L"Ease In-Out");
					item.events().bind([&](winp::events::menu_item_select &e){
						if (ws.get_x_position() == 30)
							ws.animate_x_position(320, winp::utility::animation_timing::elastic::ease_in_out, std::chrono::milliseconds(1000));
						else
							ws.animate_x_position(30, winp::utility::animation_timing::elastic::ease_in_out, std::chrono::milliseconds(1000));
					});
				});
			});

			link.add_object([&](winp::menu::link_item_with_popup &link){
				link.set_text(L"Exponential");
				link.create();

				link.add_object([&](winp::menu::action_item &item){
					item.set_text(L"Ease In");
					item.events().bind([&](winp::events::menu_item_select &e){
						if (ws.get_x_position() == 30)
							ws.animate_x_position(320, winp::utility::animation_timing::exponential::ease_in, std::chrono::milliseconds(1000));
						else
							ws.animate_x_position(30, winp::utility::animation_timing::exponential::ease_in, std::chrono::milliseconds(1000));
					});
				});

				link.add_object([&](winp::menu::action_item &item){
					item.set_text(L"Ease Out");
					item.events().bind([&](winp::events::menu_item_select &e){
						if (ws.get_x_position() == 30)
							ws.animate_x_position(320, winp::utility::animation_timing::exponential::ease_out, std::chrono::milliseconds(1000));
						else
							ws.animate_x_position(30, winp::utility::animation_timing::exponential::ease_out, std::chrono::milliseconds(1000));
					});
				});

				link.add_object([&](winp::menu::action_item &item){
					item.set_text(L"Ease In-Out");
					item.events().bind([&](winp::events::menu_item_select &e){
						if (ws.get_x_position() == 30)
							ws.animate_x_position(320, winp::utility::animation_timing::exponential::ease_in_out, std::chrono::milliseconds(1000));
						else
							ws.animate_x_position(30, winp::utility::animation_timing::exponential::ease_in_out, std::chrono::milliseconds(1000));
					});
				});
			});

			link.add_object([&](winp::menu::link_item_with_popup &link){
				link.set_text(L"Quadratic");
				link.create();

				link.add_object([&](winp::menu::action_item &item){
					item.set_text(L"Ease In");
					item.events().bind([&](winp::events::menu_item_select &e){
						if (ws.get_x_position() == 30)
							ws.animate_x_position(320, winp::utility::animation_timing::quadratic::ease_in, std::chrono::milliseconds(1000));
						else
							ws.animate_x_position(30, winp::utility::animation_timing::quadratic::ease_in, std::chrono::milliseconds(1000));
					});
				});

				link.add_object([&](winp::menu::action_item &item){
					item.set_text(L"Ease Out");
					item.events().bind([&](winp::events::menu_item_select &e){
						if (ws.get_x_position() == 30)
							ws.animate_x_position(320, winp::utility::animation_timing::quadratic::ease_out, std::chrono::milliseconds(1000));
						else
							ws.animate_x_position(30, winp::utility::animation_timing::quadratic::ease_out, std::chrono::milliseconds(1000));
					});
				});

				link.add_object([&](winp::menu::action_item &item){
					item.set_text(L"Ease In-Out");
					item.events().bind([&](winp::events::menu_item_select &e){
						if (ws.get_x_position() == 30)
							ws.animate_x_position(320, winp::utility::animation_timing::quadratic::ease_in_out, std::chrono::milliseconds(1000));
						else
							ws.animate_x_position(30, winp::utility::animation_timing::quadratic::ease_in_out, std::chrono::milliseconds(1000));
					});
				});
			});

			link.add_object([&](winp::menu::link_item_with_popup &link){
				link.set_text(L"Quart");
				link.create();

				link.add_object([&](winp::menu::action_item &item){
					item.set_text(L"Ease In");
					item.events().bind([&](winp::events::menu_item_select &e){
						if (ws.get_x_position() == 30)
							ws.animate_x_position(320, winp::utility::animation_timing::quart::ease_in, std::chrono::milliseconds(1000));
						else
							ws.animate_x_position(30, winp::utility::animation_timing::quart::ease_in, std::chrono::milliseconds(1000));
					});
				});

				link.add_object([&](winp::menu::action_item &item){
					item.set_text(L"Ease Out");
					item.events().bind([&](winp::events::menu_item_select &e){
						if (ws.get_x_position() == 30)
							ws.animate_x_position(320, winp::utility::animation_timing::quart::ease_out, std::chrono::milliseconds(1000));
						else
							ws.animate_x_position(30, winp::utility::animation_timing::quart::ease_out, std::chrono::milliseconds(1000));
					});
				});

				link.add_object([&](winp::menu::action_item &item){
					item.set_text(L"Ease In-Out");
					item.events().bind([&](winp::events::menu_item_select &e){
						if (ws.get_x_position() == 30)
							ws.animate_x_position(320, winp::utility::animation_timing::quart::ease_in_out, std::chrono::milliseconds(1000));
						else
							ws.animate_x_position(30, winp::utility::animation_timing::quart::ease_in_out, std::chrono::milliseconds(1000));
					});
				});
			});

			link.add_object([&](winp::menu::link_item_with_popup &link){
				link.set_text(L"Quint");
				link.create();

				link.add_object([&](winp::menu::action_item &item){
					item.set_text(L"Ease In");
					item.events().bind([&](winp::events::menu_item_select &e){
						if (ws.get_x_position() == 30)
							ws.animate_x_position(320, winp::utility::animation_timing::quint::ease_in, std::chrono::milliseconds(1000));
						else
							ws.animate_x_position(30, winp::utility::animation_timing::quint::ease_in, std::chrono::milliseconds(1000));
					});
				});

				link.add_object([&](winp::menu::action_item &item){
					item.set_text(L"Ease Out");
					item.events().bind([&](winp::events::menu_item_select &e){
						if (ws.get_x_position() == 30)
							ws.animate_x_position(320, winp::utility::animation_timing::quint::ease_out, std::chrono::milliseconds(1000));
						else
							ws.animate_x_position(30, winp::utility::animation_timing::quint::ease_out, std::chrono::milliseconds(1000));
					});
				});

				link.add_object([&](winp::menu::action_item &item){
					item.set_text(L"Ease In-Out");
					item.events().bind([&](winp::events::menu_item_select &e){
						if (ws.get_x_position() == 30)
							ws.animate_x_position(320, winp::utility::animation_timing::quint::ease_in_out, std::chrono::milliseconds(1000));
						else
							ws.animate_x_position(30, winp::utility::animation_timing::quint::ease_in_out, std::chrono::milliseconds(1000));
					});
				});
			});

			link.add_object([&](winp::menu::link_item_with_popup &link){
				link.set_text(L"Sine");
				link.create();

				link.add_object([&](winp::menu::action_item &item){
					item.set_text(L"Ease In");
					item.events().bind([&](winp::events::menu_item_select &e){
						if (ws.get_x_position() == 30)
							ws.animate_x_position(320, winp::utility::animation_timing::sine::ease_in, std::chrono::milliseconds(1000));
						else
							ws.animate_x_position(30, winp::utility::animation_timing::sine::ease_in, std::chrono::milliseconds(1000));
					});
				});

				link.add_object([&](winp::menu::action_item &item){
					item.set_text(L"Ease Out");
					item.events().bind([&](winp::events::menu_item_select &e){
						if (ws.get_x_position() == 30)
							ws.animate_x_position(320, winp::utility::animation_timing::sine::ease_out, std::chrono::milliseconds(1000));
						else
							ws.animate_x_position(30, winp::utility::animation_timing::sine::ease_out, std::chrono::milliseconds(1000));
					});
				});

				link.add_object([&](winp::menu::action_item &item){
					item.set_text(L"Ease In-Out");
					item.events().bind([&](winp::events::menu_item_select &e){
						if (ws.get_x_position() == 30)
							ws.animate_x_position(320, winp::utility::animation_timing::sine::ease_in_out, std::chrono::milliseconds(1000));
						else
							ws.animate_x_position(30, winp::utility::animation_timing::sine::ease_in_out, std::chrono::milliseconds(1000));
					});
				});
			});
		});
	});
}
