#pragma once

#include <unordered_map>

#include "../ui/ui_non_window_surface.h"

namespace winp::grid{
	class row;

	class column : public ui::non_window_surface{
	public:
		column();

		explicit column(ui::tree &parent);

		column(ui::tree &parent, std::size_t index);

		virtual ~column();

		virtual utility::error_code refresh(const std::function<void(column &, utility::error_code)> &callback = nullptr);

	protected:
		friend class row;

		virtual utility::error_code set_parent_value_(ui::tree *value, bool changing) override;

		virtual int compute_fixed_width_(int row_width) const;

		virtual bool is_fixed_() const;

		virtual void update_(int x, int y, int width, int height);

		virtual utility::error_code refresh_();

		bool is_updating_ = false;
	};

	class fixed_column : public column{
	public:
		fixed_column();

		explicit fixed_column(ui::tree &parent);

		fixed_column(ui::tree &parent, std::size_t index);

		virtual ~fixed_column();

	protected:
		virtual utility::error_code update_dimension_(const RECT &previous_dimension, int x, int y, int width, int height, UINT flags) override;

		virtual int compute_fixed_width_(int row_width) const override;

		virtual bool is_fixed_() const;
	};

	class proportional_column : public fixed_column{
	public:
		proportional_column();

		explicit proportional_column(ui::tree &parent);

		proportional_column(ui::tree &parent, std::size_t index);

		virtual ~proportional_column();

		virtual utility::error_code set_proportion(float value, const std::function<void(proportional_column &, utility::error_code)> &callback = nullptr);

		virtual float get_proportion(const std::function<void(float)> &callback = nullptr) const;

	protected:
		virtual int compute_fixed_width_(int row_width) const override;

		virtual utility::error_code set_proportion_(float value, bool allow_animation);

		float value_ = 0.0f;
	};

	class proportional_shared_column : public column{
	public:
		proportional_shared_column();

		explicit proportional_shared_column(ui::tree &parent);

		proportional_shared_column(ui::tree &parent, std::size_t index);

		virtual ~proportional_shared_column();

		virtual utility::error_code set_proportion(float value, const std::function<void(proportional_shared_column &, utility::error_code)> &callback = nullptr);

		virtual float get_proportion(const std::function<void(float)> &callback = nullptr) const;

	protected:
		friend class row;

		virtual int compute_fixed_width_(int shared_row_width) const override;

		virtual utility::error_code set_proportion_(float value);

		float value_ = 0.0f;
	};
}
