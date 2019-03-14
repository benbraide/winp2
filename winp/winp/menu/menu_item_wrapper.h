#pragma once

#include "menu_separator.h"
#include "menu_check_item.h"
#include "menu_link_item.h"
#include "menu_object_wrapper.h"

namespace winp::menu{
	class object;
	class popup_wrapper;

	class action_item_wrapper : public action_item{
	public:
		action_item_wrapper(menu::object &parent, std::size_t index, const MENUITEMINFOW &info);

		virtual ~action_item_wrapper();

	protected:
		virtual utility::error_code create_() override;

		virtual utility::error_code destroy_() override;

		virtual void resolve_info_(menu::object &parent, UINT index, const MENUITEMINFOW &info);
	};

	class link_item_wrapper : public link_item{
	public:
		link_item_wrapper(menu::object &parent, std::size_t index, const MENUITEMINFOW &info);

		virtual ~link_item_wrapper();

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

		virtual void resolve_info_(menu::object &parent, UINT index, const MENUITEMINFOW &info);

		std::shared_ptr<ui::object_collection<menu::popup_wrapper>> popup_;
	};

	class check_item_wrapper : public check_item{
	public:
		check_item_wrapper(menu::object &parent, std::size_t index, const MENUITEMINFOW &info);

		virtual ~check_item_wrapper();

	protected:
		virtual utility::error_code create_() override;

		virtual utility::error_code destroy_() override;

		virtual void resolve_info_(menu::object &parent, UINT index, const MENUITEMINFOW &info);
	};

	class separator_wrapper : public separator{
	public:
		separator_wrapper(menu::object &parent, std::size_t index, const MENUITEMINFOW &info);

		virtual ~separator_wrapper();

	protected:
		virtual utility::error_code create_() override;

		virtual utility::error_code destroy_() override;

		virtual void resolve_info_(menu::object &parent, UINT index, const MENUITEMINFOW &info);
	};
}
