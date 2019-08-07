#include "helper.h"

bool winp::utility::helper::rgn_is_empty(HRGN target){
	auto dimension = get_rgn_dimension(target);
	return (IsRectEmpty(&dimension) != FALSE);
}

bool winp::utility::helper::rgns_are_equal(HRGN first, HRGN second, bool compare_offsets){
	if (EqualRgn(first, second) == FALSE)
		return false;

	if (!compare_offsets)
		return true;

	auto first_offset = get_rgn_offset(first), second_offset = get_rgn_offset(second);
	return (first_offset.x == second_offset.x && first_offset.y == second_offset.y);
}

RECT winp::utility::helper::get_rgn_dimension(HRGN target){
	RECT dimension{};
	GetRgnBox(target, &dimension);
	return dimension;
}

POINT winp::utility::helper::get_rgn_offset(HRGN target){
	RECT dimension{};
	GetRgnBox(target, &dimension);
	return POINT{ dimension.left, dimension.top };
}

SIZE winp::utility::helper::get_rgn_size(HRGN target){
	RECT dimension{};
	GetRgnBox(target, &dimension);
	return SIZE{ (dimension.right - dimension.left), (dimension.bottom - dimension.top) };
}

bool winp::utility::helper::offset_rgn(HRGN target, const POINT &value){
	return offset_rgn(target, value.x, value.y);
}

bool winp::utility::helper::offset_rgn(HRGN target, int x, int y){
	return (OffsetRgn(target, x, y) != RGN_ERROR);
}

bool winp::utility::helper::move_rgn(HRGN target, const POINT &to){
	return move_rgn(target, to.x, to.y);
}

bool winp::utility::helper::move_rgn(HRGN target, int to_x, int to_y){
	RECT dimension{};
	GetRgnBox(target, &dimension);
	return (OffsetRgn(target, (to_x - dimension.left), (to_y - dimension.top)) != RGN_ERROR);
}

HRGN winp::utility::helper::combine_rgn(HRGN first, HRGN second, int mode){
	auto destination = CreateRectRgn(0, 0, 1, 1);
	if (destination == nullptr)//Failed to create destination region
		return nullptr;

	if (CombineRgn(destination, first, second, mode) == RGN_ERROR){
		DeleteObject(destination);
		destination = nullptr;
	}

	return destination;
}

bool winp::utility::helper::combine_rgn_alt(HRGN first, HRGN second, int mode){
	return (CombineRgn(first, first, second, mode) != RGN_ERROR);
}

HRGN winp::utility::helper::copy_rgn(HRGN value){
	return combine_rgn(value, nullptr, RGN_COPY);
}

HRGN winp::utility::helper::intersect_rgn(HRGN first, HRGN second){
	return combine_rgn(first, second, RGN_AND);
}

HRGN winp::utility::helper::intersect_rgn(HRGN value, const RECT &dimension){
	auto rgn = CreateRectRgn(dimension.left, dimension.top, dimension.right, dimension.bottom);
	if (rgn == nullptr)//Failed to create
		return false;

	auto result = intersect_rgn(value, rgn);
	DeleteObject(rgn);

	return result;
}

bool winp::utility::helper::intersect_rgn_alt(HRGN first, HRGN second){
	return combine_rgn_alt(first, second, RGN_AND);
}

bool winp::utility::helper::intersect_rgn_alt(HRGN value, const RECT &dimension){
	auto rgn = CreateRectRgn(dimension.left, dimension.top, dimension.right, dimension.bottom);
	if (rgn == nullptr)//Failed to create
		return false;

	auto result = intersect_rgn_alt(value, rgn);
	DeleteObject(rgn);

	return result;
}

HRGN winp::utility::helper::exclude_rgn(HRGN first, HRGN second){
	return combine_rgn(first, second, RGN_DIFF);
}

HRGN winp::utility::helper::exclude_rgn(HRGN value, const RECT &dimension){
	auto rgn = CreateRectRgn(dimension.left, dimension.top, dimension.right, dimension.bottom);
	if (rgn == nullptr)//Failed to create
		return false;

	auto result = exclude_rgn(value, rgn);
	DeleteObject(rgn);

	return result;
}

bool winp::utility::helper::exclude_rgn_alt(HRGN first, HRGN second){
	return combine_rgn_alt(first, second, RGN_DIFF);
}

bool winp::utility::helper::exclude_rgn_alt(HRGN value, const RECT &dimension){
	auto rgn = CreateRectRgn(dimension.left, dimension.top, dimension.right, dimension.bottom);
	if (rgn == nullptr)//Failed to create
		return false;

	auto result = exclude_rgn_alt(value, rgn);
	DeleteObject(rgn);

	return result;
}

HRGN winp::utility::helper::union_rgn(HRGN first, HRGN second){
	return combine_rgn(first, second, RGN_OR);
}

HRGN winp::utility::helper::union_rgn(HRGN value, const RECT &dimension){
	auto rgn = CreateRectRgn(dimension.left, dimension.top, dimension.right, dimension.bottom);
	if (rgn == nullptr)//Failed to create
		return false;

	auto result = union_rgn(value, rgn);
	DeleteObject(rgn);

	return result;
}

bool winp::utility::helper::union_rgn_alt(HRGN first, HRGN second){
	return combine_rgn_alt(first, second, RGN_OR);
}

bool winp::utility::helper::union_rgn_alt(HRGN value, const RECT &dimension){
	auto rgn = CreateRectRgn(dimension.left, dimension.top, dimension.right, dimension.bottom);
	if (rgn == nullptr)//Failed to create
		return false;

	auto result = union_rgn_alt(value, rgn);
	DeleteObject(rgn);

	return result;
}
