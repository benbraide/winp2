#include "app/app_object.h"
#include "ui/ui_tree.h"
#include "utility/traits.h"

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, LPWSTR cmd_line, int cmd_show){
	winp::app::main_object main_app;

	winp::ui::object uio;
	winp::ui::tree uit;

	uio.bind_event([](winp::events::destruct &e){
		auto &t = e.get_target();
		auto z = 0;
	});

	uio.bind_event([](winp::events::parent_change &e){
		auto v = e.get_value();
		auto d = e.get_context().is_destructed();
		auto z = 0;
	});

	std::thread th([&]{
		uit.add_child(uio, [&](winp::ui::tree &p, winp::utility::error_code e){
			auto up = uio.get_parent();
			up = nullptr;
		});
	});

	th.detach();

	return main_app.get_thread().run();
}
