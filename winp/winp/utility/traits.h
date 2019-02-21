#pragma once

#include <functional>

namespace winp::utility{
	template<typename return_type, typename... args_types>
	struct function_traits_base{
		using m_return_type = return_type;
		using function_type = std::function<m_return_type(args_types...)>;
		using raw_function_type = m_return_type(*)(args_types...);

		static const std::size_t arg_count = sizeof...(args_types);

		template <std::size_t index>
		struct args{
			using type = typename std::tuple_element<index, std::tuple<args_types...>>::type;
		};
	};

	template<typename function_type>
	struct function_traits;

	template<typename return_type, typename... args_types>
	struct function_traits<return_type(args_types...)> : function_traits_base<return_type, args_types...>{};

	template<typename return_type, typename... args_types>
	struct function_traits<return_type(*)(args_types...)> : function_traits_base<return_type, args_types...>{};

	template<typename class_type, typename return_type, typename... args_types>
	struct function_traits<return_type(class_type::*)(args_types...)> : function_traits_base<return_type, args_types...>{};

	template<typename class_type, typename return_type, typename... args_types>
	struct function_traits<return_type(class_type::*)(args_types...) const> : function_traits_base<return_type, args_types...>{};

	template<typename return_type, typename... args_types>
	struct function_traits<std::function<return_type(args_types...)>> : function_traits_base<return_type, args_types...>{};

	template<typename function_object_type>
	struct function_or_function_object_traits : function_traits<decltype(&function_object_type::operator ())>{};

	template<typename return_type, typename... args_types>
	struct function_or_function_object_traits<return_type(args_types...)> : function_traits<return_type(args_types...)>{};

	template<typename return_type, typename... args_types>
	struct function_or_function_object_traits<return_type(*)(args_types...)> : function_traits<return_type(*)(args_types...)>{};

	template<typename class_type, typename return_type, typename... args_types>
	struct function_or_function_object_traits<return_type(class_type::*)(args_types...)> : function_traits<return_type(class_type::*)(args_types...)>{};

	template<typename class_type, typename return_type, typename... args_types>
	struct function_or_function_object_traits<return_type(class_type::*)(args_types...) const> : function_traits<return_type(class_type::*)(args_types...) const>{};

	template<typename return_type, typename... args_types>
	struct function_or_function_object_traits<std::function<return_type(args_types...)>> : function_traits<std::function<return_type(args_types...)>>{};

	template<typename function_object_type>
	struct object_to_function_traits{
		using traits = function_traits<decltype(&function_object_type::operator ())>;

		static typename traits::function_type get(const function_object_type &function){
			return function;
		}
	};

	template<typename return_type, typename... args_types>
	struct object_to_function_traits<return_type(args_types...)>{
		using traits = function_traits<return_type(*)(args_types...)>;

		template <typename function_type>
		static typename traits::function_type get(function_type function){
			return function;
		}
	};

	template<typename return_type, typename... args_types>
	struct object_to_function_traits<return_type(*)(args_types...)>{
		using traits = function_traits<return_type(*)(args_types...)>;

		static typename traits::function_type get(return_type(*function)(args_types...)){
			return function;
		}
	};
}
