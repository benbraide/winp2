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
		virtual bool set_result_(const std::type_info &type, const void *result) override{
			if (type == typeid(std::wstring)){
				ref_ = *static_cast<const std::wstring *>(result);
				return true;
			}
			
			if (type == typeid(const wchar_t *) || type == typeid(wchar_t *)){
				ref_ = static_cast<const wchar_t *>(result);
				return true;
			}

			return base_type::set_result_(type, result);
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
}
