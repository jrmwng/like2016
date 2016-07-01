#include "stdafx.h"
#include "CppUnitTest.h"

#include "../shared/taylor.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest_taylor
{		
	TEST_CLASS(UnitTest_taylor)
	{
	public:
		
		TEST_METHOD(TestMethod_taylor)
		{
			using namespace jrmwng;

			double lrSin90 = taylor_sin<8>(acos(0));

			Assert::AreEqual(1.0, lrSin90);
		}

	};
}