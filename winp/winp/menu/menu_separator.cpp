#include "../app/app_object.h"

#include "menu_object.h"
#include "menu_separator.h"

winp::menu::separator::separator() = default;

winp::menu::separator::separator(ui::tree &parent)
	: separator(parent, static_cast<std::size_t>(-1)){}

winp::menu::separator::separator(ui::tree &parent, std::size_t index){
	set_parent(&parent, index);
}

winp::menu::separator::~separator() = default;

winp::utility::error_code winp::menu::separator::fill_info_(MENUITEMINFOW &info){
	return utility::error_code::nil;
}

UINT winp::menu::separator::get_filtered_states_() const{
	return ~0u;
}

UINT winp::menu::separator::get_types_() const{
	return (item::get_types_() | MFT_SEPARATOR);
}

winp::menu::wrapped_separator::wrapped_separator(menu::object &parent, std::size_t index)
	: separator(parent, index){
	MENUITEMINFOW info{
		sizeof(MENUITEMINFOW),
		(MIIM_FTYPE | MIIM_STATE | MIIM_BITMAP)
	};

	GetMenuItemInfoW((handle_ = parent.get_handle()), static_cast<UINT>(index), TRUE, &info);

	bitmap_ = info.hbmpItem;
	states_ = info.fState;
	types_ = info.fType;
}

winp::menu::wrapped_separator::~wrapped_separator(){
	destruct_();
}

winp::utility::error_code winp::menu::wrapped_separator::create_(){
	return utility::error_code::nil;
}

winp::utility::error_code winp::menu::wrapped_separator::destroy_(){
	handle_ = nullptr;
	return utility::error_code::nil;
}
