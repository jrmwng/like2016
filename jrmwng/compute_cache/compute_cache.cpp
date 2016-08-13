#include "compute_cache.h"

#include <iostream>

int main()
{
	auto const fnCompute = [](auto tArg)
	{
		return tArg;
	};

	auto spCompute = std::make_shared<decltype(fnCompute)>(fnCompute);

	std::cout << jrmwng::compute_cache(std::forward<decltype(spCompute)>(spCompute), "Hello World!") << std::endl;

	std::cout << jrmwng::compute_cache(&printf, "%s\n", "Hello World!", spCompute) << std::endl;
	spCompute.reset();
	std::cout << jrmwng::compute_cache(&printf, "%s\n", "Hello World!", spCompute) << std::endl;
	return 0;
}