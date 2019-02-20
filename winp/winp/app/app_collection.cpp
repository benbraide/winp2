#include "app_collection.h"

winp::app::main_object *winp::app::collection::get_main(){
	return main_;
}

void winp::app::collection::traverse_objects(const std::function<void(object &)> &callback){
	std::unordered_map<object *, char> list;
	{//Scoped lock
		std::lock_guard<std::mutex> guard(lock_);
		list = list_;
	}

	for (auto &item : list)
		callback(*item.first);
}

void winp::app::collection::add_(object &item){
	std::lock_guard<std::mutex> guard(lock_);
	list_[&item] = '\0';
}

void winp::app::collection::remove_(object &item){
	std::lock_guard<std::mutex> guard(lock_);
	if (!list_.empty())
		list_.erase(&item);
}

std::unordered_map<winp::app::object *, char> winp::app::collection::list_;

winp::app::main_object *winp::app::collection::main_ = nullptr;

std::mutex winp::app::collection::lock_;
