#pragma once

#include "grid_column.h"

namespace winp::grid{
	class object;

	class row : public non_window::custom{
	public:
		row();

		explicit row(thread::object &thread);

		explicit row(ui::tree &parent);

		row(ui::tree &parent, std::size_t index);

		virtual ~row();

		virtual utility::error_code refresh(const std::function<void(row &, utility::error_code)> &callback = nullptr);

	protected:
		friend class column;
		friend class grid::object;

		virtual utility::error_code set_parent_value_(ui::tree *value, bool changing) override;

		virtual utility::error_code do_insert_child_(ui::object &child, std::size_t index) override;

		virtual void child_inserted_(ui::object &child) override;

		virtual void child_erased_(ui::object &child) override;

		virtual grid_type *get_grid_() const override;

		virtual int compute_fixed_height_(int grid_height) const;

		virtual bool is_fixed_() const;

		virtual void update_(int x, int y, int width, int height);

		virtual utility::error_code refresh_();

		bool is_updating_ = false;
	};

	class fixed_row : public row{
	public:
		fixed_row();

		explicit fixed_row(thread::object &thread);

		explicit fixed_row(ui::tree &parent);

		fixed_row(ui::tree &parent, std::size_t index);

		virtual ~fixed_row();

	protected:
		virtual utility::error_code set_dimension_(int x, int y, int width, int height) override;

		virtual int compute_fixed_height_(int grid_height) const override;

		virtual bool is_fixed_() const;
	};

	class proportional_row : public fixed_row{
	public:
		proportional_row();

		explicit proportional_row(thread::object &thread);

		explicit proportional_row(ui::tree &parent);

		proportional_row(ui::tree &parent, std::size_t index);

		virtual ~proportional_row();

		virtual utility::error_code set_proportion(float value, const std::function<void(proportional_row &, utility::error_code)> &callback = nullptr);

		virtual float get_proportion(const std::function<void(float)> &callback = nullptr) const;

	protected:
		virtual int compute_fixed_height_(int grid_height) const override;

		virtual utility::error_code set_proportion_(float value);

		float value_ = 0.0f;
	};

	class proportional_shared_row : public row{
	public:
		proportional_shared_row();

		explicit proportional_shared_row(thread::object &thread);

		explicit proportional_shared_row(ui::tree &parent);

		proportional_shared_row(ui::tree &parent, std::size_t index);

		virtual ~proportional_shared_row();

		virtual utility::error_code set_proportion(float value, const std::function<void(proportional_shared_row &, utility::error_code)> &callback = nullptr);

		virtual float get_proportion(const std::function<void(float)> &callback = nullptr) const;

	protected:
		friend class grid::object;

		virtual int compute_fixed_height_(int shared_row_height) const override;

		virtual utility::error_code set_proportion_(float value);

		float value_ = 0.0f;
	};
}
