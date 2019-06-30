#include "hook_test.h"

void winp::test::hook::run(int cmd_show){
	winp::window::object ws;
	ws.set_caption(L"Test Window");
	ws.set_position(30, 30);
	ws.set_size(1500, 800);
	ws.create();
	ws.show(cmd_show);

	ws.get_grid([](winp::grid::object &grd){
		grd.add_object([](winp::grid::row &row){
			row.add_object([](winp::grid::column &col){
				col.add_object([&](control::push_button &btn){
					btn.set_text(L"Top-Left Aligned");
					btn.insert_hook<ui::placement_hook>(ui::placement_hook::alignment_type::top_left);
				});

				col.add_object([&](control::push_button &btn){
					btn.set_text(L"Top-Center Aligned");
					btn.insert_hook<ui::placement_hook>(ui::placement_hook::alignment_type::top_center);
				});

				col.add_object([&](control::push_button &btn){
					btn.set_text(L"Top-Right Aligned");
					btn.insert_hook<ui::placement_hook>(ui::placement_hook::alignment_type::top_right);
				});

				col.add_object([&](control::push_button &btn){
					btn.set_text(L"Center-Right Aligned");
					btn.insert_hook<ui::placement_hook>(ui::placement_hook::alignment_type::center_right);
				});

				col.add_object([&](control::push_button &btn){
					btn.set_text(L"Center Aligned");
					btn.insert_hook<ui::placement_hook>(ui::placement_hook::alignment_type::center);
				});

				col.add_object([&](control::push_button &btn){
					btn.set_text(L"Center-Left Aligned");
					btn.insert_hook<ui::placement_hook>(ui::placement_hook::alignment_type::center_left);
				});

				col.add_object([&](control::push_button &btn){
					btn.set_text(L"Bottom-Left Aligned");
					btn.insert_hook<ui::placement_hook>(ui::placement_hook::alignment_type::bottom_left);
				});

				col.add_object([&](control::push_button &btn){
					btn.set_text(L"Bottom-Center Aligned");
					btn.insert_hook<ui::placement_hook>(ui::placement_hook::alignment_type::bottom_center);
				});

				col.add_object([&](control::push_button &btn){
					btn.set_text(L"Bottom-Right Aligned");
					btn.insert_hook<ui::placement_hook>(ui::placement_hook::alignment_type::bottom_right);
				});

				col.add_object([&](control::push_button &btn){
					btn.set_text(L"Drag-Hooked");
					btn.set_position(200, 90);
					btn.insert_hook<ui::drag_hook>();
				});

				col.add_object([&](control::push_button &btn){
					btn.set_text(L"Sibling Aligned");
					btn.insert_hook<ui::sibling_placement_hook>(ui::sibling_placement_hook::sibling_type::previous, ui::sibling_placement_hook::alignment_type::center_right, POINT{ 5, 0 });
				});
			});

			row.add_object([](winp::grid::column &col){
				col.add_object([](winp::window::object &cw){
					cw.set_caption(L"Children Contain Window");
					cw.insert_hook<ui::children_contain_hook>(SIZE{ 30, 30 });
					cw.set_position(30, 30);
					cw.set_size(900, 500);
					cw.create();
					cw.show();

					cw.add_object([](winp::window::object &ccw){
						ccw.set_caption(L"Contained Child Window");
						ccw.set_position(30, 30);
						ccw.set_size(450, 250);
						ccw.create();
						ccw.show();
					});
				});
			});
		});
	});

	app::object::run();
}
