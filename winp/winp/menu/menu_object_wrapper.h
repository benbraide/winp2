#pragma once

#include "menu_object.h"

namespace winp::menu{
	class popup_wrapper : public popup{
	public:
		popup_wrapper();

		explicit popup_wrapper(thread::object &thread);

		popup_wrapper(thread::object &thread, HMENU handle);

		virtual ~popup_wrapper();

		virtual utility::error_code set_handle(HMENU value, const std::function<void(popup_wrapper &, utility::error_code)> &callback = nullptr);

	protected:
		virtual utility::error_code create_() override;

		virtual utility::error_code destroy_() override;

		virtual utility::error_code do_erase_child_(ui::object &child, std::size_t index) override;

		virtual utility::error_code set_handle_(HMENU value);

		virtual void resolve_handle_(HMENU handle);

		virtual std::shared_ptr<menu::item> create_item_(const MENUITEMINFOW &info, UINT index);

		std::unordered_map<thread::item *, std::shared_ptr<menu::item>> items_;
	};

	class system_popup_wrapper : public popup_wrapper, public system_object{
	public:
		system_popup_wrapper();

		explicit system_popup_wrapper(thread::object &thread);

		system_popup_wrapper(thread::object &thread, HMENU handle);

		explicit system_popup_wrapper(ui::window_surface &target_window);

		virtual ~system_popup_wrapper();

	protected:
		virtual std::shared_ptr<menu::item> create_item_(const MENUITEMINFOW &info, UINT index) override;
	};
}
