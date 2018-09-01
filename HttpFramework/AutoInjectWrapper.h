#pragma once

#include <utility>
#include <type_traits>
#include <string>
#include <functional>


namespace DI
{
	

	template<typename TInjector, typename Ctor, std::size_t = 0>
	struct AutoInjectWrapper
	{
		TInjector& injector;

		AutoInjectWrapper(TInjector& injector) : injector(injector)
		{

		}

		template<typename T, typename std::enable_if<!std::is_same_v<Ctor, T>, int>::type = 0>
		operator T()
		{
			return injector.Create<T>();
		}
	};




	

	template<typename ...Bindings>
	auto MakeInjector(Bindings... bindings)
	{
		return Injector<Bindings...>(bindings...);
	}

}
