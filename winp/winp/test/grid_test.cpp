#include "grid_test.h"

void winp::test::grid::run(int cmd_show){
	ui::object_collection<winp::window::object> ws;
	ws.set_caption(L"Test Window");
	ws.set_position(30, 30);
	ws.set_size(900, 500);
	ws.create();
	ws.show(cmd_show);

	ws.get_grid([](ui::object_collection<winp::grid::object> &grd){
		grd.add_object([](ui::object_collection<winp::grid::row> &row){
			row.add_object([](winp::ui::object_collection<winp::grid::column> &col){
				col.set_background_color(D2D1::ColorF::Red);
				col.add_object([&](control::push_button &btn){
					btn.set_text(L"Shared Column");
					btn.insert_hook<ui::placement_hook>(ui::placement_hook::alignment_type::center);
				});
			});

			row.add_object([](winp::ui::object_collection<winp::grid::column> &col){
				col.set_background_color(D2D1::ColorF::Green);
				col.add_object([&](control::push_button &btn){
					btn.set_text(L"Shared Column");
					btn.insert_hook<ui::placement_hook>(ui::placement_hook::alignment_type::center);
				});
			});

			row.add_object([](winp::ui::object_collection<winp::grid::column> &col){
				col.set_background_color(D2D1::ColorF::Blue);
				col.add_object([&](control::push_button &btn){
					btn.set_text(L"Shared Column");
					btn.insert_hook<ui::placement_hook>(ui::placement_hook::alignment_type::center);
				});
			});
		});

		grd.add_object([&](ui::object_collection<winp::grid::row> &row){
			row.add_object([](winp::ui::object_collection<winp::grid::fixed_column> &col){
				col.set_background_color(D2D1::ColorF::Black);
				col.set_width(300);
				col.add_object([&](control::push_button &btn){
					btn.set_text(L"Fixed Column");
					btn.insert_hook<ui::placement_hook>(ui::placement_hook::alignment_type::center);
				});
			});

			row.add_object([](winp::ui::object_collection<winp::grid::proportional_shared_column> &col){
				col.set_background_color(D2D1::ColorF::Teal);
				col.set_proportion(0.6f);
				col.add_object([&](control::push_button &btn){
					btn.set_text(L"Proportional Shared Column");
					btn.insert_hook<ui::placement_hook>(ui::placement_hook::alignment_type::center);
				});
			});

			row.add_object([](winp::ui::object_collection<winp::grid::column> &col){
				col.set_background_color(D2D1::ColorF::Aqua);
				col.add_object([&](control::push_button &btn){
					btn.set_text(L"Shared Column");
					btn.insert_hook<ui::placement_hook>(ui::placement_hook::alignment_type::center);
				});
			});
		});

		grd.add_object([&](ui::object_collection<winp::grid::row> &row){
			row.set_background_color(D2D1::ColorF::Magenta);
			row.add_object([](winp::ui::object_collection<winp::grid::proportional_column> &col){
				col.set_background_color(D2D1::ColorF::Gray);
				col.set_proportion(0.45f);
				col.add_object([&](control::push_button &btn){
					btn.set_text(L"Proportional Column");
					btn.insert_hook<ui::placement_hook>(ui::placement_hook::alignment_type::center);
				});
			});

			row.add_object([](winp::ui::object_collection<winp::grid::fixed_column> &col){
				col.set_background_color(D2D1::ColorF::Purple);
				col.set_width(400);
				col.add_object([&](control::push_button &btn){
					btn.set_text(L"Fixed Column");
					btn.insert_hook<ui::placement_hook>(ui::placement_hook::alignment_type::center);
				});
			});
		});
	});

	app::object::run();
}
