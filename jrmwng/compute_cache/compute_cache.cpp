#include "compute_cache.h"

#include <iostream>

int main()
{
	std::shared_ptr<void> spEvictCache(new int);

	// WriteLine: "Hello World!\n13"
	std::cout << jrmwng::compute_cache(&printf, "%s\n", "Hello World!", spEvictCache) << std::endl;
	// WriteLine: "13"
	std::cout << jrmwng::compute_cache(&printf, "%s\n", "Hello World!", spEvictCache) << std::endl;

	// explicit cache eviction
	spEvictCache.reset();

	// WriteLine: "Hello World!\n13"
	std::cout << jrmwng::compute_cache(&printf, "%s\n", "Hello World!", spEvictCache) << std::endl;

	//

	auto fnRDTSC = []()
	{
		return __rdtsc();
	};

	auto spRDTSC0 = std::make_shared<decltype(fnRDTSC)>(fnRDTSC);
	auto spRDTSC1 = std::make_shared<decltype(fnRDTSC)>(fnRDTSC);

	// WriteLine: 1234
	std::cout << jrmwng::compute_cache(spRDTSC0) << std::endl;
	// WriteLine: 5678
	std::cout << jrmwng::compute_cache(spRDTSC1) << std::endl;
	// WriteLine: 1234
	std::cout << jrmwng::compute_cache(spRDTSC0) << std::endl;
	return 0;
}
