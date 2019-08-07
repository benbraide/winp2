#pragma once

#include "windows.h"

namespace winp::utility{
	class helper{
	public:
		static bool rgn_is_empty(HRGN target);

		static bool rgns_are_equal(HRGN first, HRGN second, bool compare_offsets);

		static RECT get_rgn_dimension(HRGN target);

		static POINT get_rgn_offset(HRGN target);

		static SIZE get_rgn_size(HRGN target);

		static bool offset_rgn(HRGN target, const POINT &value);

		static bool offset_rgn(HRGN target, int x, int y);

		static bool move_rgn(HRGN target, const POINT &to);

		static bool move_rgn(HRGN target, int to_x, int to_y);

		static HRGN combine_rgn(HRGN first, HRGN second, int mode);

		static bool combine_rgn_alt(HRGN first, HRGN second, int mode);

		static HRGN copy_rgn(HRGN value);

		static HRGN intersect_rgn(HRGN first, HRGN second);

		static HRGN intersect_rgn(HRGN value, const RECT &dimension);

		static bool intersect_rgn_alt(HRGN first, HRGN second);

		static bool intersect_rgn_alt(HRGN value, const RECT &dimension);

		static HRGN exclude_rgn(HRGN first, HRGN second);

		static HRGN exclude_rgn(HRGN value, const RECT &dimension);

		static bool exclude_rgn_alt(HRGN first, HRGN second);

		static bool exclude_rgn_alt(HRGN value, const RECT &dimension);

		static HRGN union_rgn(HRGN first, HRGN second);

		static HRGN union_rgn(HRGN value, const RECT &dimension);

		static bool union_rgn_alt(HRGN first, HRGN second);

		static bool union_rgn_alt(HRGN value, const RECT &dimension);
	};
}
