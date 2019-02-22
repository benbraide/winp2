#pragma once

#include "../thread/thread_item.h"

namespace winp::ui{
	class surface : public thread::synchronized_item{
	public:
		enum class hit_target{
			nil,
			error,
			inside,
			intersect,
		};

		virtual ~surface();

		virtual void set_size(const SIZE &value, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual void set_size(int width, int height, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual void set_width(int value, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual void set_height(int value, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual void offset_size(const SIZE &value, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual void offset_size(int width, int height, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual void offset_width(int value, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual void offset_height(int value, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual const SIZE &get_size(const std::function<void(const SIZE &)> &callback = nullptr) const;

		virtual int get_width(const std::function<void(int)> &callback = nullptr) const;

		virtual int get_height(const std::function<void(int)> &callback = nullptr) const;

		virtual void set_position(const POINT &value, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual void set_position(int x, int y, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual void set_x_position(int value, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual void set_y_position(int value, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual void offset_position(const POINT &value, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual void offset_position(int x, int y, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual void offset_x_position(int value, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual void offset_y_position(int value, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual const POINT &get_position(const std::function<void(const POINT &)> &callback = nullptr) const;

		virtual int get_x_position(const std::function<void(int)> &callback = nullptr) const;

		virtual int get_y_position(const std::function<void(int)> &callback = nullptr) const;

		virtual void set_absolute_position(const POINT &value, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual void set_absolute_position(int x, int y, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual POINT get_absolute_position(const std::function<void(const POINT &)> &callback = nullptr) const;

		virtual int get_absolute_x_position(const std::function<void(int)> &callback = nullptr) const;

		virtual int get_absolute_y_position(const std::function<void(int)> &callback = nullptr) const;

		virtual void set_dimension(const RECT &value, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual void set_dimension(const POINT &position, const SIZE &size, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual void set_dimension(int x, int y, int width, int height, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual RECT get_dimension(const std::function<void(const RECT &)> &callback = nullptr) const;

		virtual RECT get_absolute_dimension(const std::function<void(const RECT &)> &callback = nullptr) const;

		virtual POINT convert_position_from_absolute_value(const POINT &value, const std::function<void(const POINT &)> &callback = nullptr) const;

		virtual POINT convert_position_from_absolute_value(int x, int y, const std::function<void(const POINT &)> &callback = nullptr) const;

		virtual int convert_x_position_from_absolute_value(int value, const std::function<void(int)> &callback = nullptr) const;

		virtual int convert_y_position_from_absolute_value(int value, const std::function<void(int)> &callback = nullptr) const;

		virtual POINT convert_position_to_absolute_value(const POINT &value, const std::function<void(const POINT &)> &callback = nullptr) const;

		virtual POINT convert_position_to_absolute_value(int x, int y, const std::function<void(const POINT &)> &callback = nullptr) const;

		virtual int convert_x_position_to_absolute_value(int value, const std::function<void(int)> &callback = nullptr) const;

		virtual int convert_y_position_to_absolute_value(int value, const std::function<void(int)> &callback = nullptr) const;

		virtual RECT convert_dimension_from_absolute_value(const RECT &value, const std::function<void(const RECT &)> &callback = nullptr) const;

		virtual RECT convert_dimension_to_absolute_value(const RECT &value, const std::function<void(const RECT &)> &callback = nullptr) const;

		virtual hit_target hit_test(const POINT &value, const std::function<void(hit_target)> &callback = nullptr) const;

		virtual hit_target hit_test(int x, int y, const std::function<void(hit_target)> &callback = nullptr) const;

		virtual hit_target absolute_hit_test(const POINT &value, const std::function<void(hit_target)> &callback = nullptr) const;

		virtual hit_target absolute_hit_test(int x, int y, const std::function<void(hit_target)> &callback = nullptr) const;

	protected:
		virtual void set_size_(int width, int height);

		virtual void set_width_(int value);

		virtual void set_height_(int value);

		virtual void offset_size_(int width, int height);

		virtual void offset_width_(int value);

		virtual void offset_height_(int value);

		virtual const SIZE &get_size_() const;

		virtual void set_position_(int x, int y);

		virtual void set_x_position_(int value);

		virtual void set_y_position_(int value);

		virtual void offset_position_(int x, int y);

		virtual void offset_x_position_(int value);

		virtual void offset_y_position_(int value);

		virtual const POINT &get_position_() const;

		virtual void set_absolute_position_(int x, int y);

		virtual POINT get_absolute_position_() const;

		virtual void set_dimension_(int x, int y, int width, int height);

		virtual RECT get_dimension_() const;

		virtual RECT get_absolute_dimension_() const;

		virtual POINT convert_position_from_absolute_value_(int x, int y) const;

		virtual POINT convert_position_to_absolute_value_(int x, int y) const;

		virtual RECT convert_dimension_from_absolute_value_(const RECT &value) const;

		virtual RECT convert_dimension_to_absolute_value_(const RECT &value) const;

		virtual hit_target hit_test_(int x, int y) const;

		virtual hit_target absolute_hit_test_(int x, int y) const;

		SIZE size_{};
		POINT position_{};
	};
}