#include "app/app_object.h"
#include "ui/ui_tree.h"
#include "utility/traits.h"

void gf(int &i){
	i = 99;
}

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, LPWSTR cmd_line, int cmd_show){
	auto i = 0;

	struct s{
		void operator ()(bool){}
	} si;

	auto f = []{};

	auto ctf1 = winp::utility::object_to_function_traits<decltype(f)>::get(f);
	auto ctf2 = winp::utility::object_to_function_traits<s>::get(si);
	auto ctf3 = winp::utility::object_to_function_traits<decltype(gf)>::get(gf);
	auto ctf4 = winp::utility::object_to_function_traits<decltype(&wWinMain)>::get(&wWinMain);
	
	winp::utility::function_or_function_object_traits<decltype(f)> ffo1;
	winp::utility::function_or_function_object_traits<s> ffo2;
	winp::utility::function_or_function_object_traits<decltype(wWinMain)> ffo3;
	winp::utility::function_or_function_object_traits<decltype(&wWinMain)> ffo4;

	winp::app::main_object main_app;
	winp::ui::object uio;
	uio.get_parent();
	return 0;
}
