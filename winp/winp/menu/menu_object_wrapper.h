#pragma once

#include "menu_object.h"
#include "menu_item_wrapper.h"

namespace winp::menu{
	class popup_wrapper : public popup{
	public:
		explicit popup_wrapper(thread::object &thread, HMENU handle);

		virtual ~popup_wrapper();

	protected:
		virtual utility::error_code create_() override;

		virtual utility::error_code destroy_() override;

		virtual utility::error_code do_erase_child_(ui::object &child, std::size_t index) override;

		void resolve_handle_(HMENU handle);

		std::unordered_map<thread::item *, std::shared_ptr<menu::item>> items_;
	};
}
