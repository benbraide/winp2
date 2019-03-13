#include "../app/app_collection.h"

#include "menu_tree.h"
#include "menu_separator.h"

winp::menu::separator::separator()
	: separator(app::collection::get_main()->get_thread()){}

winp::menu::separator::separator(thread::object &thread)
	: item(thread){}

winp::menu::separator::separator(tree &parent)
	: separator(parent, static_cast<std::size_t>(-1)){}

winp::menu::separator::separator(tree &parent, std::size_t index)
	: separator(parent.get_thread()){
	set_parent(&parent, index);
}

winp::menu::separator::~separator() = default;

UINT winp::menu::separator::get_filtered_states_() const{
	return ~0u;
}

UINT winp::menu::separator::get_types_() const{
	return (item::get_types_() | MFT_SEPARATOR);
}
