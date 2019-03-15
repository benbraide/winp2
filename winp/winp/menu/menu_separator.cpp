#include "../app/app_collection.h"

#include "menu_object.h"
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

HMENU winp::menu::separator::create_handle_(menu::object &parent){
	MENUITEMINFOW info{};
	auto index = fill_basic_info_(parent, info);

	return ((InsertMenuItemW(parent.get_handle(), index, TRUE, &info) == FALSE) ? nullptr : parent.get_handle());
}

winp::utility::error_code winp::menu::separator::generate_id_(){
	id_ = 0u;
	return utility::error_code::nil;
}

UINT winp::menu::separator::get_filtered_states_() const{
	return ~0u;
}

UINT winp::menu::separator::get_types_() const{
	return (item::get_types_() | MFT_SEPARATOR);
}
