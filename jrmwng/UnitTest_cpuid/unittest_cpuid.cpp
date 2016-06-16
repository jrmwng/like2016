#include "stdafx.h"
#include "CppUnitTest.h"

#include "..\shared\cpuid.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest_cpuid
{		
	TEST_CLASS(UnitTest_cpuid)
	{
	public:
		
		TEST_METHOD(TestMethod_cpuid)
		{
			using namespace jrmwng::cpuid;

			cpuid_forest_t cpuidForest;

			// TODO: Your test code here
		}

	};
}