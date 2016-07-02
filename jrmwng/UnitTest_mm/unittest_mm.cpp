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
				using Tmm = std::decay_t<decltype(mm)>;

				Tmm const mmA = mm;
				Tmm const mmB = mm;

				Tmm const mm_neg_AsubB = -(mmA - mmB);
				Tmm const mm_neg_negA = -(-mmA);
				auto const mm_negA = -mmA;

				auto const mm_negA_plus_negB = (-mmA) + (-mmB);
				Tmm const mm_negA_plus_B = (-mmA) + mmB;
				Tmm const mm_A_plus_negB = mmA + (-mmB);
				Tmm const mmAaddB = mmA + mmB;

				Tmm const mm_negA_sub_negB = (-mmA) - (-mmB);
				auto const mm_negA_sub_B = (-mmA) - mmB;
				Tmm const mm_A_sub_negB = mmA - (-mmB);
				Tmm const mmAsubB = mmA - mmB;

				Tmm const mm_negA_mul_negB = (-mmA) * (-mmB);
				auto const mm_negA_mul_B = (-mmA) * mmB;
				auto const mm_A_mul_negB = mmA * (-mmB);
				Tmm const mmAmulB = mmA * mmB;

				Tmm const mm_negA_div_negB = (-mmA) / (-mmB);
				auto const mm_negA_div_B = (-mmA) / mmB;
				auto const mm_A_div_negB = mmA / (-mmB);
				Tmm const mmAdivB = mmA / mmB;

				Tmm mmProduct(1.0);
				{
					mmProduct *= mmA;
					mmProduct *= mmA + mmB;
					mmProduct *= mmA - mmB;
					mmProduct *= mmA * mmB;
					mmProduct *= mmA / mmB;
					mmProduct *= (-mmA);
					mmProduct /= mmA;
					mmProduct /= mmA + mmB;
					mmProduct /= mmA - mmB;
					mmProduct /= mmA * mmB;
					mmProduct /= mmA / mmB;
					mmProduct /= (-mmA);
				}

				Tmm mmSum(0.0);
				{
					mmSum += mmA;
					mmSum += mmA + mmB;
					mmSum += mmA - mmB;
					mmSum += mmA * mmB;
					mmSum += mmA / mmB;
					mmSum += (-mmA);
					mmSum -= mmA;
					mmSum -= mmA + mmB;
					mmSum -= mmA - mmB;
					mmSum -= mmA * mmB;
					mmSum -= mmA / mmB;
					mmSum -= (-mmA);
				}
			});
		}

	};
}