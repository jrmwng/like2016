#include "compute_cache.h"

#include <iostream>

int main()
{
	auto const fnCompute = [](auto tArg)
	{
		return tArg;
	};

	auto spCompute = std::make_shared<decltype(fnCompute)>(fnCompute);

	std::cout << jrmwng::compute_cache(spCompute, "Hello World!") << std::endl;
	return 0;
}