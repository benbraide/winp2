#pragma once

#include <memory>

#include "windows.h"

namespace winp::utility{
	class rgn{
	public:
		using base_type = std::remove_pointer_t<HRGN>;

		enum class compare_result_type{
			nil,
			equal_shape_and_size,
			equal_shape_and_size_and_offset,
		};

		rgn();

		rgn(const rgn &other);

		rgn(rgn &&other);

		explicit rgn(HRGN value, bool is_managed = false);

		explicit rgn(nullptr_t);

		rgn &operator =(const rgn &other);

		rgn &operator =(rgn &&other);

		rgn &operator =(HRGN value);

		rgn &operator =(nullptr_t);

		operator HRGN() const;

		operator bool() const;

		bool operator ==(const rgn &target) const;

		bool operator ==(HRGN target) const;

		bool operator ==(nullptr_t) const;

		bool operator !=(const rgn &target) const;

		bool operator !=(HRGN target) const;

		bool operator !=(nullptr_t) const;

		void reset(HRGN value, bool is_managed = false);

		HRGN get() const;

		rgn get_copy() const;

		bool copy(HRGN target);

		bool is_empty() const;

		RECT get_dimension() const;

		POINT get_offset() const;

		SIZE get_size() const;

		bool offset(const POINT &value);

		bool offset(int x, int y);

		bool move(const POINT &to);

		bool move(int to_x, int to_y);

		rgn get_combination(HRGN target, int mode) const;

		rgn get_combination(const RECT &dimension, int mode) const;

		bool combine_with(HRGN target, int mode);

		bool combine_with(const RECT &dimension, int mode);

		rgn get_intersection(HRGN target) const;

		rgn get_intersection(const RECT &dimension) const;

		bool intersect_with(HRGN target);

		bool intersect_with(const RECT &dimension);

		rgn get_union(HRGN target) const;

		rgn get_union(const RECT &dimension) const;

		bool union_with(HRGN target);

		bool union_with(const RECT &dimension);

		rgn get_exclusion(HRGN target) const;

		rgn get_exclusion(const RECT &dimension) const;

		bool exclude(HRGN target);

		bool exclude(const RECT &dimension);

		compare_result_type compare(const rgn &target) const;

		friend bool operator ==(HRGN target, const rgn &right);

		friend bool operator ==(nullptr_t, const rgn &right);

		friend bool operator !=(HRGN target, const rgn &right);

		friend bool operator !=(nullptr_t, const rgn &right);

	private:
		std::shared_ptr<base_type> value_;
	};
}
