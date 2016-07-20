#include <Windows.h>
#include <stdio.h>

int main()
{
	LARGE_INTEGER stFreqency;
	{
		QueryPerformanceFrequency(&stFreqency);
	}
	LARGE_INTEGER stCounterSmall;
	{
		QueryPerformanceCounter(&stCounterSmall);
	}
	LARGE_INTEGER stCounterLarge;
	{
		stCounterLarge.QuadPart = stCounterSmall.QuadPart + (1LL << 60);// stFreqency.QuadPart * 3600 * 24 * 30 * 1000;
	}

	for (;; stCounterSmall.QuadPart += stFreqency.QuadPart, stCounterLarge.QuadPart += stFreqency.QuadPart)
	{
		printf(
			"%016I64X %016I64X %08lX %08lX %016I64X %08lX %08lX\n",
			stFreqency.QuadPart,
			stCounterSmall.QuadPart,
			static_cast<unsigned long>(stCounterSmall.QuadPart * 1000.0 / stFreqency.QuadPart),
			static_cast<unsigned long>(stCounterSmall.QuadPart * 1000.0 * 1000.0 / stFreqency.QuadPart),
			stCounterLarge.QuadPart,
			static_cast<unsigned long>(stCounterLarge.QuadPart * 1000.0 / stFreqency.QuadPart),
			static_cast<unsigned long>(stCounterLarge.QuadPart * 1000.0 * 1000.0 / stFreqency.QuadPart)
		);
	}
	return 0;
}