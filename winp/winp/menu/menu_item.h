#pragma once

#include "../ui/ui_tree.h"
#include "../ui/ui_surface.h"

namespace winp::menu{
	class tree;
	class object;
	class popup;
	class wrapped_popup;
	class system_popup;

	class item : public ui::object, public ui::surface{
	public:
		item();

		explicit item(thread::object &thread);

		explicit item(tree &parent);

		item(tree &parent, std::size_t index);

		virtual ~item();

		virtual utility::error_code set_absolute_index(std::size_t value, const std::function<void(item &, utility::error_code)> &callback = nullptr);

		virtual std::size_t get_absolute_index(const std::function<void(std::size_t)> &callback = nullptr) const;

		virtual std::size_t get_items_count_before(const std::function<void(std::size_t)> &callback = nullptr) const;

		virtual utility::error_code set_bitmap(HBITMAP value, const std::function<void(item &, utility::error_code)> &callback = nullptr);

		virtual HBITMAP get_bitmap(const std::function<void(HBITMAP)> &callback = nullptr) const;

		virtual utility::error_code set_states(UINT value, const std::function<void(item &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code add_states(UINT value, const std::function<void(item &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code remove_states(UINT value, const std::function<void(item &, utility::error_code)> &callback = nullptr);

		virtual UINT get_states(const std::function<void(UINT)> &callback = nullptr) const;

		virtual UINT get_persistent_states(const std::function<void(UINT)> &callback = nullptr) const;

		virtual UINT get_filtered_states(const std::function<void(UINT)> &callback = nullptr) const;

		virtual UINT get_types(const std::function<void(UINT)> &callback = nullptr) const;

		virtual utility::error_code set_enabled_state(bool is_enabled, const std::function<void(item &, utility::error_code)> &callback = nullptr);

		virtual bool is_enabled(const std::function<void(bool)> &callback = nullptr) const;

		virtual bool is_owner_drawn(const std::function<void(bool)> &callback = nullptr) const;

		virtual utility::error_code select(const std::function<void(item &, utility::error_code)> &callback = nullptr);

	protected:
		friend class tree;
		friend class popup;
		friend class wrapped_popup;
		friend class system_popup;

		virtual utility::error_code create_() override;

		virtual utility::error_code destroy_() override;

		virtual bool is_created_() const override;

		virtual utility::error_code set_parent_value_(ui::tree *value, bool changing) override;

		virtual utility::error_code set_index_value_(std::size_t value, bool changing) override;

		virtual HMENU create_handle_();

		virtual utility::error_code destroy_handle_();

		virtual utility::error_code fill_info_(MENUITEMINFOW &info) = 0;

		virtual utility::error_code set_absolute_index_(std::size_t value);

		virtual std::size_t get_absolute_index_() const;

		virtual std::size_t get_items_count_before_() const;

		virtual UINT get_insertion_index_() const;

		virtual utility::error_code set_bitmap_(HBITMAP value);

		virtual utility::error_code set_states_(UINT value);

		virtual utility::error_code update_states_();

		virtual UINT get_states_() const;

		virtual UINT get_persistent_states_() const;

		virtual UINT get_filtered_states_() const;

		virtual utility::error_code update_types_();

		virtual UINT get_types_() const;

		virtual utility::error_code set_enabled_state_(bool is_enabled);

		virtual bool is_owner_drawn_() const;

		virtual bool is_own_info_(const MENUITEMINFOW &info) const;

		HMENU handle_ = nullptr;
		HBITMAP bitmap_ = nullptr;

		UINT local_id_ = 0u;
		UINT states_ = 0u;
		UINT types_ = 0u;
	};
}
