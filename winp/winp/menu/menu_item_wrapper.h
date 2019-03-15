#pragma once

#include "../ui/ui_object_collection.h"

#include "menu_separator.h"
#include "menu_check_item.h"
#include "menu_link_item.h"
#include "menu_object_wrapper.h"

namespace winp::menu{
	class action_item_wrapper : public action_item{
	public:
		action_item_wrapper(menu::object &parent, std::size_t index, const MENUITEMINFOW &info);

		virtual ~action_item_wrapper();

	protected:
		explicit action_item_wrapper(thread::object &thread);

		virtual utility::error_code create_() override;

		virtual utility::error_code destroy_() override;

		virtual void resolve_info_(menu::object &parent, UINT index, const MENUITEMINFOW &info);
	};

	class system_action_item_wrapper : public action_item_wrapper{
	public:
		system_action_item_wrapper(menu::object &parent, std::size_t index, const MENUITEMINFOW &info);

		virtual ~system_action_item_wrapper();
	};

	template <class popup_type>
	class generic_link_item_wrapper : public link_item{
	public:
		generic_link_item_wrapper(menu::object &parent, std::size_t index, const MENUITEMINFOW &info)
			: generic_link_item_wrapper(parent.get_thread()){
			target_ = (popup_ = std::make_shared<ui::object_collection<popup_type>>(thread_)).get();
			set_parent(&parent, index);
			resolve_info_(parent, static_cast<UINT>(index), info);
		}

		virtual ~generic_link_item_wrapper(){
			destruct();
		}

		template <typename callback_type, typename... args_types>
		auto add_object(const callback_type &callback, args_types &&... args){
			return popup_->add_object(callback, std::forward<args_types>(args)...);
		}

		template <typename object_type, typename... args_types>
		auto add_object_direct(args_types &&... args){
			return popup_->add_object_direct<object_type>(std::forward<args_types>(args)...);
		}

	protected:
		explicit generic_link_item_wrapper(thread::object &thread)
			: link_item(thread){}

		virtual utility::error_code create_() override{
			if (handle_ != nullptr)
				return utility::error_code::nil;

			auto object_parent = dynamic_cast<menu::object *>(parent_);
			if (object_parent == nullptr || !object_parent->is_created())
				return utility::error_code::parent_not_created;

			MENUITEMINFOW info{
				sizeof(MENUITEMINFOW),
				(MIIM_ID | MIIM_STRING | MIIM_FTYPE | MIIM_STATE | MIIM_BITMAP | MIIM_SUBMENU)
			};

			auto index = static_cast<UINT>(get_index_());
			if (GetMenuItemInfoW(object_parent->get_handle(), index, TRUE, &info) == FALSE)
				return utility::error_code::action_could_not_be_completed;

			resolve_info_(*object_parent, index, info);
			return utility::error_code::nil;
		}

		virtual utility::error_code destroy_() override{
			if (handle_ != nullptr){
				handle_ = nullptr;
				popup_->destroy();
				thread_.get_item_manager().remove_generated_item_id(*this);
			}

			return utility::error_code::nil;
		}

		virtual void resolve_info_(menu::object &parent, UINT index, const MENUITEMINFOW &info){
			if (0u < info.cch){//Retrieve text
				text_.resize(info.cch);

				MENUITEMINFOW text_info{
					sizeof(MENUITEMINFOW),
					MIIM_STRING
				};

				text_info.dwTypeData = text_.data();
				text_info.cch = (info.cch + 1u);

				GetMenuItemInfoW(parent.get_handle(), index, TRUE, &text_info);
			}

			id_ = info.wID;
			handle_ = parent.get_handle();
			bitmap_ = info.hbmpItem;

			states_ = info.fState;
			types_ = info.fType;

			if (info.hSubMenu != nullptr)//Wrap sub-menu
				popup_->set_handle(info.hSubMenu);

			thread_.get_item_manager().add_generated_item_id(*this);
		}

		std::shared_ptr<ui::object_collection<popup_type>> popup_;
	};

	using link_item_wrapper = generic_link_item_wrapper<menu::popup_wrapper>;

	class system_link_item_wrapper : public generic_link_item_wrapper<menu::system_popup_wrapper>{
	public:
		using m_base_type = generic_link_item_wrapper<menu::system_popup_wrapper>;

		system_link_item_wrapper(menu::object &parent, std::size_t index, const MENUITEMINFOW &info);

		virtual ~system_link_item_wrapper() = default;
	};

	class check_item_wrapper : public check_item{
	public:
		check_item_wrapper(menu::object &parent, std::size_t index, const MENUITEMINFOW &info);

		virtual ~check_item_wrapper();

	protected:
		explicit check_item_wrapper(thread::object &thread);

		virtual utility::error_code create_() override;

		virtual utility::error_code destroy_() override;

		virtual void resolve_info_(menu::object &parent, UINT index, const MENUITEMINFOW &info);
	};

	class system_check_item_wrapper : public check_item_wrapper{
	public:
		system_check_item_wrapper(menu::object &parent, std::size_t index, const MENUITEMINFOW &info);

		virtual ~system_check_item_wrapper();
	};

	class separator_wrapper : public separator{
	public:
		separator_wrapper(menu::object &parent, std::size_t index, const MENUITEMINFOW &info);

		virtual ~separator_wrapper();

	protected:
		explicit separator_wrapper(thread::object &thread);

		virtual utility::error_code create_() override;

		virtual utility::error_code destroy_() override;

		virtual void resolve_info_(menu::object &parent, UINT index, const MENUITEMINFOW &info);
	};

	class system_separator_wrapper : public separator_wrapper{
	public:
		system_separator_wrapper(menu::object &parent, std::size_t index, const MENUITEMINFOW &info);

		virtual ~system_separator_wrapper();
	};
}
