#include "ui_object.h"

winp::ui::hook::hook(object &target)
	: target_(target){}

winp::ui::hook::~hook() = default;

bool winp::ui::hook::setup_(){
	return (&thread_ == &target_.get_thread());
}
