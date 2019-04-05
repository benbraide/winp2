#pragma once

#include "control_object.h"
#include "tool_tip_control_item.h"

namespace winp::control{
	class tool_tip : public object{
	public:
		tool_tip();

		explicit tool_tip(thread::object &thread);

		explicit tool_tip(tree &parent);

		tool_tip(tree &parent, std::size_t index);

		virtual ~tool_tip();

		virtual utility::error_code set_max_width(int value, const std::function<void(tool_tip &, utility::error_code)> &callback = nullptr);

		virtual int get_max_width(const std::function<void(int)> &callback = nullptr) const;

		virtual utility::error_code set_font(HFONT value, const std::function<void(tool_tip &, utility::error_code)> &callback = nullptr);

		virtual HFONT get_font(const std::function<void(HFONT)> &callback = nullptr) const;

	protected:
		friend class tool_tip_item;

		virtual DWORD get_persistent_styles_(bool is_extended) const override;

		virtual bool is_top_level_() const override;

		virtual LRESULT dispatch_notification_(MSG &msg) const override;

		virtual utility::error_code set_max_width_(int value);

		virtual utility::error_code set_font_(HFONT value);

		virtual tool_tip_item *get_current_item_() const;

		int max_width_ = 0;
		HFONT font_ = nullptr;
	};
}
