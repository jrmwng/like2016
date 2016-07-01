#include "stdafx.h"
#include "CppUnitTest.h"

#include "../shared/mm.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest_mm
{		
	TEST_CLASS(UnitTest_mm)
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
		
		TEST_METHOD(TestMethod_mm)
		{
			using namespace jrmwng::mm;

			for_each(
				std::make_tuple(
					Mvar<__m256d, double>(1),
					Mvar<__m256, float>(1),
					Mvar<__m128d, double>(1),
					Mvar<__m128, float>(1)),
				[&](auto const & mm)
			{
				auto const mmA = mm;
				auto const mmB = mm;

				auto const mmAaddB = mmA + mmB;
				auto const mmAsubB = mmA - mmB;
				auto const mmAmulB = mmA * mmB;
				auto const mmAdivB = mmA / mmB;

				std::decay_t<decltype(mm)> mmProduct(1.0);
				{
					mmProduct *= mmA;
					mmProduct *= mmA + mmB;
					mmProduct *= mmA - mmB;
					mmProduct *= mmA * mmB;
					mmProduct *= mmA / mmB;
					mmProduct /= mmA;
					mmProduct /= mmA + mmB;
					mmProduct /= mmA - mmB;
					mmProduct /= mmA * mmB;
					mmProduct /= mmA / mmB;
				}

				std::decay_t<decltype(mm)> mmSum(0.0);
				{
					mmSum += mmA;
					mmSum += mmA + mmB;
					mmSum += mmA - mmB;
					mmSum += mmA * mmB;
					mmSum += (-mmA) * mmB;
					mmSum += mmA * (-mmB);
					mmSum += (-mmA) * (-mmB);
					mmSum += -(mmA * mmB);
					mmSum += mmA / mmB;
					mmSum += (-mmA) / mmB;
					mmSum += mmA / (-mmB);
					mmSum += (-mmA) / (-mmB);
					mmSum += -(mmA / mmB);
					mmSum -= mmA;
					mmSum -= mmA + mmB;
					mmSum -= mmA - mmB;
					mmSum -= mmA * mmB;
					mmSum -= (-mmA) * mmB;
					mmSum -= mmA * (-mmB);
					mmSum -= (-mmA) * (-mmB);
					mmSum -= -(mmA * mmB);
					mmSum -= mmA / mmB;
					mmSum -= (-mmA) / mmB;
					mmSum -= mmA / (-mmB);
					mmSum -= (-mmA) / (-mmB);
					mmSum -= -(mmA / mmB);
				}
			});
		}

	};
}