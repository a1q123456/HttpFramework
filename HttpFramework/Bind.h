#pragma once
#include <functional>
#include <type_traits>

namespace DI
{
	template<typename T, typename TImp = void>
	struct Bind
	{
		T* value;
		std::function<T()> provider;

		// Bind to provider
		Bind(std::function<T()> provider) : provider(provider), value(nullptr) {}

		// Bind interface to implementation
		Bind() : value(nullptr)
		{
			static_assert(!std::is_void_v<TImp>, "must bind to an implementation");
		}

		// Bind to value
		Bind(T&& value) : value(new T(value)) 
		{
			static_assert(!std::is_pointer_v<T>, "cannot bind value for pointer");
		};

		~Bind()
		{
			if (value != nullptr)
			{
				delete value;
			}
		}

		Bind(const Bind& other) : value(nullptr), provider(other.provider)
		{
			if (other.value != nullptr)
			{
				value = new T(*other.value);
			}
		}

		Bind(Bind&& other) : value(nullptr), provider(std::move(other.provider))
		{
			if (other.value != nullptr)
			{
				value = other.value;
				other.value = nullptr;
			}
		}

		template<typename TBind, typename TTarget>
		struct IsSameBindingType : std::false_type {};

		template<typename TBind, typename TImp>
		struct IsSameBindingType<Bind<TBind, TImp>, TBind> : std::true_type {};


		template<typename TBind>
		struct IsBindingToImp : std::true_type {};

		template<typename TBind>
		struct IsBindingToImp<Bind<TBind, void>> : std::false_type {};


		template<typename TBind, typename TImp = void>
		struct GetBindingImpType
		{
			using Type = void;
		};

		template<typename TBind, typename TImp>
		struct GetBindingImpType<Bind<TBind, TImp>, void>
		{
			using Type = TImp;
		};
	};
}
