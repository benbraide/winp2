#pragma once

#include "../ui/ui_window_surface.h"

namespace winp::control{
	class object : public ui::window_surface{
	public:
		virtual ~object();

		virtual popup_menu_type &get_system_menu(const std::function<void(popup_menu_type &)> &callback = nullptr) const override;

		virtual menu::bar &get_menu_bar(const std::function<void(menu::bar &)> &callback = nullptr) const override;

	protected:
		friend class thread::item_manager;

		object(const std::wstring &class_name, DWORD common_id);

		virtual bool is_dialog_message_(MSG &msg) const override;

		virtual const std::wstring &get_class_name_() const override;

		virtual HINSTANCE get_instance_() const override;

		virtual LRESULT dispatch_command_(MSG &msg) const;

		virtual LRESULT dispatch_notification_(MSG &msg) const;

		std::wstring class_name_;
	};
}
