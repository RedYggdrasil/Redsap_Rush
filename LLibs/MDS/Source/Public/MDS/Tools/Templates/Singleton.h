#pragma once
#include <memory>

namespace mds
{
	template<class TClassType>
	class Singleton
	{
	public:
		Singleton(const Singleton&) = delete;
		Singleton operator=(const Singleton&) = delete;

		static TClassType& Get()
		{
			static TClassType instance;
			return instance;
		}

	protected:
		Singleton() {};
	};
}