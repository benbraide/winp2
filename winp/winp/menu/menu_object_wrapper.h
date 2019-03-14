#pragma once

#include "menu_object.h"
#include "menu_item_wrapper.h"

namespace winp::menu{
	class popup_wrapper : public popup{
	public:
		popup_wrapper();

		explicit popup_wrapper(thread::object &thread);

		popup_wrapper(thread::object &thread, HMENU handle, bool is_system);

		virtual ~popup_wrapper();

		virtual utility::error_code set_handle(HMENU value, bool is_system, const std::function<void(popup_wrapper &, utility::error_code)> &callback = nullptr);

	protected:
		virtual utility::error_code create_() override;

		virtual utility::error_code destroy_() override;

		virtual utility::error_code do_erase_child_(ui::object &child, std::size_t index) override;

		virtual bool is_system_() const override;

		virtual utility::error_code set_handle_(HMENU value);

		virtual void resolve_handle_(HMENU handle);

		bool is_system_value_ = false;
		std::unordered_map<thread::item *, std::shared_ptr<menu::item>> items_;
	};
}
