#pragma once

#include "ui_visible_surface.h"

namespace winp::ui{
	class interactive_surface : public visible_surface{
	public:
		virtual ~interactive_surface();

		virtual bool is_dialog_message(MSG &msg, const std::function<void(bool)> &callback = nullptr) const;

	protected:
		virtual bool is_dialog_message_(MSG &msg) const;
	};
}
