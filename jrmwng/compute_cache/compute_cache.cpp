#include "compute_cache.h"

#include <iostream>

int main()
{
	std::shared_ptr<void> spVoid(new int);

	std::cout << jrmwng::compute_cache(&printf, "%s\n", "Hello World!", spVoid) << std::endl;
	std::cout << jrmwng::compute_cache(&printf, "%s\n", "Hello World!", spVoid) << std::endl;
	return 0;
}