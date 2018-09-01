#pragma once
#include "Bind.h"
#include "AutoInjectWrapper.h"
#include "DIConfig.h"

namespace DI
{
	template<typename T, typename TInjector, std::size_t... RestI>
	constexpr bool IsConstructiableWithNumArg(std::index_sequence<RestI...>)
	{
		return std::is_constructible_v<T, AutoInjectWrapper<TInjector, T, RestI>...>;
	}

	template<typename T, typename TInjector, std::size_t I>
	constexpr std::size_t _getCtorParamNum(std::index_sequence<I>)
	{
		static_assert(IsConstructiableWithNumArg<T, TInjector>(std::make_index_sequence<I>()), "inject failed, please increase the value of MaxCtorParamNum");
		return I;
	}

	template<typename T, typename TInjector, std::size_t I, std::size_t... RestI>
	constexpr std::size_t _getCtorParamNum(std::index_sequence<I, RestI...>)
	{
		if constexpr (IsConstructiableWithNumArg<T, TInjector>(std::make_index_sequence<I>()))
		{
			return I;
		}
		else
		{
			return _getCtorParamNum<T, TInjector>(std::index_sequence<RestI...>());
		}
	}

	template<typename T, typename TInjector>
	constexpr std::size_t GetCtorParamNum()
	{
		return _getCtorParamNum<T, TInjector>(std::make_index_sequence<MaxCtorParamNum + 1>());
	}

	template<typename ...TBindings>
	class Injector
	{
		template<typename T, std::size_t I>
		T _create(std::index_sequence<I>)
		{
			using TBind = std::remove_reference_t<decltype(std::get<I>(_bindings))>;
			using bindImpType = typename GetBindingImpType<TBind>::Type;
			using isBindToImp = IsBindingToImp<TBind>;
			using sameBindType = IsSameBindingType<TBind, T>;

			static_assert(sameBindType(), "type not registered");

			if constexpr (sameBindType() && isBindToImp())
			{
				return static_cast<T>(new std::remove_pointer_t<bindImpType>());
			}
			else if constexpr (sameBindType())
			{
				if (!std::get<I>(_bindings).provider)
				{
					return *std::get<I>(_bindings).value;
				}
				else
				{
					return std::get<I>(_bindings).provider();
				}
			}
		}

		template<typename T, std::size_t I, std::size_t ...RestI>
		T _create(std::index_sequence<I, RestI...>)
		{
			using TBind = std::remove_reference_t<decltype(std::get<I>(_bindings))>;
			using sameBindType = IsSameBindingType<TBind, T>;
			using bindImpType = typename GetBindingImpType<TBind>::Type;
			using isBindToImp = IsBindingToImp<TBind>;

			if constexpr (sameBindType() && isBindToImp())
			{
				return static_cast<T>(new std::remove_pointer_t<bindImpType>());
			}
			else if constexpr (sameBindType())
			{
				if (!std::get<I>(_bindings).provider)
				{
					return *std::get<I>(_bindings).value;
				}
				else
				{
					return std::get<I>(_bindings).provider();
				}
			}
			else
			{
				return _create<T>(std::index_sequence<RestI...>());
			}
		}

		template<typename T, std::size_t... I>
		T _construct(std::index_sequence<I...>)
		{
			return T(AutoInjectWrapper<Injector, T, I>(*this)...);
		}

	public:

		std::tuple<TBindings...> _bindings;
		Injector(TBindings... bindings) : _bindings(std::make_tuple(bindings...))
		{

		}

		template<typename T>
		T Create()
		{
			auto seq = std::make_index_sequence<sizeof...(TBindings)>();
			return _create<T>(seq);
		}

		template<typename T>
		T Construct()
		{
			constexpr std::size_t num = GetCtorParamNum<T, Injector>();
			return _construct<T>(std::make_index_sequence<num>());
		}
	};
}
