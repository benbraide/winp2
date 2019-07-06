#pragma once

#include "menu_item.h"

namespace winp::menu{
	class link_item : public item{
	public:
		link_item();

		explicit link_item(ui::tree &parent);

		link_item(ui::tree &parent, std::size_t index);

		virtual ~link_item();

		template <typename callback_type, typename... args_types>
		auto add_object(const callback_type &callback, args_types &&... args){
			return target_->add_object(callback, std::forward<args_types>(args)...);
		}

		template <typename object_type, typename... args_types>
		auto add_object_direct(args_types &&... args){
			return target_->add_object_direct<object_type>(std::forward<args_types>(args)...);
		}

		virtual popup *get_target(const std::function<void(popup *)> &callback = nullptr) const;

		virtual utility::error_code set_text(const std::wstring &value, const std::function<void(link_item &, utility::error_code)> &callback = nullptr);

		virtual const std::wstring &get_text(const std::function<void(const std::wstring &)> &callback = nullptr) const;

	protected:
		friend class tree;
		friend class popup;
		friend class menu::object;

		virtual utility::error_code fill_info_(MENUITEMINFOW &info) override;

		virtual utility::error_code set_text_(const std::wstring &value);

		std::shared_ptr<tree> target_;
		std::wstring text_;
	};

	class wrapped_link_item : public link_item{
	public:
		virtual ~wrapped_link_item();

	protected:
		friend class wrapped_popup;

		wrapped_link_item(menu::object &parent, std::size_t index);

		virtual utility::error_code create_() override;

		virtual utility::error_code destroy_() override;
	};
}
