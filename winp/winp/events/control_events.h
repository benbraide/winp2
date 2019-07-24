#pragma once

#include "event_object.h"

namespace winp::events{
	template <class base_type>
	class get_string_value : public base_type{
	public:
		template <typename... args_types>
		explicit get_string_value(std::wstring &ref, args_types &&... args)
			: base_type(std::forward<args_types>(args)...), ref_(ref){}

	protected:
		virtual bool set_result_untyped_(const std::any &result) override{
			if (auto value = std::any_cast<std::wstring>(&result); value != nullptr){
				ref_ = *value;
				return true;
			}

			if (auto value = std::any_cast<const wchar_t *>(&result); value != nullptr){
				ref_ = *value;
				return true;
			}

			return base_type::set_result_untyped_(result);
		}

		std::wstring &ref_;
	};

	class get_title : public get_string_value<object>{
	public:
		using m_base_type = get_string_value<object>;

		template <typename... args_types>
		explicit get_title(args_types &&... args)
			: m_base_type(std::forward<args_types>(args)...){}
	};

	class get_text : public get_string_value<object>{
	public:
		using m_base_type = get_string_value<object>;

		template <typename... args_types>
		explicit get_text(args_types &&... args)
			: m_base_type(std::forward<args_types>(args)...){}
	};

	class get_font : public object{
	public:
		template <typename... args_types>
		explicit get_font(args_types &&... args)
			: object(std::forward<args_types>(args)...){}
	};

	class get_image : public object{
	public:
		template <typename... args_types>
		explicit get_image(args_types &&... args)
			: object(std::forward<args_types>(args)...){}
	};

	class link_clicked : public object{
	public:
		template <typename... args_types>
		explicit link_clicked(args_types &&... args)
			: object(std::forward<args_types>(args)...){}
	};

	class activate_page : public object{
	public:
		template <typename... args_types>
		explicit activate_page(args_types &&... args)
			: object(std::forward<args_types>(args)...){}
	};

	class deactivate_page : public object{
	public:
		template <typename... args_types>
		explicit deactivate_page(args_types &&... args)
			: object(std::forward<args_types>(args)...){}
	};
}
