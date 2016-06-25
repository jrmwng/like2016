#include "stdafx.h"
#include "CppUnitTest.h"

#include "..\shared\dct.h"

#include <algorithm>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

template __declspec(dllexport) void jrmwng::dct<1>(__m256 const (&)[1], __m256(&)[1]);
template __declspec(dllexport) void jrmwng::dct<2>(__m128 const (&)[2], __m128(&)[2]);
template __declspec(dllexport) void jrmwng::dct<2>(__m256d const (&)[2], __m256d(&)[2]);
template __declspec(dllexport) void jrmwng::dct<4>(__m128d const (&)[4], __m128d(&)[4]);

namespace UnitTest_dct
{		
	TEST_CLASS(UnitTest_dct)
	{
	public:
		
		TEST_METHOD(TestMethod_dct)
		{
			using namespace jrmwng;

			double alrInput[8];
			__m128d alr2Input[4];
			__m256d alr4Input[2];
			{
				std::fill(std::begin(alrInput), std::end(alrInput), 1.0);
				std::fill(std::begin(alr2Input), std::end(alr2Input), _mm_set1_pd(1.0));
				std::fill(std::begin(alr4Input), std::end(alr4Input), _mm256_set1_pd(1.0));
			}

			double alrOutput[8];
			__m128d alr2Output[4];
			__m256d alr4Output[2];
			{
				dct(alrInput, alrOutput);
				dct(alr2Input, alr2Output);
				dct(alr4Input, alr4Output);

				for (unsigned i = 0; i < 8; i++)
				{
					if (alrOutput[i] != alr2Output->m128d_f64[i])
					{
						Assert::AreEqual(alrOutput[i], alr2Output->m128d_f64[i]);
					}
					if (alrOutput[i] != alr4Output->m256d_f64[i])
					{
						Assert::AreEqual(alrOutput[i], alr4Output->m256d_f64[i]);
					}
				}
			}
		}

	};
}