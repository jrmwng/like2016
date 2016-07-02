#include "stdafx.h"
#include "CppUnitTest.h"

#include "../shared/taylor.h"
#include "../shared/mm.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest_taylor_mm
{		
	TEST_CLASS(UnitTest_taylor_mm)
	{
		template <size_t... uIndex, typename Tfunc>
		void for_each(std::index_sequence<uIndex...>, Tfunc && tFunc)
		{
			using type = int [];
			(void) type {
				(std::forward<Tfunc>(tFunc)(std::integral_constant<size_t, uIndex>()), 0)...
			};
		}
		template <typename... Targs, typename Tfunc>
		void for_each(std::tuple<Targs...> const & tArgs, Tfunc && tFunc)
		{
			for_each(std::make_index_sequence<sizeof...(Targs)>(), [&](auto const n)
			{
				std::forward<Tfunc>(tFunc)(std::get<n.value>(tArgs));
			});
		}
	public:
		
		TEST_METHOD(TestMethod_taylor_mm)
		{
			using namespace jrmwng;
			using namespace jrmwng::mm;

			for_each(std::make_tuple(0.0, 0.0F, 0), [&](auto const tAngle)
			{
				auto const tSin = taylor_sin<8>(tAngle);
				auto const tCos = taylor_cos<8>(tAngle);
			});
		}

	};
}