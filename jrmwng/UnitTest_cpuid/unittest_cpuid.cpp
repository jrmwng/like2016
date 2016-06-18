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

			cpuid_forest_t cpuidForest0;
			std::stringstream ss;
			{
				ss << cpuidForest0;
				ss.seekp(0);
			}
			cpuid_forest_t cpuidForest1(ss);

			cpuid_standard_tree_t cpuidStandardTree = cpuidForest1;
			cpuid_extended_tree_t cpuidExtendedTree = cpuidForest1;
			cpuid_hypervisor_tree_t cpuidHypervisorTree = cpuidForest1;

			cpuid_t<0x00000000U> cpuid00000000 = cpuidStandardTree;
			cpuid_t<0x00000001U> cpuid00000001 = cpuidStandardTree;
			cpuid_t<0x00000004U, 0> cpuid00000004_0 = cpuidStandardTree;
			cpuid_t<0x00000004U, 1> cpuid00000004_1 = cpuidStandardTree;
			cpuid_t<0x00000004U, 2> cpuid00000004_2 = cpuidStandardTree;
			cpuid_t<0x00000004U, 3> cpuid00000004_3 = cpuidStandardTree;
			cpuid_t<0x00000007U> cpuid00000007 = cpuidStandardTree;
			cpuid_t<0x80000000U> cpuid80000000 = cpuidExtendedTree;
			cpuid_t<0x80000000U> cpuid80000001 = cpuidExtendedTree;
			cpuid_t<0x40000000U> cpuid40000000 = cpuidHypervisorTree;
		}

	};
}