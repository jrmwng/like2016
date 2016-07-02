#include "../shared/cpuid.h"

/* Author: jrmwng @ 2016 */

// cpuid.exe

int main(int nArgc, wchar_t const *apArg [])
{
	using namespace jrmwng::cpuid;

	// forest (trees) of CPUID instruction
	cpuid_forest_t cpuidForest;

	std::cout << cpuidForest << std::endl;
	return 0;
}