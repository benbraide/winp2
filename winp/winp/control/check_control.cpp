#include "../app/app_object.h"

#include "check_control.h"
#include "control_group.h"

winp::control::check::check()
	: check(app::object::get_thread()){}

winp::control::check::check(thread::object &thread)
	: push_button(thread){
	add_event_handler_([this](events::click &e){
		if (!is_radio_())//Toggle check state
			set_checked_state_(!is_checked_);
		else//Radio check item
			set_checked_state_(true);
	});

	add_event_handler_([this](events::item_check &e){
		if (!e.is_checked())
			return;

		if (auto radio_group_parent = get_first_ancestor_of_<radio_group>(nullptr); radio_group_parent != nullptr){
			radio_group_parent->traverse_all_offspring_of<check>([this](check &offspring){//Remove check state from siblings
				if (&offspring != this)
					offspring.set_checked_state_(false);
			}, true);
		}
	});

	padding_.cx = 0;
	padding_.cy = 0;
}

winp::control::check::check(tree &parent)
	: check(parent, static_cast<std::size_t>(-1)){}

winp::control::check::check(tree &parent, std::size_t index)
	: check(parent.get_thread()){
	set_parent(&parent, index);
}

winp::control::check::~check() = default;

winp::utility::error_code winp::control::check::set_checked_state(bool is_checked, const std::function<void(check &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_checked_state_(is_checked));
	}, (callback != nullptr), utility::error_code::nil);
}

bool winp::control::check::is_checked(const std::function<void(bool)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, is_checked_);
	}, (callback != nullptr), false);
}

bool winp::control::check::is_radio(const std::function<void(bool)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, is_radio_());
	}, (callback != nullptr), false);
}

DWORD winp::control::check::get_persistent_styles_(bool is_extended) const{
	return (push_button::get_persistent_styles_(is_extended) | (is_extended ? 0u : (is_radio_() ? BS_RADIOBUTTON : BS_CHECKBOX)));
}

SIZE winp::control::check::compute_additional_size_() const{
	return SIZE{ (GetSystemMetrics(SM_CXMENUCHECK) + GetSystemMetrics(SM_CXEDGE)), 0 };
}

winp::utility::error_code winp::control::check::set_checked_state_(bool is_checked){
	if (is_checked == is_checked_)
		return utility::error_code::nil;

	if (handle_ != nullptr)//Update states
		SendMessageW(handle_, BM_SETCHECK, (is_checked ? BST_CHECKED : BST_UNCHECKED), 0);

	is_checked_ = is_checked;
	thread_.send_message(*this, WINP_WM_ITEM_CHECK, (is_checked ? TRUE : FALSE));

	return utility::error_code::nil;
}

bool winp::control::check::is_radio_() const{
	return (get_first_ancestor_of_<radio_group>(nullptr) != nullptr);
}

winp::control::three_state_check::three_state_check()
	: three_state_check(app::object::get_thread()){}

winp::control::three_state_check::three_state_check(thread::object &thread)
	: push_button(thread){
	add_event_handler_([this](events::click &e){
		set_checked_state_(get_next_checked_state(checked_state_));
	});

	padding_.cx = 0;
	padding_.cy = 0;
}

winp::control::three_state_check::three_state_check(tree &parent)
	: three_state_check(parent, static_cast<std::size_t>(-1)){}

winp::control::three_state_check::three_state_check(tree &parent, std::size_t index)
	: three_state_check(parent.get_thread()){
	set_parent(&parent, index);
}

winp::control::three_state_check::~three_state_check() = default;

winp::utility::error_code winp::control::three_state_check::set_checked_state(checked_state_type state, const std::function<void(three_state_check &, utility::error_code)> &callback){
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, *this, set_checked_state_(state));
	}, (callback != nullptr), utility::error_code::nil);
}

winp::control::three_state_check::checked_state_type winp::control::three_state_check::get_checked_state(const std::function<void(checked_state_type)> &callback) const{
	return compute_or_post_task_inside_thread_context([=]{
		return pass_return_value_to_callback(callback, checked_state_);
	}, (callback != nullptr), checked_state_type::indeterminate);
}

winp::control::three_state_check::checked_state_type winp::control::three_state_check::get_next_checked_state(checked_state_type state){
	switch (state){
	case checked_state_type::checked:
		return checked_state_type::unchecked;
	case checked_state_type::unchecked:
		return checked_state_type::indeterminate;
	default:
		break;
	}

	return checked_state_type::checked;
}

DWORD winp::control::three_state_check::get_persistent_styles_(bool is_extended) const{
	return (push_button::get_persistent_styles_(is_extended) | (is_extended ? 0u : BS_3STATE));
}

SIZE winp::control::three_state_check::compute_additional_size_() const{
	return SIZE{ (GetSystemMetrics(SM_CXMENUCHECK) + GetSystemMetrics(SM_CXEDGE)), 0 };
}

winp::utility::error_code winp::control::three_state_check::set_checked_state_(checked_state_type state){
	if (state == checked_state_)
		return utility::error_code::nil;

	if (handle_ != nullptr){//Update states
		switch (state){
		case checked_state_type::checked:
			SendMessageW(handle_, BM_SETCHECK, BST_CHECKED, 0);
			break;
		case checked_state_type::unchecked:
			SendMessageW(handle_, BM_SETCHECK, BST_UNCHECKED, 0);
			break;
		default:
			SendMessageW(handle_, BM_SETCHECK, BST_INDETERMINATE, 0);
			break;
		}
	}

	checked_state_ = state;
	thread_.send_message(*this, WINP_WM_ITEM_CHECK, static_cast<int>(checked_state_));

	return utility::error_code::nil;
}
