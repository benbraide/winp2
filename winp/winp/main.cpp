#include "app/app_object.h"
#include "ui/ui_tree.h"
#include "utility/traits.h"

void gf(int &i){
	i = 99;
}

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, LPWSTR cmd_line, int cmd_show){
	auto i = 0;

	class s{
	public:
		virtual ~s() = default;

		void operator ()(bool){}
	} si;

	class sc : public s{
	public:
		virtual ~sc() = default;
	} sci;

	auto &scis = sci;
	auto eti = (typeid(scis) == typeid(sci));

	auto f = []{};

	auto ctf1 = winp::utility::object_to_function_traits::get(f);
	auto ctf2 = winp::utility::object_to_function_traits::get(si);
	auto ctf3 = winp::utility::object_to_function_traits::get(gf);
	auto ctf4 = winp::utility::object_to_function_traits::get(wWinMain);
	auto ctf5 = winp::utility::object_to_function_traits::get(&wWinMain);
	
	winp::utility::function_or_function_object_traits<decltype(f)> ffo1;
	winp::utility::function_or_function_object_traits<s> ffo2;
	winp::utility::function_or_function_object_traits<decltype(wWinMain)> ffo3;
	winp::utility::function_or_function_object_traits<decltype(&wWinMain)> ffo4;

	winp::app::main_object main_app;
	winp::ui::object uio;
	uio.get_parent();
	uio.bind_event([](winp::events::object &){});
	uio.bind_event([](const winp::events::object &){});
	uio.bind_event<winp::events::object>([]{});
	uio.events().bind([](winp::events::object &){});
	return 0;
}
