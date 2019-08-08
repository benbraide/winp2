#include "rgn.h"

winp::utility::rgn::rgn(){}

winp::utility::rgn::rgn(const rgn &other)
	: value_(other.value_){}

winp::utility::rgn::rgn(rgn &&other)
	: value_(std::move(other.value_)){}

winp::utility::rgn::rgn(HRGN value, bool is_managed){
	value_.reset(value, [this, is_managed](HRGN value){
		if (is_managed && value != nullptr)
			DeleteObject(value);
	});
}

winp::utility::rgn::rgn(nullptr_t)
	: value_(nullptr){}

winp::utility::rgn &winp::utility::rgn::operator =(const rgn &other){
	value_ = other.value_;
	return *this;
}

winp::utility::rgn &winp::utility::rgn::operator =(rgn &&other){
	value_ = std::move(other.value_);
	return *this;
}

winp::utility::rgn &winp::utility::rgn::operator =(HRGN value){
	reset(value, true);
	return *this;
}

winp::utility::rgn &winp::utility::rgn::operator=(nullptr_t){
	value_ = nullptr;
	return *this;
}

winp::utility::rgn::operator HRGN() const{
	return value_.get();
}

winp::utility::rgn::operator bool() const{
	return (value_ != nullptr);
}

bool winp::utility::rgn::operator ==(const rgn &target) const{
	return (*this == target.value_.get());
}

bool winp::utility::rgn::operator ==(HRGN target) const{
	return (value_.get() == target);
}

bool winp::utility::rgn::operator ==(nullptr_t target) const{
	return (value_ == nullptr);
}

bool winp::utility::rgn::operator !=(const rgn &target) const{
	return !(*this == target);
}

bool winp::utility::rgn::operator !=(HRGN target) const{
	return !(*this == target);
}

bool winp::utility::rgn::operator !=(nullptr_t) const{
	return (value_ != nullptr);
}

void winp::utility::rgn::reset(HRGN value, bool is_managed){
	value_.reset(value, [this, is_managed](HRGN value){
		if (is_managed && value != nullptr)
			DeleteObject(value);
	});
}

HRGN winp::utility::rgn::get() const{
	return value_.get();
}

winp::utility::rgn winp::utility::rgn::get_copy() const{
	auto destination = CreateRectRgn(0, 0, 1, 1);
	if (destination == nullptr)//Failed to create destination region
		return rgn();

	if (CombineRgn(destination, value_.get(), nullptr, RGN_COPY) == RGN_ERROR){
		DeleteObject(destination);
		destination = nullptr;
	}

	return rgn(destination, true);
}

bool winp::utility::rgn::copy(HRGN target){
	if (value_ == nullptr)//Create dummy region
		reset(CreateRectRgn(0, 0, 1, 1), true);
	return (value_ != nullptr && target != nullptr && CombineRgn(value_.get(), target, nullptr, RGN_COPY) != RGN_ERROR);
}

bool winp::utility::rgn::is_empty() const{
	auto dimension = get_dimension();
	return (IsRectEmpty(&dimension) != FALSE);
}

RECT winp::utility::rgn::get_dimension() const{
	RECT dimension{};
	if (value_ != nullptr)
		GetRgnBox(value_.get(), &dimension);
	return dimension;
}

POINT winp::utility::rgn::get_offset() const{
	auto dimension = get_dimension();
	return POINT{ dimension.left, dimension.top };
}

SIZE winp::utility::rgn::get_size() const{
	auto dimension = get_dimension();
	return SIZE{ (dimension.right - dimension.left), (dimension.bottom - dimension.top) };
}

bool winp::utility::rgn::offset(const POINT &value){
	return offset(value.x, value.y);
}

bool winp::utility::rgn::offset(int x, int y){
	return (value_ != nullptr && OffsetRgn(value_.get(), x, y) != RGN_ERROR);
}

bool winp::utility::rgn::move(const POINT &to){
	return move(to.x, to.y);
}

bool winp::utility::rgn::move(int to_x, int to_y){
	auto dimension = get_dimension();
	return offset((to_x - dimension.left), (to_y - dimension.top));
}

winp::utility::rgn winp::utility::rgn::get_combination(HRGN target, int mode) const{
	if (value_ == nullptr || target == nullptr)
		return rgn();

	auto destination = CreateRectRgn(0, 0, 1, 1);
	if (destination == nullptr)//Failed to create destination region
		return rgn();

	if (CombineRgn(destination, value_.get(), target, mode) == RGN_ERROR){
		DeleteObject(destination);
		destination = nullptr;
	}

	return rgn(destination, true);
}

winp::utility::rgn winp::utility::rgn::get_combination(const RECT &dimension, int mode) const{
	if (value_ == nullptr)
		return rgn();

	auto target = CreateRectRgn(dimension.left, dimension.top, dimension.right, dimension.bottom);
	if (target == nullptr)//Failed to create
		return rgn();

	if (CombineRgn(target, value_.get(), target, mode) == RGN_ERROR){
		DeleteObject(target);
		target = nullptr;
	}

	return rgn(target, true);
}

bool winp::utility::rgn::combine_with(HRGN target, int mode){
	return (value_ != nullptr && target != nullptr && CombineRgn(value_.get(), value_.get(), target, mode) != RGN_ERROR);
}

bool winp::utility::rgn::combine_with(const RECT &dimension, int mode){
	auto rgn = CreateRectRgn(dimension.left, dimension.top, dimension.right, dimension.bottom);
	if (rgn == nullptr)//Failed to create
		return false;

	auto result = CombineRgn(value_.get(), value_.get(), rgn, mode);
	DeleteObject(rgn);

	return (result != RGN_ERROR);
}

winp::utility::rgn winp::utility::rgn::get_intersection(HRGN target) const{
	return get_combination(target, RGN_AND);
}

winp::utility::rgn winp::utility::rgn::get_intersection(const RECT &dimension) const{
	return get_combination(dimension, RGN_AND);
}

bool winp::utility::rgn::intersect_with(HRGN target){
	return combine_with(target, RGN_AND);
}

bool winp::utility::rgn::intersect_with(const RECT &dimension){
	return combine_with(dimension, RGN_AND);
}

winp::utility::rgn winp::utility::rgn::get_union(HRGN target) const{
	return get_combination(target, RGN_OR);
}

winp::utility::rgn winp::utility::rgn::get_union(const RECT &dimension) const{
	return get_combination(dimension, RGN_OR);
}

bool winp::utility::rgn::union_with(HRGN target){
	return combine_with(target, RGN_OR);
}

bool winp::utility::rgn::union_with(const RECT &dimension){
	return combine_with(dimension, RGN_OR);
}

winp::utility::rgn winp::utility::rgn::get_exclusion(HRGN target) const{
	return get_combination(target, RGN_DIFF);
}

winp::utility::rgn winp::utility::rgn::get_exclusion(const RECT &dimension) const{
	return get_combination(dimension, RGN_DIFF);
}

bool winp::utility::rgn::exclude(HRGN target){
	return combine_with(target, RGN_DIFF);
}

bool winp::utility::rgn::exclude(const RECT &dimension){
	return combine_with(dimension, RGN_DIFF);
}

winp::utility::rgn::compare_result_type winp::utility::rgn::compare(const rgn &target) const{
	if (value_ == nullptr || target == nullptr || EqualRgn(value_.get(), target) == FALSE)
		return compare_result_type::nil;
	
	auto offset = get_offset(), target_offset = target.get_offset();
	return ((offset.x == target_offset.x && offset.y == target_offset.y) ? compare_result_type::equal_shape_and_size_and_offset : compare_result_type::equal_shape_and_size);

}

bool winp::utility::operator ==(HRGN target, const rgn &right){
	return (right == target);
}

bool winp::utility::operator ==(nullptr_t, const rgn &right){
	return (right == nullptr);
}

bool winp::utility::operator!=(HRGN target, const rgn &right){
	return (right != target);
}

bool winp::utility::operator !=(nullptr_t, const rgn &right){
	return (right != nullptr);
}
