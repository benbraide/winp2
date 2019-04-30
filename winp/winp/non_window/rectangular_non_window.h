#pragma once

#include "../ui/ui_non_window_surface.h"

namespace winp::non_window{
	class rectangle : public ui::non_window_surface{
	public:
		rectangle();

		explicit rectangle(thread::object &thread);

		explicit rectangle(tree &parent);

		rectangle(tree &parent, std::size_t index);

		virtual ~rectangle();

	protected:
		rectangle(thread::object &thread, bool init_grid);

		virtual HRGN create_handle_() const override;

		virtual utility::error_code update_handle_() override;
	};
}
