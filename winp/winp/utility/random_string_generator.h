#pragma once

#include <string>
#include <vector>
#include <climits>
#include <unordered_map>

#include "random_generator.h"

namespace winp::utility{
	struct random_string_generator_char_set{
		static constexpr unsigned int nil			= (0 << 0x0000);
		static constexpr unsigned int alpha			= (1 << 0x0000);
		static constexpr unsigned int uppercase		= (1 << 0x0001);
		static constexpr unsigned int lowercase		= (1 << 0x0002);
		static constexpr unsigned int digits		= (1 << 0x0003);
		static constexpr unsigned int oct			= (1 << 0x0004);
		static constexpr unsigned int hex			= (1 << 0x0005);
		static constexpr unsigned int invert		= (1 << 0x0006);
	};

	template <template <class> class distribution_type = std::uniform_int_distribution, class engine_type = std::mt19937, class seeder_type = std::random_device>
	class basic_random_string_generator{
	public:
		struct set_range{
			int from;
			int to;
		};

		using set_range_list_type = std::vector<set_range>;
		using char_set_type = unsigned int;

		template <typename... args_types>
		explicit basic_random_string_generator(args_types &&... args)
			: generator_(std::forward<args_types>(args)...){}

		template <typename string_type>
		string_type generate(std::size_t min_size, std::size_t max_size, const set_range_list_type &char_set){
			return generate<string_type>(min_size, max_size, char_set, std::unordered_map<std::size_t, typename string_type::traits_type::char_type>{});
		}

		template <typename string_type>
		string_type generate(std::size_t min_size, std::size_t max_size, const set_range_list_type &char_set,
			const std::unordered_map<std::size_t, typename string_type::traits_type::char_type> &delim){
			return generate<string_type>(generator_(min_size, max_size), char_set, delim);
		}

		template <typename string_type>
		string_type generate(std::size_t size, const set_range_list_type &char_set){
			return generate<string_type>(size, char_set, std::unordered_map<std::size_t, typename string_type::traits_type::char_type>{});
		}

		template <typename string_type>
		string_type generate(std::size_t size, const set_range_list_type &char_set,
			const std::unordered_map<std::size_t, typename string_type::traits_type::char_type> &delim){
			using char_type = typename string_type::traits_type::char_type;
			using const_iterator_type = typename std::unordered_map<std::size_t, typename string_type::traits_type::char_type>::const_iterator;

			if (size == 0u || char_set.empty())
				return string_type();

			string_type value;
			const_iterator_type iter;
			std::size_t char_set_index, max_char_set_index = (char_set.size() - 1u);

			value.reserve(size + delim.size());
			if (!delim.empty() && (iter = delim.find(static_cast<std::size_t>(0))) != delim.end())
				value.append(1, iter->second);//Append delimiter

			for (std::size_t count = 1; count <= size; ++count){
				char_set_index = ((max_char_set_index == 0u) ? 0u : generator_(static_cast<std::size_t>(0), max_char_set_index));
				value.append(1, static_cast<char_type>(generator_(char_set[char_set_index].from, char_set[char_set_index].to)));
				if (!delim.empty() && (iter = delim.find(count)) != delim.end())
					value.append(1, iter->second);//Append delimiter
			}

			return value;
		}

		template <typename string_type>
		string_type generate(std::size_t min_size, std::size_t max_size, char_set_type char_set){
			return generate<string_type>(min_size, max_size, char_set, std::unordered_map<std::size_t, typename string_type::traits_type::char_type>{});
		}

		template <typename string_type>
		string_type generate(std::size_t min_size, std::size_t max_size, char_set_type char_set,
			const std::unordered_map<std::size_t, typename string_type::traits_type::char_type> &delim){
			set_range_list_type list;
			resolve_char_set<typename string_type::traits_type::char_type>(char_set, list);
			return generate<string_type>(min_size, max_size, list, delim);
		}

		template <typename string_type>
		string_type generate(std::size_t size, char_set_type char_set){
			return generate<string_type>(size, char_set, std::unordered_map<std::size_t, typename string_type::traits_type::char_type>{});
		}

		template <typename string_type>
		string_type generate(std::size_t size, char_set_type char_set,
			const std::unordered_map<std::size_t, typename string_type::traits_type::char_type> &delim){
			return generate<string_type>(size, size, char_set, delim);
		}

		template <typename char_type>
		static void resolve_char_set(char_set_type char_set, set_range_list_type &out){
			if (char_set == random_string_generator_char_set::nil){//Include all
				out.push_back(set_range{ 0, static_cast<int>(std::numeric_limits<char_type>::max()) });
				return;
			}

			if ((char_set & random_string_generator_char_set::invert) == random_string_generator_char_set::invert)
				char_set = ~char_set;//Exclude specified

			if ((char_set & random_string_generator_char_set::alpha) == random_string_generator_char_set::alpha){//Alphabet range
				if ((char_set & (random_string_generator_char_set::uppercase | random_string_generator_char_set::lowercase)) != 0u){
					if ((char_set & random_string_generator_char_set::uppercase) == random_string_generator_char_set::uppercase)//Uppercase range
						out.push_back(set_range{ 0x0041, 0x005A });

					if ((char_set & random_string_generator_char_set::lowercase) == random_string_generator_char_set::lowercase)//Lowercase range
						out.push_back(set_range{ 0x0061, 0x007A });
				}
				else{//Full alphabet range
					out.push_back(set_range{ 0x0041, 0x005A });
					out.push_back(set_range{ 0x0061, 0x007A });
				}
			}

			if ((char_set & random_string_generator_char_set::digits) == random_string_generator_char_set::digits){//Digits range
				if ((char_set & random_string_generator_char_set::hex) == random_string_generator_char_set::hex){//Hexadecimal range
					out.push_back(set_range{ 0x0030, 0x0039 });
					if ((char_set & (random_string_generator_char_set::uppercase | random_string_generator_char_set::lowercase)) != 0u){
						if ((char_set & random_string_generator_char_set::uppercase) == random_string_generator_char_set::uppercase)//Uppercase range
							out.push_back(set_range{ 0x0041, 0x0046 });

						if ((char_set & random_string_generator_char_set::lowercase) == random_string_generator_char_set::lowercase)//Lowercase range
							out.push_back(set_range{ 0x0061, 0x0066 });
					}
					else{//Full hex range
						out.push_back(set_range{ 0x0041, 0x0046 });
						out.push_back(set_range{ 0x0061, 0x0066 });
					}
				}
				else if ((char_set & random_string_generator_char_set::oct) == random_string_generator_char_set::oct)//Octal range
					out.push_back(set_range{ 0x0030, 0x0039 });
				else//Full digit range
					out.push_back(set_range{ 0x0030, 0x0037 });
			}
		}

	private:
		random_generator<distribution_type, engine_type, seeder_type> generator_;
	};

	using random_string_generator = basic_random_string_generator<>;

	template <class string_type, template <class> class distribution_type = std::uniform_int_distribution, class engine_type = std::mt19937, class seeder_type = std::random_device>
	class typed_random_string_generator : public basic_random_string_generator<distribution_type, engine_type, seeder_type>{
	public:
		using base_type = basic_random_string_generator<distribution_type, engine_type, seeder_type>;
		using set_range_list_type = typename base_type::set_range_list_type;
		using char_set_type = typename base_type::char_set_type;

		template <typename... args_types>
		explicit typed_random_string_generator(args_types &&... args)
			: base_type(std::forward<args_types>(args)...){}

		string_type operator ()(std::size_t min_size, std::size_t max_size, const set_range_list_type &char_set){
			return base_type::template generate<string_type>(min_size, max_size, char_set);
		}

		string_type operator ()(std::size_t min_size, std::size_t max_size, const set_range_list_type &char_set,
			const std::unordered_map<std::size_t, typename string_type::traits_type::char_type> &delim){
			return base_type::template generate<string_type>(min_size, max_size, char_set, delim);
		}

		string_type operator ()(std::size_t size, const set_range_list_type &char_set){
			return base_type::template generate<string_type>(size, char_set);
		}

		string_type operator ()(std::size_t size, const set_range_list_type &char_set,
			const std::unordered_map<std::size_t, typename string_type::traits_type::char_type> &delim){
			return base_type::template generate<string_type>(size, char_set, delim);
		}

		string_type operator ()(std::size_t min_size, std::size_t max_size, char_set_type char_set){
			return base_type::template generate<string_type>(min_size, max_size, char_set);
		}

		string_type operator ()(std::size_t min_size, std::size_t max_size, char_set_type char_set,
			const std::unordered_map<std::size_t, typename string_type::traits_type::char_type> &delim){
			return base_type::template generate<string_type>(min_size, max_size, char_set, delim);
		}

		string_type operator ()(std::size_t size, char_set_type char_set){
			return base_type::template generate<string_type>(size, char_set);
		}

		string_type operator ()(std::size_t size, char_set_type char_set,
			const std::unordered_map<std::size_t, typename string_type::traits_type::char_type> &delim){
				return base_type::template generate<string_type>(size, char_set, delim);
			}
	};

	using random_narrow_string_generator = typed_random_string_generator<std::string>;
	using random_wide_string_generator = typed_random_string_generator<std::wstring>;
}
