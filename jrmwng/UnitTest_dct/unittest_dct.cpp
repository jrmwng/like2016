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

			double alrInput[8] = { 0 };
			{
				std::fill(std::begin(alrInput), std::end(alrInput), 1.0F);
			}

			double alrOutput[8] = { 0 };
			{
				dct(alrInput, alrOutput);
			}

			Assert::AreEqual<double>(8, alrOutput[0]);

			dct_matrix<float, 8> stMatrix8;

			Assert::AreEqual<float>(1.0F, stMatrix8[0][0]);
		}

	};
}