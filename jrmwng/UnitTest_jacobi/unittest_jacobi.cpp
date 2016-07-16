#include "stdafx.h"
#include "CppUnitTest.h"

#include "../shared/jacobi.h"
#include <algorithm>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest_jacobi
{		
	TEST_CLASS(UnitTest_jacobi)
	{
	public:
		
		TEST_METHOD(TestMethod_jacobi)
		{
			float aarA[3][3] =
			{
				{ 1.0F, 0.0F, 0.0F },
				{ 0.0F, 1.0F, 0.0F },
				{ 0.0F, 0.0F, 1.0F },
			};
			float arB[3] = { 1.0F, 1.0F, 1.0F };

			float aarX[2][3] = { 0.0F };

			for (size_t i = 0; i < 10000; i++)
			{
				jrmwng::jacobi_iterate(aarA, arB, aarX[0], aarX[1]);

				if (fabsf(aarX[1][0] - aarX[0][0]) < 0.001F)
				{
					break;
				}
				std::copy(std::begin(aarX[1]), std::end(aarX[1]), aarX[0]);
			}

			Assert::AreEqual(1.0F, aarX[1][0]);
			// TODO: Your test code here
		}

	};
}