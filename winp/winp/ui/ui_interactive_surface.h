#pragma once

#include "ui_visible_surface.h"

namespace winp::ui{
	class interactive_surface : public visible_surface{
	public:
		virtual ~interactive_surface();

		virtual bool is_dialog_message(MSG &msg, const std::function<void(bool)> &callback = nullptr) const;

	protected:
		friend class thread::item_manager;

		virtual bool is_dialog_message_(MSG &msg) const;

		virtual interactive_surface *get_deepest_moused_() const;

		virtual interactive_surface *find_deepest_moused_(const POINT &absolute_position) const;

		virtual void create_moused_link_with_ancestors_();

		virtual void remove_moused_link_();

		interactive_surface *moused_ = nullptr;
	};
}
