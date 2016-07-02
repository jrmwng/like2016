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
			float arInput[8];
			__m128 ar4Input[2];
			__m256 ar8Input[1];
			{
				std::fill(std::begin(alrInput), std::end(alrInput), 1.0);
				std::fill(std::begin(alr2Input), std::end(alr2Input), _mm_set1_pd(1.0));
				std::fill(std::begin(alr4Input), std::end(alr4Input), _mm256_set1_pd(1.0));
				std::fill(std::begin(arInput), std::end(arInput), 1.0F);
				std::fill(std::begin(ar4Input), std::end(ar4Input), _mm_set1_ps(1.0F));
				std::fill(std::begin(ar8Input), std::end(ar8Input), _mm256_set1_ps(1.0F));
			}

			double alrOutput[8];
			__m128d alr2Output[4];
			__m256d alr4Output[2];
			float arOutput[8];
			__m128 ar4Output[2];
			__m256 ar8Output[1];
			{
				dct(alrInput, alrOutput);
				dct(alr2Input, alr2Output);
				dct(alr4Input, alr4Output);
				dct(arInput, arOutput);
				dct(ar4Input, ar4Output);
				dct(ar8Input, ar8Output);

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
					if (arOutput[i] != ar4Output->m128_f32[i])
					{
						Assert::AreEqual(arOutput[i], ar4Output->m128_f32[i]);
					}
					if (arOutput[i] != ar8Output->m256_f32[i])
					{
						Assert::AreEqual(arOutput[i], ar8Output->m256_f32[i]);
					}
				}
			}
		}

	};
}