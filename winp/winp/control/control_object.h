#pragma once

#include "../ui/ui_window_surface.h"

namespace winp::control{
	class object : public ui::window_surface{
	public:
		virtual ~object();

		virtual LRESULT dispatch_command(MSG &msg) const;

		virtual LRESULT dispatch_notification(MSG &msg) const;

	protected:
		object(thread::object &thread, const std::wstring &class_name, DWORD common_id);

		virtual bool is_dialog_message_(MSG &msg) const override;

		virtual system_menu_type &get_system_menu_() const override;

		virtual bar_menu_type &get_menu_bar_() const override;

		virtual const std::wstring &get_class_name_() const override;

		virtual HINSTANCE get_instance_() const override;

		std::wstring class_name_;
	};
}
