#include "d2d_geometry.h"

winp::d2d::geometry::geometry(){}

winp::d2d::geometry::geometry(const geometry &other)
	: value_(other.value_){}

winp::d2d::geometry::geometry(geometry &&other)
	: value_(std::move(other.value_)){}

winp::d2d::geometry::geometry(ID2D1Geometry *value, bool is_managed){
	value_.reset(value, [this, is_managed](ID2D1Geometry *value){
		if (is_managed && value != nullptr)
			DeleteObject(value);
	});
}

winp::d2d::geometry::geometry(nullptr_t)
	: value_(nullptr){}

winp::d2d::geometry &winp::d2d::geometry::operator =(const geometry &other){
	value_ = other.value_;
	return *this;
}

winp::d2d::geometry &winp::d2d::geometry::operator =(geometry &&other){
	value_ = std::move(other.value_);
	return *this;
}

winp::d2d::geometry &winp::d2d::geometry::operator =(ID2D1Geometry *value){
	reset(value, true);
	return *this;
}

winp::d2d::geometry &winp::d2d::geometry::operator=(nullptr_t){
	value_ = nullptr;
	return *this;
}

winp::d2d::geometry::operator ID2D1Geometry *() const{
	return value_.get();
}

winp::d2d::geometry::operator bool() const{
	return (value_ != nullptr);
}

bool winp::d2d::geometry::operator ==(const geometry &target) const{
	return (*this == target.value_.get());
}

bool winp::d2d::geometry::operator ==(ID2D1Geometry *target) const{
	return (value_.get() == target);
}

bool winp::d2d::geometry::operator ==(nullptr_t target) const{
	return (value_ == nullptr);
}

bool winp::d2d::geometry::operator !=(const geometry &target) const{
	return !(*this == target);
}

bool winp::d2d::geometry::operator !=(ID2D1Geometry *target) const{
	return !(*this == target);
}

bool winp::d2d::geometry::operator !=(nullptr_t) const{
	return (value_ != nullptr);
}

void winp::d2d::geometry::reset(ID2D1Geometry *value, bool is_managed){
	value_.reset(value, [this, is_managed](ID2D1Geometry *value){
		if (is_managed && value != nullptr)
			value->Release();
	});
}

ID2D1Geometry *winp::d2d::geometry::get() const{
	return value_.get();
}

winp::d2d::geometry winp::d2d::geometry::get_copy() const{
	return geometry();
}

bool winp::d2d::geometry::copy(ID2D1Geometry *target){
	return false;
}

bool winp::d2d::geometry::is_empty() const{
	return false;
}

RECT winp::d2d::geometry::get_dimension() const{
	RECT dimension{};
	if (value_ != nullptr){
		
	}

	return dimension;
}

POINT winp::d2d::geometry::get_offset() const{
	auto dimension = get_dimension();
	return POINT{ dimension.left, dimension.top };
}

SIZE winp::d2d::geometry::get_size() const{
	auto dimension = get_dimension();
	return SIZE{ (dimension.right - dimension.left), (dimension.bottom - dimension.top) };
}

bool winp::d2d::geometry::offset(const POINT &value){
	return offset(value.x, value.y);
}

bool winp::d2d::geometry::offset(int x, int y){
	return false;
}

bool winp::d2d::geometry::move(const POINT &to){
	return move(to.x, to.y);
}

bool winp::d2d::geometry::move(int to_x, int to_y){
	return false;
}

winp::d2d::geometry winp::d2d::geometry::get_combination(ID2D1Geometry *target, int mode) const{
	return geometry();
}

winp::d2d::geometry winp::d2d::geometry::get_combination(const RECT &dimension, int mode) const{
	return geometry();
}

bool winp::d2d::geometry::combine_with(ID2D1Geometry *target, int mode){
	return false;
}

bool winp::d2d::geometry::combine_with(const RECT &dimension, int mode){
	return false;
}

winp::d2d::geometry winp::d2d::geometry::get_intersection(ID2D1Geometry *target) const{
	return geometry();
}

winp::d2d::geometry winp::d2d::geometry::get_intersection(const RECT &dimension) const{
	return geometry();
}

bool winp::d2d::geometry::intersect_with(ID2D1Geometry *target){
	return false;
}

bool winp::d2d::geometry::intersect_with(const RECT &dimension){
	return false;
}

winp::d2d::geometry winp::d2d::geometry::get_union(ID2D1Geometry *target) const{
	return geometry();
}

winp::d2d::geometry winp::d2d::geometry::get_union(const RECT &dimension) const{
	return geometry();
}

bool winp::d2d::geometry::union_with(ID2D1Geometry *target){
	return false;
}

bool winp::d2d::geometry::union_with(const RECT &dimension){
	return false;
}

winp::d2d::geometry winp::d2d::geometry::get_exclusion(ID2D1Geometry *target) const{
	return geometry();
}

winp::d2d::geometry winp::d2d::geometry::get_exclusion(const RECT &dimension) const{
	return geometry();
}

bool winp::d2d::geometry::exclude(ID2D1Geometry *target){
	return false;
}

bool winp::d2d::geometry::exclude(const RECT &dimension){
	return false;
}

winp::d2d::geometry::compare_result_type winp::d2d::geometry::compare(const geometry &target) const{
	return compare_result_type::nil;
}

bool winp::d2d::operator ==(ID2D1Geometry *target, const geometry &right){
	return (right == target);
}

bool winp::d2d::operator ==(nullptr_t, const geometry &right){
	return (right == nullptr);
}

bool winp::d2d::operator!=(ID2D1Geometry *target, const geometry &right){
	return (right != target);
}

bool winp::d2d::operator !=(nullptr_t, const geometry &right){
	return (right != nullptr);
}
