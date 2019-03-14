#pragma once

#include "menu_link_item.h"
#include "menu_object.h"

namespace winp::menu{
	class link_item_with_popup : public link_item{
	public:
		link_item_with_popup();

		explicit link_item_with_popup(thread::object &thread);

		explicit link_item_with_popup(tree &parent);

		link_item_with_popup(tree &parent, std::size_t index);

		virtual ~link_item_with_popup();

		template <typename callback_type, typename... args_types>
		auto add_object(const callback_type &callback, args_types &&... args){
			return popup_->add_object(callback, std::forward<args_types>(args)...);
		}

		template <typename object_type, typename... args_types>
		auto add_object_direct(args_types &&... args){
			return popup_->add_object_direct<object_type>(std::forward<args_types>(args)...);
		}

	protected:
		virtual utility::error_code create_() override;

		virtual utility::error_code destroy_() override;

		virtual utility::error_code set_parent_value_(ui::tree *value, bool changing) override;

		std::shared_ptr<ui::object_collection<menu::popup>> popup_;
	};
}
