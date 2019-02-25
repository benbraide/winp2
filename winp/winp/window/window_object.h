#pragma once

#include "../ui/ui_window_surface.h"

namespace winp::window{
	class object : public ui::window_surface{
	public:
		object();

		explicit object(thread::object &thread);

		explicit object(tree &parent);

		object(tree &parent, std::size_t index);

		virtual ~object();

		virtual utility::error_code set_caption(const std::wstring &value, const std::function<void(object &, utility::error_code)> &callback = nullptr);

		virtual const std::wstring &get_caption(const std::function<void(const std::wstring &)> &callback = nullptr) const;

	protected:
		virtual const wchar_t *get_window_text_() const override;

		virtual utility::error_code set_caption_(const std::wstring &value);

		virtual const std::wstring &get_caption_() const;

		std::wstring caption_;
	};
}
