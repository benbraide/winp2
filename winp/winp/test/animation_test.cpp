#include "animation_test.h"

void winp::test::animation::run(int cmd_show){
	winp::window::object ws;
	ws.insert_hook<ui::system_menu_as_context_menu>();
	ws.set_caption(L"Test Window");
	ws.set_position(30, 30);
	ws.set_size(900, 500);
	ws.create();
	ws.show(cmd_show);
	ws.insert_hook<ui::fullscreen_hook>();

	auto nws = ws.add_object([&](non_window::object &nwo){
		nwo.set_position(30, 30);
		nwo.set_size(200, 100);
		
		nwo.events().bind([&](events::animation &e){
			if (e.is_type<D2D1_COLOR_F>() && e.get_progress() == events::animation::progress_type::end){
				if (ui::visible_surface::compare_colors(nwo.get_background_color(), D2D1::ColorF(D2D1::ColorF::Red)))
					nwo.set_background_color(D2D1::ColorF::Blue);
				else if (ui::visible_surface::compare_colors(nwo.get_background_color(), D2D1::ColorF(D2D1::ColorF::Blue)))
					nwo.set_background_color(D2D1::ColorF::White);
				else
					nwo.set_background_color(D2D1::ColorF::Red);
			}
		});

		nwo.insert_hook<ui::auto_hide_cursor_hook>();
	});

	nws->insert_hook<ui::drag_hook>();
	auto hk = nws->insert_hook<ui::animation_hook>(std::chrono::milliseconds(3000));
	if (hk == nullptr)
		return;

	nws->set_background_color(D2D1::ColorF::Red);
	ws.get_system_menu([&](ui::window_surface::popup_menu_type &smn){
		smn.add_object([](menu::separator &item){});

		smn.add_object([&](menu::link_item &link){
			link.set_text(L"Animations");

			link.add_object([&](menu::link_item &link){
				link.set_text(L"Linear");
				link.add_object([&](menu::action_item &item){
					item.set_text(L"Ease");
					item.events().bind([&](events::menu_item_select &e){
						hk->set_easing<POINT>(utility::animation_timing::linear::ease);
						if (nws->get_x_position() == 30)
							nws->set_x_position(320);
						else
							nws->set_x_position(30);
					});
				});
			});

			link.add_object([&](menu::link_item &link){
				link.set_text(L"Back");
				link.add_object([&](menu::action_item &item){
					item.set_text(L"Ease In");
					item.events().bind([&](events::menu_item_select &e){
						hk->set_easing<POINT>(utility::animation_timing::back::ease_in);
						if (nws->get_x_position() == 30)
							nws->set_x_position(320);
						else
							nws->set_x_position(30);
					});
				});

				link.add_object([&](menu::action_item &item){
					item.set_text(L"Ease Out");
					item.events().bind([&](events::menu_item_select &e){
						hk->set_easing<POINT>(utility::animation_timing::back::ease_out);
						if (nws->get_x_position() == 30)
							nws->set_x_position(320);
						else
							nws->set_x_position(30);
					});
				});

				link.add_object([&](menu::action_item &item){
					item.set_text(L"Ease In-Out");
					item.events().bind([&](events::menu_item_select &e){
						hk->set_easing<POINT>(utility::animation_timing::back::ease_in_out);
						if (nws->get_x_position() == 30)
							nws->set_x_position(320);
						else
							nws->set_x_position(30);
					});
				});
			});

			link.add_object([&](menu::link_item &link){
				link.set_text(L"Bounce");
				link.add_object([&](menu::action_item &item){
					item.set_text(L"Ease In");
					item.events().bind([&](events::menu_item_select &e){
						hk->set_easing<POINT>(utility::animation_timing::bounce::ease_in);
						if (nws->get_x_position() == 30)
							nws->set_x_position(320);
						else
							nws->set_x_position(30);
					});
				});

				link.add_object([&](menu::action_item &item){
					item.set_text(L"Ease Out");
					item.events().bind([&](events::menu_item_select &e){
						hk->set_easing<POINT>(utility::animation_timing::bounce::ease_out);
						if (nws->get_x_position() == 30)
							nws->set_x_position(320);
						else
							nws->set_x_position(30);
					});
				});

				link.add_object([&](menu::action_item &item){
					item.set_text(L"Ease In-Out");
					item.events().bind([&](events::menu_item_select &e){
						hk->set_easing<POINT>(utility::animation_timing::bounce::ease_in_out);
						if (nws->get_x_position() == 30)
							nws->set_x_position(320);
						else
							nws->set_x_position(30);
					});
				});
			});

			link.add_object([&](menu::link_item &link){
				link.set_text(L"Circle");
				link.add_object([&](menu::action_item &item){
					item.set_text(L"Ease In");
					item.events().bind([&](events::menu_item_select &e){
						hk->set_easing<POINT>(utility::animation_timing::circle::ease_in);
						if (nws->get_x_position() == 30)
							nws->set_x_position(320);
						else
							nws->set_x_position(30);
					});
				});

				link.add_object([&](menu::action_item &item){
					item.set_text(L"Ease Out");
					item.events().bind([&](events::menu_item_select &e){
						hk->set_easing<POINT>(utility::animation_timing::circle::ease_out);
						if (nws->get_x_position() == 30)
							nws->set_x_position(320);
						else
							nws->set_x_position(30);
					});
				});

				link.add_object([&](menu::action_item &item){
					item.set_text(L"Ease In-Out");
					item.events().bind([&](events::menu_item_select &e){
						hk->set_easing<POINT>(utility::animation_timing::circle::ease_in_out);
						if (nws->get_x_position() == 30)
							nws->set_x_position(320);
						else
							nws->set_x_position(30);
					});
				});
			});

			link.add_object([&](menu::link_item &link){
				link.set_text(L"Cubic");
				link.add_object([&](menu::action_item &item){
					item.set_text(L"Ease In");
					item.events().bind([&](events::menu_item_select &e){
						hk->set_easing<POINT>(utility::animation_timing::cubic::ease_in);
						if (nws->get_x_position() == 30)
							nws->set_x_position(320);
						else
							nws->set_x_position(30);
					});
				});

				link.add_object([&](menu::action_item &item){
					item.set_text(L"Ease Out");
					item.events().bind([&](events::menu_item_select &e){
						hk->set_easing<POINT>(utility::animation_timing::cubic::ease_out);
						if (nws->get_x_position() == 30)
							nws->set_x_position(320);
						else
							nws->set_x_position(30);
					});
				});

				link.add_object([&](menu::action_item &item){
					item.set_text(L"Ease In-Out");
					item.events().bind([&](events::menu_item_select &e){
						hk->set_easing<POINT>(utility::animation_timing::cubic::ease_in_out);
						if (nws->get_x_position() == 30)
							nws->set_x_position(320);
						else
							nws->set_x_position(30);
					});
				});
			});

			link.add_object([&](menu::link_item &link){
				link.set_text(L"Elastic");
				link.add_object([&](menu::action_item &item){
					item.set_text(L"Ease In");
					item.events().bind([&](events::menu_item_select &e){
						hk->set_easing<POINT>(utility::animation_timing::elastic::ease_in);
						if (nws->get_x_position() == 30)
							nws->set_x_position(320);
						else
							nws->set_x_position(30);
					});
				});

				link.add_object([&](menu::action_item &item){
					item.set_text(L"Ease Out");
					item.events().bind([&](events::menu_item_select &e){
						hk->set_easing<POINT>(utility::animation_timing::elastic::ease_out);
						if (nws->get_x_position() == 30)
							nws->set_x_position(320);
						else
							nws->set_x_position(30);
					});
				});

				link.add_object([&](menu::action_item &item){
					item.set_text(L"Ease In-Out");
					item.events().bind([&](events::menu_item_select &e){
						hk->set_easing<POINT>(utility::animation_timing::elastic::ease_in_out);
						if (nws->get_x_position() == 30)
							nws->set_x_position(320);
						else
							nws->set_x_position(30);
					});
				});
			});

			link.add_object([&](menu::link_item &link){
				link.set_text(L"Exponential");
				link.add_object([&](menu::action_item &item){
					item.set_text(L"Ease In");
					item.events().bind([&](events::menu_item_select &e){
						hk->set_easing<POINT>(utility::animation_timing::exponential::ease_in);
						if (nws->get_x_position() == 30)
							nws->set_x_position(320);
						else
							nws->set_x_position(30);
					});
				});

				link.add_object([&](menu::action_item &item){
					item.set_text(L"Ease Out");
					item.events().bind([&](events::menu_item_select &e){
						hk->set_easing<POINT>(utility::animation_timing::exponential::ease_out);
						if (nws->get_x_position() == 30)
							nws->set_x_position(320);
						else
							nws->set_x_position(30);
					});
				});

				link.add_object([&](menu::action_item &item){
					item.set_text(L"Ease In-Out");
					item.events().bind([&](events::menu_item_select &e){
						hk->set_easing<POINT>(utility::animation_timing::exponential::ease_in_out);
						if (nws->get_x_position() == 30)
							nws->set_x_position(320);
						else
							nws->set_x_position(30);
					});
				});
			});

			link.add_object([&](menu::link_item &link){
				link.set_text(L"Quadratic");
				link.add_object([&](menu::action_item &item){
					item.set_text(L"Ease In");
					item.events().bind([&](events::menu_item_select &e){
						hk->set_easing<POINT>(utility::animation_timing::quadratic::ease_in);
						if (nws->get_x_position() == 30)
							nws->set_x_position(320);
						else
							nws->set_x_position(30);
					});
				});

				link.add_object([&](menu::action_item &item){
					item.set_text(L"Ease Out");
					item.events().bind([&](events::menu_item_select &e){
						hk->set_easing<POINT>(utility::animation_timing::quadratic::ease_out);
						if (nws->get_x_position() == 30)
							nws->set_x_position(320);
						else
							nws->set_x_position(30);
					});
				});

				link.add_object([&](menu::action_item &item){
					item.set_text(L"Ease In-Out");
					item.events().bind([&](events::menu_item_select &e){
						hk->set_easing<POINT>(utility::animation_timing::quadratic::ease_in_out);
						if (nws->get_x_position() == 30)
							nws->set_x_position(320);
						else
							nws->set_x_position(30);
					});
				});
			});

			link.add_object([&](menu::link_item &link){
				link.set_text(L"Quart");
				link.add_object([&](menu::action_item &item){
					item.set_text(L"Ease In");
					item.events().bind([&](events::menu_item_select &e){
						hk->set_easing<POINT>(utility::animation_timing::quart::ease_in);
						if (nws->get_x_position() == 30)
							nws->set_x_position(320);
						else
							nws->set_x_position(30);
					});
				});

				link.add_object([&](menu::action_item &item){
					item.set_text(L"Ease Out");
					item.events().bind([&](events::menu_item_select &e){
						hk->set_easing<POINT>(utility::animation_timing::quart::ease_out);
						if (nws->get_x_position() == 30)
							nws->set_x_position(320);
						else
							nws->set_x_position(30);
					});
				});

				link.add_object([&](menu::action_item &item){
					item.set_text(L"Ease In-Out");
					item.events().bind([&](events::menu_item_select &e){
						hk->set_easing<POINT>(utility::animation_timing::quart::ease_in_out);
						if (nws->get_x_position() == 30)
							nws->set_x_position(320);
						else
							nws->set_x_position(30);
					});
				});
			});

			link.add_object([&](menu::link_item &link){
				link.set_text(L"Quint");
				link.add_object([&](menu::action_item &item){
					item.set_text(L"Ease In");
					item.events().bind([&](events::menu_item_select &e){
						hk->set_easing<POINT>(utility::animation_timing::quint::ease_in);
						if (nws->get_x_position() == 30)
							nws->set_x_position(320);
						else
							nws->set_x_position(30);
					});
				});

				link.add_object([&](menu::action_item &item){
					item.set_text(L"Ease Out");
					item.events().bind([&](events::menu_item_select &e){
						hk->set_easing<POINT>(utility::animation_timing::quint::ease_out);
						if (nws->get_x_position() == 30)
							nws->set_x_position(320);
						else
							nws->set_x_position(30);
					});
				});

				link.add_object([&](menu::action_item &item){
					item.set_text(L"Ease In-Out");
					item.events().bind([&](events::menu_item_select &e){
						hk->set_easing<POINT>(utility::animation_timing::quint::ease_in_out);
						if (nws->get_x_position() == 30)
							nws->set_x_position(320);
						else
							nws->set_x_position(30);
					});
				});
			});

			link.add_object([&](menu::link_item &link){
				link.set_text(L"Sine");
				link.add_object([&](menu::action_item &item){
					item.set_text(L"Ease In");
					item.events().bind([&](events::menu_item_select &e){
						hk->set_easing<POINT>(utility::animation_timing::sine::ease_in);
						if (nws->get_x_position() == 30)
							nws->set_x_position(320);
						else
							nws->set_x_position(30);
					});
				});

				link.add_object([&](menu::action_item &item){
					item.set_text(L"Ease Out");
					item.events().bind([&](events::menu_item_select &e){
						hk->set_easing<POINT>(utility::animation_timing::sine::ease_out);
						if (nws->get_x_position() == 30)
							nws->set_x_position(320);
						else
							nws->set_x_position(30);
					});
				});

				link.add_object([&](menu::action_item &item){
					item.set_text(L"Ease In-Out");
					item.events().bind([&](events::menu_item_select &e){
						hk->set_easing<POINT>(utility::animation_timing::sine::ease_in_out);
						if (nws->get_x_position() == 30)
							nws->set_x_position(320);
						else
							nws->set_x_position(30);
					});
				});
			});
		});
	});

	app::object::run();
}
