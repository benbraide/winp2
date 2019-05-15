#pragma once

#include "../ui/ui_object_collection.h"

#include "menu_link_item.h"
#include "menu_object.h"

namespace winp::menu{
	template <class popup_type>
	class generic_link_item_with_popup : public link_item{
	public:
		generic_link_item_with_popup()
			: generic_link_item_with_popup(app::object::get_thread()){}

		explicit generic_link_item_with_popup(thread::object &thread)
			: link_item(thread){
			target_ = (popup_ = std::make_shared<ui::object_collection<popup_type>>(thread)).get();
		}

		explicit generic_link_item_with_popup(tree &parent)
			: generic_link_item_with_popup(parent, static_cast<std::size_t>(-1)){}

		generic_link_item_with_popup(tree &parent, std::size_t index)
			: generic_link_item_with_popup(parent.get_thread()){
			set_parent(&parent, index);
		}

		virtual ~generic_link_item_with_popup(){
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
		explicit generic_link_item_with_popup(thread::object &thread, bool)
			: link_item(thread){}

		virtual utility::error_code create_() override{
			popup_->create();
			if (auto error_code = link_item::create_(); error_code != utility::error_code::nil){
				popup_->destroy();
				return error_code;
			}

			return utility::error_code::nil;
		}

		virtual utility::error_code destroy_() override{
			if (popup_ != nullptr)
				popup_->destroy();

			if (auto error_code = link_item::destroy_(); error_code != utility::error_code::nil)
				return error_code;

			return utility::error_code::nil;
		}

		std::shared_ptr<ui::object_collection<popup_type>> popup_;
	};

	using link_item_with_popup = generic_link_item_with_popup<menu::popup>;

	class system_link_item_with_popup : public generic_link_item_with_popup<menu::system_popup>{
	public:
		using m_base_type = generic_link_item_with_popup<menu::system_popup>;

		explicit system_link_item_with_popup(tree &parent);

		system_link_item_with_popup(tree &parent, std::size_t index);

		virtual ~system_link_item_with_popup();
	};

}
