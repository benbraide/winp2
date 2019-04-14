#pragma once

#include "../thread/thread_item.h"

#include "ui_object_collection.h"

namespace winp::grid{
	class object;
}

namespace winp::ui{
	class tree;

	class surface : public thread::synchronized_item{
	public:
		using grid_type = object_collection<grid::object>;

		virtual ~surface();

		virtual utility::error_code set_size(const SIZE &value, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code set_size(int width, int height, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code set_width(int value, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code set_height(int value, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code offset_size(const SIZE &value, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code offset_size(int width, int height, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code offset_width(int value, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code offset_height(int value, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual const SIZE &get_size(const std::function<void(const SIZE &)> &callback = nullptr) const;

		virtual int get_width(const std::function<void(int)> &callback = nullptr) const;

		virtual int get_height(const std::function<void(int)> &callback = nullptr) const;

		virtual utility::error_code set_position(const POINT &value, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code set_position(int x, int y, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code set_x_position(int value, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code set_y_position(int value, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code offset_position(const POINT &value, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code offset_position(int x, int y, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code offset_x_position(int value, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code offset_y_position(int value, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual const POINT &get_position(const std::function<void(const POINT &)> &callback = nullptr) const;

		virtual int get_x_position(const std::function<void(int)> &callback = nullptr) const;

		virtual int get_y_position(const std::function<void(int)> &callback = nullptr) const;

		virtual utility::error_code set_absolute_position(const POINT &value, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code set_absolute_position(int x, int y, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual POINT get_absolute_position(const std::function<void(const POINT &)> &callback = nullptr) const;

		virtual int get_absolute_x_position(const std::function<void(int)> &callback = nullptr) const;

		virtual int get_absolute_y_position(const std::function<void(int)> &callback = nullptr) const;

		virtual utility::error_code set_dimension(const RECT &value, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code set_dimension(const POINT &position, const SIZE &size, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code set_dimension(int x, int y, int width, int height, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

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

		virtual UINT hit_test(const POINT &value, const std::function<void(UINT)> &callback = nullptr) const;

		virtual UINT hit_test(int x, int y, const std::function<void(UINT)> &callback = nullptr) const;

		virtual UINT absolute_hit_test(const POINT &value, const std::function<void(UINT)> &callback = nullptr) const;

		virtual UINT absolute_hit_test(int x, int y, const std::function<void(UINT)> &callback = nullptr) const;

		virtual grid_type &get_grid(const std::function<void(const grid_type &)> &callback = nullptr) const;

		virtual bool has_grid(const std::function<void(bool)> &callback = nullptr) const;

	protected:
		explicit surface(tree *tree_self = nullptr);

		virtual utility::error_code set_size_(int width, int height);

		virtual utility::error_code set_width_(int value);

		virtual utility::error_code set_height_(int value);

		virtual utility::error_code offset_size_(int width, int height);

		virtual utility::error_code offset_width_(int value);

		virtual utility::error_code offset_height_(int value);

		virtual const SIZE &get_size_() const;

		virtual utility::error_code set_position_(int x, int y);

		virtual utility::error_code set_x_position_(int value);

		virtual utility::error_code set_y_position_(int value);

		virtual utility::error_code offset_position_(int x, int y);

		virtual utility::error_code offset_x_position_(int value);

		virtual utility::error_code offset_y_position_(int value);

		virtual const POINT &get_position_() const;

		virtual utility::error_code set_absolute_position_(int x, int y);

		virtual POINT get_absolute_position_() const;

		virtual POINT get_client_offset_() const;

		virtual utility::error_code set_dimension_(int x, int y, int width, int height);

		virtual utility::error_code position_change_(bool is_changing);

		virtual utility::error_code size_change_(bool is_changing);

		virtual RECT get_dimension_() const;

		virtual RECT get_absolute_dimension_() const;

		virtual POINT convert_position_from_absolute_value_(int x, int y) const;

		virtual POINT convert_position_to_absolute_value_(int x, int y) const;

		virtual RECT convert_dimension_from_absolute_value_(const RECT &value) const;

		virtual RECT convert_dimension_to_absolute_value_(const RECT &value) const;

		virtual UINT hit_test_(int x, int y) const;

		virtual UINT absolute_hit_test_(int x, int y) const;

		SIZE size_{};
		POINT position_{};
		std::shared_ptr<grid_type> grid_;
	};
}
