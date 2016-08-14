#include "compute_cache.h"

#include <iostream>

int main()
{
	// WriteLine: "Hello World!\n13"
	std::cout << jrmwng::compute_cache(&printf, "%s\n", "Hello World!") << std::endl;
	// WriteLine: "13"
	std::cout << jrmwng::compute_cache(&printf, "%s\n", "Hello World!") << std::endl;

	// explicit cache eviction
	jrmwng::compute_cache_flush();

	// WriteLine: "Hello World!\n13"
	std::cout << jrmwng::compute_cache(&printf, "%s\n", "Hello World!") << std::endl;

	//

	auto fnRDTSC = [](int)
	{
		return __rdtsc();
	};

	std::cout << jrmwng::compute_cache(fnRDTSC, 0) << std::endl;
	std::cout << jrmwng::compute_cache(fnRDTSC, 1) << std::endl;
	std::cout << jrmwng::compute_cache(fnRDTSC, 0) << std::endl;
	return 0;
}
