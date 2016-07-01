#include "stdafx.h"
#include "CppUnitTest.h"

#include "../shared/mm.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest_mm
{		
	TEST_CLASS(UnitTest_mm)
	{
	public:
		
		TEST_METHOD(TestMethod_mm)
		{
			using namespace jrmwng::mm;

			Mvar<__m256d, double> const lr4A(1.0);
			Mvar<__m256d, double> const lr4B(_mm256_set1_pd(1.0));

			auto const lr4AaddB = lr4A + lr4B;
			auto const lr4AsubB = lr4A - lr4B;
			auto const lr4AmulB = lr4A * lr4B;
			auto const lr4AdivB = lr4A / lr4B;

			Mvar<__m256d, double> lr4Product(1.0);
			{
				lr4Product *= lr4A;
				lr4Product *= lr4A + lr4B;
				lr4Product *= lr4A - lr4B;
				lr4Product *= lr4A * lr4B;
				lr4Product *= lr4A / lr4B;
				lr4Product /= lr4A;
				lr4Product /= lr4A + lr4B;
				lr4Product /= lr4A - lr4B;
				lr4Product /= lr4A * lr4B;
				lr4Product /= lr4A / lr4B;
			}

			Mvar<__m256d, double> lr4Sum(0.0);
			{
				lr4Sum += lr4A;
				lr4Sum += lr4A + lr4B;
				lr4Sum += lr4A - lr4B;
				lr4Sum += lr4A * lr4B;
				lr4Sum += (-lr4A) * lr4B;
				lr4Sum += lr4A * (-lr4B);
				lr4Sum += (-lr4A) * (-lr4B);
				lr4Sum += -(lr4A * lr4B);
				lr4Sum += lr4A / lr4B;
				lr4Sum += (-lr4A) / lr4B;
				lr4Sum += lr4A / (-lr4B);
				lr4Sum += (-lr4A) / (-lr4B);
				lr4Sum += -(lr4A / lr4B);
				lr4Sum -= lr4A;
				lr4Sum -= lr4A + lr4B;
				lr4Sum -= lr4A - lr4B;
				lr4Sum -= lr4A * lr4B;
				lr4Sum -= (-lr4A) * lr4B;
				lr4Sum -= lr4A * (-lr4B);
				lr4Sum -= (-lr4A) * (-lr4B);
				lr4Sum -= -(lr4A * lr4B);
				lr4Sum -= lr4A / lr4B;
				lr4Sum -= (-lr4A) / lr4B;
				lr4Sum -= lr4A / (-lr4B);
				lr4Sum -= (-lr4A) / (-lr4B);
				lr4Sum -= -(lr4A / lr4B);
			}
		}

	};
}