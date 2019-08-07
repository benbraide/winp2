#pragma once

#include "../menu/menu_object.h"
#include "../grid/grid_object.h"

#include "ui_tree.h"
#include "ui_visible_surface.h"

namespace winp::ui{
	class window_surface : public tree, public visible_surface{
	public:
		using popup_menu_type = menu::popup;

		window_surface();

		explicit window_surface(tree &parent);

		window_surface(tree &parent, std::size_t index);

		virtual ~window_surface();

		using visible_surface::show;

		virtual utility::error_code show(int how, const std::function<void(window_surface &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code maximize(const std::function<void(window_surface &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code toggle_maximized(const std::function<void(window_surface &, utility::error_code)> &callback = nullptr);

		virtual bool is_maximized(const std::function<void(bool)> &callback = nullptr) const;

		virtual utility::error_code minimize(const std::function<void(window_surface &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code toggle_minimized(const std::function<void(window_surface &, utility::error_code)> &callback = nullptr);

		virtual bool is_minimized(const std::function<void(bool)> &callback = nullptr) const;

		virtual utility::error_code set_styles(DWORD value, bool is_extended, const std::function<void(window_surface &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code add_styles(DWORD value, bool is_extended, const std::function<void(window_surface &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code remove_styles(DWORD value, bool is_extended, const std::function<void(window_surface &, utility::error_code)> &callback = nullptr);

		virtual DWORD get_styles(bool is_extended, const std::function<void(DWORD)> &callback = nullptr) const;

		virtual bool has_styles(DWORD value, bool is_extended, bool has_all, const std::function<void(bool)> &callback = nullptr) const;

		virtual HWND get_handle(const std::function<void(HWND)> &callback = nullptr) const;

		virtual popup_menu_type &get_system_menu(const std::function<void(popup_menu_type &)> &callback = nullptr) const;

		virtual menu::bar &get_menu_bar(const std::function<void(menu::bar &)> &callback = nullptr) const;

		virtual const std::wstring &get_class_name(const std::function<void(const std::wstring &)> &callback = nullptr) const;

		virtual const wchar_t *get_theme_id(const std::function<void(const wchar_t *)> &callback = nullptr) const;

		virtual void traverse_child_windows(const std::function<bool(window_surface &)> &callback, bool block) const;

		virtual void traverse_all_child_windows(const std::function<void(window_surface &)> &callback, bool block) const;

	protected:
		friend class surface;
		friend class thread::item_manager;

		virtual utility::error_code create_() override;

		virtual utility::error_code destroy_() override;

		virtual bool is_created_() const override;

		virtual HTHEME get_theme_() const override;

		virtual std::pair<HDC, HWND> get_device_context_() const override;

		virtual utility::error_code redraw_(bool non_client) const override;

		virtual utility::error_code redraw_(const RECT &region) const override;

		virtual utility::error_code redraw_(HRGN rgn) const override;

		virtual utility::error_code show_() override;

		virtual utility::error_code hide_() override;

		virtual bool is_visible_() const override;

		virtual SIZE get_current_client_size_() const override;

		virtual RECT get_client_padding_() const;

		virtual POINT get_current_absolute_position_() const override;

		virtual utility::error_code update_dimension_(const RECT &previous_dimension, int x, int y, int width, int height, UINT flags) override;

		virtual RECT get_current_absolute_dimension_() const override;

		virtual POINT convert_position_from_absolute_value_(int x, int y) const override;

		virtual POINT convert_position_to_absolute_value_(int x, int y) const override;

		virtual UINT absolute_hit_test_(int x, int y) const override;

		virtual bool is_dialog_message_(MSG &msg) const override;

		virtual void update_position_() const;

		virtual utility::error_code show_(int how);

		virtual utility::error_code maximize_();

		virtual utility::error_code restore_maximized_();

		virtual bool is_maximized_() const;

		virtual utility::error_code minimize_();

		virtual utility::error_code restore_minimized_();

		virtual bool is_minimized_() const;

		virtual utility::error_code set_styles_(DWORD value, bool is_extended);

		virtual DWORD get_styles_(bool is_extended) const;

		virtual bool has_styles_(DWORD value, bool is_extended, bool has_all) const;

		virtual DWORD get_persistent_styles_(bool is_extended) const;

		virtual DWORD get_filtered_styles_(bool is_extended) const;

		virtual HWND get_handle_() const;

		virtual POINT get_context_menu_position_() const;

		virtual const std::wstring &get_class_name_() const;

		virtual const wchar_t *get_window_text_() const;

		virtual const wchar_t *get_theme_id_() const;

		virtual HINSTANCE get_instance_() const;

		virtual bool is_top_level_() const;

		virtual bool traverse_child_windows_(const tree &parent, const std::function<bool(window_surface &)> &callback) const;

		HWND handle_ = nullptr;
		WNDPROC entry_ = DefWindowProcW;

		DWORD styles_ = 0;
		DWORD extended_styles_ = 0;

		mutable menu::system_popup system_menu_;
		mutable menu::bar menu_bar_;

		bool updating_dimension_ = false;
	};
}
