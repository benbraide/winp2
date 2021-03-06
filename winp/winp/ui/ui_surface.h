#pragma once

#include "../thread/thread_item.h"

#include "ui_tree.h"

namespace winp::grid{
	class object;
}

namespace winp::ui{
	class tree;
	class non_window_surface;

	class placement_hook;
	class parent_fill_hook;
	class children_contain_hook;
	class sibling_placement_hook;

	class drag_hook;
	class non_client_drag_hook;
	class edge_drag_hook;
	class drag_resize_hook;

	class surface : public thread::synchronized_item{
	public:
		using grid_type = grid::object;
		using easing_type = std::function<float(float)>;

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

		virtual const SIZE &get_current_size(const std::function<void(const SIZE &)> &callback = nullptr) const;

		virtual int get_width(const std::function<void(int)> &callback = nullptr) const;

		virtual int get_height(const std::function<void(int)> &callback = nullptr) const;

		virtual SIZE get_client_size(const std::function<void(const SIZE &)> &callback = nullptr) const;

		virtual SIZE get_current_client_size(const std::function<void(const SIZE &)> &callback = nullptr) const;

		virtual int get_client_width(const std::function<void(int)> &callback = nullptr) const;

		virtual int get_client_height(const std::function<void(int)> &callback = nullptr) const;

		virtual POINT get_client_offset(const std::function<void(const POINT &)> &callback = nullptr) const;

		virtual int get_client_x_offset(const std::function<void(int)> &callback = nullptr) const;

		virtual int get_client_y_offset(const std::function<void(int)> &callback = nullptr) const;

		virtual POINT get_client_start_offset(const std::function<void(const POINT &)> &callback = nullptr) const;

		virtual RECT get_client_padding(const std::function<void(const RECT &)> &callback = nullptr) const;

		virtual utility::error_code set_position(const POINT &value, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code set_position(int x, int y, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code set_x_position(int value, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code set_y_position(int value, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code offset_position(const POINT &value, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code offset_position(int x, int y, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code offset_x_position(int value, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code offset_y_position(int value, const std::function<void(surface &, utility::error_code)> &callback = nullptr);

		virtual const POINT &get_position(const std::function<void(const POINT &)> &callback = nullptr) const;

		virtual const POINT &get_current_position(const std::function<void(const POINT &)> &callback = nullptr) const;

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

		virtual RECT get_current_dimension(const std::function<void(const RECT &)> &callback = nullptr) const;

		virtual RECT get_absolute_dimension(const std::function<void(const RECT &)> &callback = nullptr) const;

		template <typename ancestor_type>
		POINT convert_position_relative_to_ancestor(const POINT &value, const std::function<void(const POINT &)> &callback = nullptr) const{
			return convert_position_relative_to_ancestor<ancestor_type>(value.x, value.y, callback);
		}

		template <typename ancestor_type>
		POINT convert_position_relative_to_ancestor(int x, int y, const std::function<void(const POINT &)> &callback = nullptr) const{
			return synchronized_item_compute_or_post_task_inside_thread_context([=]{
				return synchronized_item_pass_return_value_to_callback(callback, convert_position_relative_to_ancestor_<ancestor_type>(x, y));
			}, (callback != nullptr), POINT{});
		}

		template <typename ancestor_type>
		std::pair<ancestor_type *, POINT> get_first_ancestor_and_relative_offset(const std::function<void(const std::pair<ancestor_type *, POINT> &)> &callback = nullptr) const{
			return synchronized_item_compute_or_post_task_inside_thread_context([=]{
				return synchronized_item_pass_return_value_to_callback(callback, get_first_ancestor_and_relative_offset_<ancestor_type>());
			}, (callback != nullptr), std::make_pair<ancestor_type *, POINT>(nullptr, POINT{}));
		}

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

		virtual grid_type *get_grid(const std::function<void(grid_type &)> &callback = nullptr) const;

		virtual bool has_grid(const std::function<void(bool)> &callback = nullptr) const;

	protected:
		friend class non_window_surface;

		friend class ui::placement_hook;
		friend class ui::parent_fill_hook;

		friend class ui::sibling_placement_hook;
		friend class ui::children_contain_hook;

		friend class ui::drag_hook;
		friend class ui::non_client_drag_hook;
		friend class ui::edge_drag_hook;
		friend class ui::drag_resize_hook;

		friend class thread::item_manager;

		friend class events::draw;
		friend class events::erase_background;
		friend class events::paint;
		friend class events::non_client_paint;

		virtual utility::error_code set_size_(int width, int height, bool allow_animation);

		virtual const SIZE &get_size_() const;

		virtual const SIZE &get_current_size_() const;

		virtual SIZE get_client_size_() const;

		virtual SIZE get_current_client_size_() const;

		virtual POINT get_client_offset_() const;

		virtual POINT get_client_start_offset_() const;

		virtual RECT get_client_padding_() const;

		virtual utility::error_code set_position_(int x, int y, bool allow_animation);

		virtual const POINT &get_position_() const;

		virtual const POINT &get_current_position_() const;

		virtual utility::error_code set_absolute_position_(int x, int y, bool allow_animation);

		virtual POINT get_absolute_position_() const;

		virtual POINT get_current_absolute_position_() const;

		virtual utility::error_code set_dimension_(int x, int y, int width, int height, UINT flags, bool allow_animation);

		virtual utility::error_code animate_dimension_(object &object_self, animation_hook &hk, const RECT &previous_dimension, int x, int y, int width, int height, UINT flags);

		virtual utility::error_code update_dimension_(const RECT &previous_dimension, int x, int y, int width, int height, UINT flags);

		virtual RECT get_dimension_() const;

		virtual RECT get_current_dimension_() const;

		virtual RECT get_absolute_dimension_() const;

		virtual RECT get_current_absolute_dimension_() const;

		template <typename ancestor_type>
		POINT convert_position_relative_to_ancestor_(int x, int y) const{
			auto result = get_first_ancestor_and_relative_offset_<ancestor_type>();
			return POINT{ (x + result.second.x), (y + result.second.y) };
		}

		template <typename ancestor_type>
		std::pair<ancestor_type *, POINT> get_first_ancestor_and_relative_offset_() const{
			auto result = std::make_pair<ancestor_type *, POINT>(nullptr, POINT{});
			auto object_self = dynamic_cast<const object *>(this);

			if (object_self == nullptr)
				return result;

			result.first = object_self->get_first_ancestor_of_<ancestor_type>([&](tree &ancestor){
				if (auto surface_ancestor = dynamic_cast<surface *>(&ancestor); surface_ancestor != nullptr){
					auto &ancestor_position = surface_ancestor->get_current_position_();
					auto ancestor_client_offset = surface_ancestor->get_client_offset();
					auto ancestor_client_start_offset = surface_ancestor->get_client_start_offset();

					result.second.x += (ancestor_position.x + ancestor_client_offset.x + ancestor_client_start_offset.x);
					result.second.y += (ancestor_position.y + ancestor_client_offset.y + ancestor_client_start_offset.y);
				}

				return true;
			});

			if (auto surface_ancestor = dynamic_cast<surface *>(result.first); surface_ancestor != nullptr){
				auto ancestor_client_start_offset = surface_ancestor->get_client_start_offset_();
				result.second.x += ancestor_client_start_offset.x;
				result.second.y += ancestor_client_start_offset.y;
			}

			return result;
		}

		virtual POINT convert_position_from_absolute_value_(int x, int y) const;

		virtual POINT convert_position_to_absolute_value_(int x, int y) const;

		virtual RECT convert_dimension_from_absolute_value_(const RECT &value) const;

		virtual RECT convert_dimension_to_absolute_value_(const RECT &value) const;

		virtual UINT hit_test_(int x, int y) const;

		virtual UINT absolute_hit_test_(int x, int y) const;

		virtual grid_type *get_grid_() const;

		SIZE size_{};
		POINT position_{};

		mutable std::shared_ptr<grid_type> grid_;
	};
}
