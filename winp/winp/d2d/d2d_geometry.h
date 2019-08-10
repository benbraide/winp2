#pragma once

#include <memory>

#include "../utility/windows.h"

namespace winp::d2d{
	class geometry{
	public:
		enum class compare_result_type{
			nil,
			equal_shape_and_size,
			equal_shape_and_size_and_offset,
		};

		geometry();

		geometry(const geometry &other);

		geometry(geometry &&other);

		explicit geometry(ID2D1Geometry *value, bool is_managed = false);

		explicit geometry(nullptr_t);

		geometry &operator =(const geometry &other);

		geometry &operator =(geometry &&other);

		geometry &operator =(ID2D1Geometry *value);

		geometry &operator =(nullptr_t);

		operator ID2D1Geometry *() const;

		operator bool() const;

		bool operator ==(const geometry &target) const;

		bool operator ==(ID2D1Geometry *target) const;

		bool operator ==(nullptr_t) const;

		bool operator !=(const geometry &target) const;

		bool operator !=(ID2D1Geometry *target) const;

		bool operator !=(nullptr_t) const;

		void reset(ID2D1Geometry *value, bool is_managed = false);

		ID2D1Geometry *get() const;

		geometry get_copy() const;

		bool copy(ID2D1Geometry *target);

		bool is_empty() const;

		RECT get_dimension() const;

		POINT get_offset() const;

		SIZE get_size() const;

		bool offset(const POINT &value);

		bool offset(int x, int y);

		bool move(const POINT &to);

		bool move(int to_x, int to_y);

		geometry get_combination(ID2D1Geometry *target, int mode) const;

		geometry get_combination(const RECT &dimension, int mode) const;

		bool combine_with(ID2D1Geometry *target, int mode);

		bool combine_with(const RECT &dimension, int mode);

		geometry get_intersection(ID2D1Geometry *target) const;

		geometry get_intersection(const RECT &dimension) const;

		bool intersect_with(ID2D1Geometry *target);

		bool intersect_with(const RECT &dimension);

		geometry get_union(ID2D1Geometry *target) const;

		geometry get_union(const RECT &dimension) const;

		bool union_with(ID2D1Geometry *target);

		bool union_with(const RECT &dimension);

		geometry get_exclusion(ID2D1Geometry *target) const;

		geometry get_exclusion(const RECT &dimension) const;

		bool exclude(ID2D1Geometry *target);

		bool exclude(const RECT &dimension);

		compare_result_type compare(const geometry &target) const;

		friend bool operator ==(ID2D1Geometry *target, const geometry &right);

		friend bool operator ==(nullptr_t, const geometry &right);

		friend bool operator !=(ID2D1Geometry *target, const geometry &right);

		friend bool operator !=(nullptr_t, const geometry &right);

	private:
		std::shared_ptr<ID2D1Geometry> value_;
	};
}
