#include "memory.h"

#include <functional>

int main()
{
	jrmwng::memory_manager *pMM = new jrmwng::memory_manager;
	delete pMM;
	return 0;
}