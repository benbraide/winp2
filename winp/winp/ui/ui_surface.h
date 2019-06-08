#pragma once

#include "../thread/thread_item.h"

#include "ui_object_collection.h"
#include "ui_tree.h"

namespace winp::grid{
	class object;
}

namespace winp::ui{
	class tree;

	class surface : public thread::synchronized_item{
	public:
		using grid_type = object_collection<grid::object>;
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

		virtual int get_width(const std::function<void(int)> &callback = nullptr) const;

		virtual int get_height(const std::function<void(int)> &callback = nullptr) const;

		virtual SIZE get_client_size(const std::function<void(const SIZE &)> &callback = nullptr) const;

		virtual int get_client_width(const std::function<void(int)> &callback = nullptr) const;

		virtual int get_client_height(const std::function<void(int)> &callback = nullptr) const;

		virtual POINT get_client_offset(const std::function<void(const POINT &)> &callback = nullptr) const;

		virtual int get_client_x_offset(const std::function<void(int)> &callback = nullptr) const;

		virtual int get_client_y_offset(const std::function<void(int)> &callback = nullptr) const;

		virtual POINT get_client_start_offset(const std::function<void(const POINT &)> &callback = nullptr) const;

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
		friend class thread::item_manager;

		friend class events::draw;
		friend class events::erase_background;
		friend class events::paint;

		virtual utility::error_code set_size_(int width, int height);

		virtual const SIZE &get_size_() const;

		virtual SIZE get_client_size_() const;

		virtual POINT get_client_offset_() const;

		virtual POINT get_client_start_offset_() const;

		virtual utility::error_code set_position_(int x, int y);

		virtual const POINT &get_position_() const;

		virtual utility::error_code set_absolute_position_(int x, int y);

		virtual POINT get_absolute_position_() const;

		virtual utility::error_code set_dimension_(int x, int y, int width, int height);

		virtual utility::error_code dimension_change_(int x, int y, int width, int height, UINT flags);

		virtual RECT get_dimension_() const;

		virtual RECT get_absolute_dimension_() const;

		template <typename ancestor_type>
		POINT convert_position_relative_to_ancestor_(int x, int y) const{
			auto object_self = dynamic_cast<const object *>(this);
			if (object_self == nullptr)
				return POINT{ x, y };

			auto ancestor = object_self->get_first_ancestor_of_<ancestor_type>([&](tree &ancestor){
				if (auto surface_ancestor = dynamic_cast<surface *>(&ancestor); surface_ancestor != nullptr){
					auto ancestor_position = POINT{ surface_ancestor->current_dimension_.left, surface_ancestor->current_dimension_.top };
					auto ancestor_client_offset = surface_ancestor->get_client_offset();
					auto ancestor_client_start_offset = surface_ancestor->get_client_start_offset();

					x += (ancestor_position.x + ancestor_client_offset.x + ancestor_client_start_offset.x);
					y += (ancestor_position.y + ancestor_client_offset.y + ancestor_client_start_offset.y);
				}

				return true;
			});

			if (auto surface_ancestor = dynamic_cast<surface *>(ancestor); surface_ancestor != nullptr){
				auto ancestor_client_start_offset = surface_ancestor->get_client_start_offset_();
				x += ancestor_client_start_offset.x;
				y += ancestor_client_start_offset.y;
			}

			return POINT{ x, y };
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

		RECT current_dimension_{};
		mutable std::shared_ptr<grid_type> grid_;

		unsigned __int64 size_animation_state_ = 0;
		unsigned __int64 position_animation_state_ = 0;
	};
}
