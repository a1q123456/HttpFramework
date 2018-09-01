#pragma once
#include <string>
#include <type_traits>
#include <vector>
#include <cctype>

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

namespace Text
{
	namespace Detail
	{
		template <typename CharT, typename StrT>
		StrT TrimStartImpl(StrT&& s, CharT ch)
		{
			auto first_not = s.find_first_not_of(ch);
			if (first_not != s.npos)
			{
				s = s.substr(first_not);
			}
			return s;
		}

		template <typename CharT, typename StrT>
		StrT TrimEndImpl(StrT&& s, CharT ch)
		{
			auto last = s.find_last_of(ch);
			if (last != s.npos)
			{
				s = s.substr(0, last);
			}
			return s;
		}

		template <typename StrT>
		auto GetCStr(StrT&& s) -> decltype(s.c_str())
		{
			return s.c_str();
		}

		template <typename StrT>
		StrT* GetCStr(StrT* s)
		{
			return s;
		}
	}

	template< typename C, typename TArg, typename = void >
	struct has_push_back
		: std::false_type
	{};

	template< typename C, typename TArg >
	struct has_push_back< C, TArg, typename std::enable_if<
		std::is_same<
		decltype(std::declval<C>().push_back(std::declval<const TArg&>())),
		void
		>::value
	>::type >
		: std::true_type
	{};


	template< typename C, typename TArg, typename TRet, typename = void >
	struct has_subscription
		: std::false_type
	{};

	template< typename C, typename TArg, typename TRet>
	struct has_subscription< C, TArg, TRet, typename std::enable_if<
		std::is_same<
		decltype(std::declval<C>()[std::declval<const TArg&>()]),
		TRet
		>::value
	>::type >
		: std::true_type
	{};

	template< typename C, typename = void >
	struct has_size
		: std::false_type
	{};

	template< typename C>
	struct has_size<C, typename std::enable_if<
		std::is_same<
		decltype(std::declval<C>().size()),
		std::size_t
		>::value
	>::type >
		: std::true_type
	{};

	struct StringUtils
	{
		StringUtils() = delete;
		StringUtils(const StringUtils&) = delete;
		StringUtils(StringUtils&&) = delete;

		template<class T, class TChar, class TFind>
		static T Split(const std::basic_string<TChar>& str, TFind&& delimiter, std::size_t& count)
		{
			return Split<T, TChar>(std::move(std::basic_string<TChar>(str)), delimiter, count);
		}

		template<class T, class TChar, class TFind>
		static T Split(std::basic_string<TChar>&& str, TFind&& delimiter, std::size_t& count)
		{
			static_assert(std::is_same_v<std::remove_reference_t<TFind>, TChar> || std::is_same_v<std::remove_reference_t<TFind>, TChar*>, "delimiter must be TChar or TChar*")
			count = 0;
			using String = std::basic_string<TChar>;
			using StringView = std::basic_string_view<TChar>;
			constexpr bool support_subscription = has_subscription<T, std::size_t, String&>::value;

			constexpr bool support_push_back = has_push_back<T, String>::value;
			constexpr bool support_size = has_size<T>::value;
			constexpr bool T_support_subscription_or_push_back = support_subscription || support_push_back;

			static_assert(T_support_subscription_or_push_back, "T must have a push_back method or support subscription");
			static_assert(std::is_same_v<T::value_type, String>, "T must be a container of std::basic_string<TChar>");
			static_assert(std::is_default_constructible_v<T>, "T needs to be default constructiable");

			// for safety
			if constexpr (!support_push_back && support_subscription)
			{
				static_assert(support_size, "T must have a size method returns std::size_t");
			}

			T container;
			std::size_t idx = 0;
			StringView strView = StringView(str);
			while (true)
			{
				auto pos = strView.find_first_of(delimiter);
				if (pos == strView.npos)
				{
					break;
				}
				auto found = strView.substr(0, pos);
				strView = strView.substr(pos + 1);
				if constexpr (support_push_back)
				{
					container.push_back(std::move(String(found)));
				}
				else
				{
					container[idx] = std::move(String(found));
				}
				idx++;
				count++;
			}
			if constexpr (support_push_back)
			{
				container.push_back(std::move(String(strView)));
			}
			else
			{
				if (container.size() <= idx)
				{
					throw std::out_of_range();
				}
				container[idx] = std::move(String(strView));
			}
			count++;
			return container;
		}

		template <typename CharT>
		static bool IsInteger(const std::basic_string<CharT>& s)
		{
			static_assert(std::is_integral_v<CharT>, "CharT must be a character type");
			return !s.empty() && std::find_if(s.begin(),
				s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
		}

		

		template <typename CharT>
		static std::basic_string_view<CharT>& TrimStart(std::basic_string_view<CharT>& s, CharT ch = ' ')
		{
			return Detail::TrimStartImpl(s, ch);
		}

		template <typename CharT>
		static std::basic_string_view<CharT> TrimStart(const std::basic_string_view<CharT>& s, CharT ch = ' ')
		{
			return Detail::TrimStartImpl(std::basic_string_view<CharT>(s), ch);
		}

		template <typename CharT>
		static std::basic_string_view<CharT>&& TrimStart(std::basic_string_view<CharT>&& s, CharT ch = ' ')
		{
			return Detail::TrimStartImpl(s, ch);
		}

		template <typename CharT>
		static std::basic_string<CharT>&& TrimStart(std::basic_string<CharT>&& s, CharT ch = ' ')
		{
			return Detail::TrimStartImpl(s, ch);
		}

		template <typename CharT>
		static std::basic_string<CharT>& TrimStart(std::basic_string<CharT>& s, CharT ch = ' ')
		{
			return Detail::TrimStartImpl(s, ch);
		}

		template <typename CharT>
		static std::basic_string<CharT> TrimStart(const std::basic_string<CharT>& s, CharT ch = ' ')
		{
			return Detail::TrimStartImpl(std::basic_string_view<CharT>(s.c_str()), ch);
		}



		template <typename CharT>
		static std::basic_string_view<CharT>& TrimEnd(std::basic_string_view<CharT>& s, CharT ch = ' ')
		{
			return Detail::TrimEndImpl(s, ch);
		}

		template <typename CharT>
		static std::basic_string_view<CharT> TrimEnd(const std::basic_string_view<CharT>& s, CharT ch = ' ')
		{
			return Detail::TrimEndImpl(std::basic_string_view<CharT>(s), ch);
		}

		template <typename CharT>
		static std::basic_string_view<CharT>&& TrimEnd(std::basic_string_view<CharT>&& s, CharT ch = ' ')
		{
			return Detail::TrimEndImpl(s, ch);
		}

		template <typename CharT>
		static std::basic_string<CharT>&& TrimEnd(std::basic_string<CharT>&& s, CharT ch = ' ')
		{
			return Detail::TrimEndImpl(s, ch);
		}

		template <typename CharT>
		static std::basic_string<CharT>& TrimEnd(std::basic_string<CharT>& s, CharT ch = ' ')
		{
			return Detail::TrimEndImpl(s, ch);
		}

		template <typename CharT>
		static std::basic_string<CharT> TrimEnd(const std::basic_string<CharT>& s, CharT ch = ' ')
		{
			return Detail::TrimEndImpl(std::basic_string_view<CharT>(s.c_str()), ch);
		}

		template <typename CharT, typename StrT>
		static StrT Trim(StrT&& s, CharT ch)
		{
			return Detail::TrimStartImpl(Detail::TrimEndImpl(std::basic_string_view<CharT>(Detail::GetCStr(s)), ch), ch);
		}

		template <typename CharT>
		static std::basic_string<CharT> Trim(CharT* s, CharT ch)
		{
			return Detail::TrimStartImpl(Detail::TrimEndImpl(std::basic_string_view<CharT>(Detail::GetCStr(s)), ch), ch);
		}

		template <typename CharT>
		static void ToLower(std::basic_string<CharT>& str)
		{
			static_assert(std::is_integral_v<CharT>, "CharT must be a character type");
			std::transform(str.begin(), str.end(), [](CharT ch) {
				return std::tolower(ch);
			});
		}

		template <typename CharT>
		static std::basic_string<CharT> ToLower(const std::basic_string<CharT>& str)
		{
			std::basic_string<CharT> s(str);
			ToLower(s);
			return s;
		}

		template <typename CharT>
		static std::basic_string<CharT>&& ToLower(std::basic_string<CharT>&& str)
		{
			ToLower(str);
			return str;
		}
	};
}

