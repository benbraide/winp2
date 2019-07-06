#pragma once

#include "control_object.h"
#include "tab_control_page.h"

namespace winp::control{
	class tab : public object{
	public:
		tab();

		explicit tab(tree &parent);

		tab(tree &parent, std::size_t index);

		virtual ~tab();

		virtual utility::error_code set_active_page(tab_page &value, const std::function<void(tab &, utility::error_code)> &callback = nullptr);

		virtual tab_page *get_active_page(const std::function<void(tab_page *)> &callback = nullptr) const;

	protected:
		friend class tab_page;

		virtual utility::error_code do_insert_child_(ui::object &child, std::size_t index) override;

		virtual SIZE get_client_size_() const override;

		virtual SIZE get_current_client_size_() const override;

		virtual POINT get_client_offset_() const override;

		virtual POINT get_client_start_offset_() const override;

		virtual LRESULT dispatch_notification_(MSG &msg) const override;

		virtual utility::error_code set_active_page_(tab_page &value, int index);

		virtual tab_page *get_active_page_() const;
	};
}
