#pragma once

#include "grid_row.h"

namespace winp::grid{
	class object : public non_window::custom{
	public:
		object();

		explicit object(ui::tree &parent);

		object(ui::tree &parent, std::size_t index);

		virtual ~object();

		virtual utility::error_code refresh(const std::function<void(object &, utility::error_code)> &callback = nullptr);

	protected:
		friend class row;

		virtual utility::error_code do_insert_child_(ui::object &child, std::size_t index) override;

		virtual void child_inserted_(ui::object &child) override;

		virtual void child_erased_(ui::object &child) override;

		virtual utility::error_code update_dimension_(const RECT &previous_dimension, int x, int y, int width, int height, UINT flags) override;

		virtual grid_type *get_grid_() const override;

		virtual utility::error_code refresh_();

		bool is_updating_ = false;
	};
}
