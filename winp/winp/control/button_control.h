#pragma once

#include "control_object.h"

namespace winp::control{
	class button : public object{
	public:
		using scale_type = D2D1_SIZE_F;

		virtual ~button();

		virtual utility::error_code set_text(const std::wstring &value, const std::function<void(button &, utility::error_code)> &callback = nullptr);

		virtual const std::wstring &get_text(const std::function<void(const std::wstring &)> &callback = nullptr) const;

		virtual utility::error_code set_font(HFONT value, const std::function<void(button &, utility::error_code)> &callback = nullptr);

		virtual HFONT get_font(const std::function<void(HFONT)> &callback = nullptr) const;

		virtual utility::error_code set_padding(const SIZE &value, const std::function<void(button &, utility::error_code)> &callback = nullptr);

		virtual utility::error_code set_padding(int horizontal, int vertical, const std::function<void(button &, utility::error_code)> &callback = nullptr);

		virtual const SIZE &get_padding(const std::function<void(const SIZE &)> &callback = nullptr) const;

		virtual utility::error_code set_scale(const scale_type &value, const std::function<void(button &, utility::error_code)> &callback = nullptr);

		virtual const scale_type &get_scale(const std::function<void(const scale_type &)> &callback = nullptr) const;

	protected:
		explicit button(thread::object &thread);

		virtual DWORD get_filtered_styles_(bool is_extended) const override;

		virtual const wchar_t *get_window_text_() const override;

		virtual utility::error_code set_text_(const std::wstring &value);

		virtual utility::error_code set_font_(HFONT value);

		virtual utility::error_code set_padding_(int horizontal, int vertical);

		virtual utility::error_code set_scale_(const scale_type &value);

		virtual utility::error_code update_size_();

		virtual SIZE compute_size_() const;

		virtual SIZE compute_additional_size_() const;

		std::wstring text_;
		HFONT font_ = nullptr;
		scale_type scale_{ 1.0f, 1.0f };
		SIZE padding_{ 20, 10 };
	};
}
