#pragma once

#include "../ui/ui_window_surface.h"

namespace winp::control{
	class object : public ui::window_surface{
	public:
		virtual ~object();

	protected:
		friend class thread::item_manager;

		object(thread::object &thread, const std::wstring &class_name, DWORD common_id);

		object(thread::object &thread, const std::wstring &class_name, DWORD common_id, bool init_grid);

		virtual bool is_dialog_message_(MSG &msg) const override;

		virtual system_menu_type &get_system_menu_() const override;

		virtual bar_menu_type &get_menu_bar_() const override;

		virtual const std::wstring &get_class_name_() const override;

		virtual HINSTANCE get_instance_() const override;

		virtual LRESULT dispatch_command_(MSG &msg) const;

		virtual LRESULT dispatch_notification_(MSG &msg) const;

		std::wstring class_name_;
	};
}
