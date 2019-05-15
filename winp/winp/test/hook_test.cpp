#include "hook_test.h"

void winp::test::hook::run(int cmd_show){
	ui::object_collection<winp::window::object> ws;
	ws.set_caption(L"Test Window");
	ws.set_position(30, 30);
	ws.set_size(900, 500);
	ws.create();
	ws.show(cmd_show);

	ws.add_object([&](control::push_button &btn){
		btn.set_text(L"Top-Left Aligned");
		btn.insert_hook<ui::placement_hook>(ui::placement_hook::alignment_type::top_left);
	});

	ws.add_object([&](control::push_button &btn){
		btn.set_text(L"Top-Center Aligned");
		btn.insert_hook<ui::placement_hook>(ui::placement_hook::alignment_type::top_center);
	});

	ws.add_object([&](control::push_button &btn){
		btn.set_text(L"Top-Right Aligned");
		btn.insert_hook<ui::placement_hook>(ui::placement_hook::alignment_type::top_right);
	});

	ws.add_object([&](control::push_button &btn){
		btn.set_text(L"Center-Right Aligned");
		btn.insert_hook<ui::placement_hook>(ui::placement_hook::alignment_type::center_right);
	});

	ws.add_object([&](control::push_button &btn){
		btn.set_text(L"Center Aligned");
		btn.insert_hook<ui::placement_hook>(ui::placement_hook::alignment_type::center);
	});

	ws.add_object([&](control::push_button &btn){
		btn.set_text(L"Center-Left Aligned");
		btn.insert_hook<ui::placement_hook>(ui::placement_hook::alignment_type::center_left);
	});

	ws.add_object([&](control::push_button &btn){
		btn.set_text(L"Bottom-Left Aligned");
		btn.insert_hook<ui::placement_hook>(ui::placement_hook::alignment_type::bottom_left);
	});

	ws.add_object([&](control::push_button &btn){
		btn.set_text(L"Bottom-Center Aligned");
		btn.insert_hook<ui::placement_hook>(ui::placement_hook::alignment_type::bottom_center);
	});

	ws.add_object([&](control::push_button &btn){
		btn.set_text(L"Bottom-Right Aligned");
		btn.insert_hook<ui::placement_hook>(ui::placement_hook::alignment_type::bottom_right);
	});

	app::object::run();
}
