#pragma once

/* Author: jrmwng @ 2015~2016 */

#include <intrin.h>
#include <numeric>
#include <functional>

namespace jrmwng
{
	namespace
	{
		template <typename Tint, Tint... tInt, typename Tfunc>
		void sudoku_for_each(std::integer_sequence<Tint, tInt...>, Tfunc && tFunc)
		{
			using type = int [];
			(void) type {
				(std::forward<Tfunc>(tFunc)(std::integral_constant<Tint, tInt>()), 0)...
			};
		}
	}
	struct sudoku_9x9_traits
	{
		enum
		{
			SUDOKU_SIZE = 3,
			SUDOKU_CANDIDATE_COUNT = 9,
			SUDOKU_NUMBER_COUNT = 81,
			SUDOKU_GROUP_COUNT = 27,
		};
	};

	template <typename TT>
	struct sudoku_t
	{
		struct number_t
		{
			long lCandidateSet;
			long lGroupSet;
		} astNumber[TT::SUDOKU_NUMBER_COUNT];

		struct group_t
		{
			long alNumberIndex[TT::SUDOKU_CANDIDATE_COUNT];
		} astGroup[TT::SUDOKU_GROUP_COUNT];

		sudoku_t(void)
		{
			for (unsigned y0 = 0; y0 < TT::SUDOKU_SIZE; y0++)
			{
				for (unsigned y1 = 0; y1 < TT::SUDOKU_SIZE; y1++)
				{
					for (unsigned x0 = 0; x0 < TT::SUDOKU_SIZE; x0++)
					{
						for (unsigned x1 = 0; x1 < TT::SUDOKU_SIZE; x1++)
						{
							auto & stNumber = astNumber[(x1 + x0 * TT::SUDOKU_SIZE) + (y1 + y0 * TT::SUDOKU_SIZE) * TT::SUDOKU_CANDIDATE_COUNT];
							auto & stGroupBox = astGroup[TT::SUDOKU_CANDIDATE_COUNT * 0 + x0 + y0 * TT::SUDOKU_SIZE];
							auto & stGroupCol = astGroup[TT::SUDOKU_CANDIDATE_COUNT * 1 + x1 + x0 * TT::SUDOKU_SIZE];
							auto & stGroupRow = astGroup[TT::SUDOKU_CANDIDATE_COUNT * 2 + y1 + y0 * TT::SUDOKU_SIZE];
							{
								stNumber.lCandidateSet = (1 << TT::SUDOKU_CANDIDATE_COUNT) - 1;
								stNumber.lGroupSet = 0;
								stNumber.lGroupSet |= (1 << (&stGroupBox - astGroup));
								stNumber.lGroupSet |= (1 << (&stGroupCol - astGroup));
								stNumber.lGroupSet |= (1 << (&stGroupRow - astGroup));
								stGroupBox.alNumberIndex[x1 + y1 * 3] = static_cast<long>(&stNumber - astNumber);
								stGroupCol.alNumberIndex[y1 + y0 * 3] = static_cast<long>(&stNumber - astNumber);
								stGroupRow.alNumberIndex[x1 + x0 * 3] = static_cast<long>(&stNumber - astNumber);
							}
						}
					}
				}
			}
		}
		void update(long lGroupSet0)
		{
			// loop for dirty group-set
			for (__m128i xmmGroupSet1 = _mm_setzero_si128(); lGroupSet0; lGroupSet0 = std::accumulate(std::cbegin(xmmGroupSet1.m128i_i32), std::cend(xmmGroupSet1.m128i_i32), 0, std::bit_or<int>()), xmmGroupSet1 = _mm_setzero_si128())
			{
				// for each dirty group
				for (unsigned long ulGroupIndex0; _BitScanForward(&ulGroupIndex0, lGroupSet0); _bittestandreset(&lGroupSet0, ulGroupIndex0))
				{
					auto const & stGroup = astGroup[ulGroupIndex0];

					if (TT::SUDOKU_CANDIDATE_COUNT == 9)
					{
						long lGroupSet2 = 0;
						std::tuple<__m128i, __m128i, __m128i> axmmCandidateSet012;
						std::tuple<__m128i, __m128i, __m128i> axmmGroupSetABC;
						std::tuple<__m128i, __m128i, __m128i> axmmGroupSet;
						std::tuple<__m128i, __m128i, __m128i> axmmCandidateSetABC;
						{
							sudoku_for_each(std::make_index_sequence<3>(), [&](auto const i)
							{
								__m128i const xmmCandidateSet0 = _mm_unpacklo_epi16(_mm_setzero_si128(), _mm_shufflelo_epi16(_mm_cvtsi32_si128(astNumber[stGroup.alNumberIndex[i * 3 + 0]].lCandidateSet), 0));
								__m128i const xmmCandidateSet1 = _mm_unpacklo_epi32(_mm_setzero_si128(), _mm_shufflelo_epi16(_mm_cvtsi32_si128(astNumber[stGroup.alNumberIndex[i * 3 + 1]].lCandidateSet), 0));
								__m128i const xmmCandidateSet2 = _mm_unpacklo_epi64(_mm_setzero_si128(), _mm_shufflelo_epi16(_mm_cvtsi32_si128(astNumber[stGroup.alNumberIndex[i * 3 + 2]].lCandidateSet), 0));

								__m128i const xmmCandidateSet012 = _mm_or_si128(_mm_or_si128(xmmCandidateSet0, xmmCandidateSet1), xmmCandidateSet2);
								__m128i const xmmGroupSet = _mm_set_epi32(0, astNumber[stGroup.alNumberIndex[i * 3 + 2]].lGroupSet, astNumber[stGroup.alNumberIndex[i * 3 + 1]].lGroupSet, astNumber[stGroup.alNumberIndex[i * 3 + 0]].lGroupSet);

								std::get<i.value>(axmmCandidateSet012) = xmmCandidateSet012;
								std::get<i.value>(axmmGroupSet) = xmmGroupSet;
								std::get<i.value>(axmmGroupSetABC) = xmmGroupSet;
								std::get<i.value>(axmmCandidateSetABC) = _mm_and_si128(_mm_shufflelo_epi16(std::get<i.value>(axmmCandidateSet012), _MM_SHUFFLE(0, 2, 0, 1)), _mm_set_epi32(0, 0xFFFF, ~0, ~0));
							});
						}

						for (long lNumberSet2 = 0; lNumberSet2 < (1 << TT::SUDOKU_CANDIDATE_COUNT); lNumberSet2 += (1 << 6), std::get<2>(axmmCandidateSet012) = _mm_alignr_epi8(std::get<2>(axmmCandidateSet012), std::get<2>(axmmCandidateSet012), 2))
						{
							long const lCandidateSet2 = _mm_extract_epi16(std::get<2>(axmmCandidateSet012), 0);
							unsigned const uMinCntNumber2 = __popcnt(lNumberSet2);
							unsigned const uMaxCntNumber2 = __popcnt(lNumberSet2) + 6;
							unsigned const uMinCntCandidate2 = __popcnt(lCandidateSet2);
							unsigned const uMaxCntCandidate2 = __popcnt(lCandidateSet2 | _mm_extract_epi16(_mm_or_si128(std::get<1>(axmmCandidateSet012), std::get<0>(axmmCandidateSet012)), 7));

							if (uMinCntCandidate2 <= uMaxCntNumber2 &&
								uMinCntNumber2 <= uMaxCntCandidate2)
							{
								for (long lNumberSet1 = lNumberSet2; lNumberSet1 < lNumberSet2 + (1 << 6); lNumberSet1 += (1 << 3), std::get<1>(axmmCandidateSet012) = _mm_alignr_epi8(std::get<1>(axmmCandidateSet012), std::get<1>(axmmCandidateSet012), 2))
								{
									long const lCandidateSet1 = lCandidateSet2 | _mm_extract_epi16(std::get<1>(axmmCandidateSet012), 0);
									unsigned const uMinCntNumber1 = __popcnt(lNumberSet1);
									unsigned const uMaxCntNumber1 = __popcnt(lNumberSet1) + 3;
									unsigned const uMinCntCandidate1 = __popcnt(lCandidateSet1);
									unsigned const uMaxCntCandidate1 = __popcnt(lCandidateSet1 | _mm_extract_epi16(std::get<0>(axmmCandidateSet012), 7));

									if (uMinCntCandidate1 <= uMaxCntNumber1 &&
										uMinCntNumber1 <= uMaxCntCandidate1)
									{
										for (long lNumberSet0 = lNumberSet1; lNumberSet0 < lNumberSet1 + (1 << 3); lNumberSet0++, std::get<0>(axmmCandidateSet012) = _mm_alignr_epi8(std::get<0>(axmmCandidateSet012), std::get<0>(axmmCandidateSet012), 2))
										{
											long const lCandidateSet0 = lCandidateSet1 | _mm_extract_epi16(std::get<0>(axmmCandidateSet012), 0);
											unsigned const uPopCntNumber0 = __popcnt(lNumberSet0);
											unsigned const uPopCntCandidate0 = __popcnt(lCandidateSet0);

											__m128i const xmmNumberSet = _mm_set1_epi32(lNumberSet0);
											__m128i const xmmCandidateSet = _mm_set1_epi32(lCandidateSet0);

											if (uPopCntCandidate0 == uPopCntNumber0)
											{
												sudoku_for_each(std::make_index_sequence<3>(), [&](auto const i)
												{
													__m128i const xmmNotNumberBit = _mm_andnot_si128(xmmNumberSet, _mm_set_epi32(0, (0x004 << (i * 3)), (0x002 << (i * 3)), (0x001 << (i * 3))));

													__m128i const xmmNotNumberNull = _mm_cmpeq_epi32(xmmNotNumberBit, _mm_setzero_si128());

													__m128i const xmmCandidateClear = _mm_andnot_si128(xmmNotNumberNull, xmmCandidateSet);

													__m128i const xmmCandidateNew = _mm_andnot_si128(xmmCandidateClear, std::get<i.value>(axmmCandidateSetABC));

													__m128i const xmmCandidateSame = _mm_cmpeq_epi32(std::get<i.value>(axmmCandidateSetABC), xmmCandidateNew);

													std::get<i.value>(axmmCandidateSetABC) = xmmCandidateNew;

													std::get<i.value>(axmmGroupSetABC) = _mm_and_si128(xmmCandidateSame, std::get<i.value>(axmmGroupSetABC));
												});
											}
										}
									}
								}
							}
						}
						sudoku_for_each(std::make_index_sequence<3>(), [&](auto const i)
						{
							__m128i const xmmGroupMask = _mm_cmpeq_epi32(std::get<i.value>(axmmGroupSetABC), _mm_setzero_si128());

							std::get<i.value>(axmmGroupSetABC) = _mm_xor_si128(std::get<i.value>(axmmGroupSetABC), std::get<i.value>(axmmGroupSet));

							int const nGroupMask = _mm_movemask_epi8(xmmGroupMask) & 0xFFF;

							if (nGroupMask)
							{
								sudoku_for_each(std::make_index_sequence<3>(), [&](auto const j)
								{
									astNumber[stGroup.alNumberIndex[i * 3 + j]].lCandidateSet = _mm_extract_epi32(std::get<i.value>(axmmCandidateSetABC), j.value);
								});
							}
						});

						xmmGroupSet1 = _mm_or_si128(_mm_or_si128(xmmGroupSet1, std::get<0>(axmmGroupSetABC)), _mm_or_si128(std::get<1>(axmmGroupSetABC), std::get<2>(axmmGroupSetABC)));
					}
					else // general case follows
					{
						for (long lNumberSet0 = 1; lNumberSet0 < (1 << TT::SUDOKU_CANDIDATE_COUNT) - 1; lNumberSet0++)
						{
							long lCandidateSet0 = 0;
							{
								long lNumberSet1 = lNumberSet0;

								for (unsigned long ulNumberIndex1; _BitScanForward(&ulNumberIndex1, lNumberSet1); _bittestandreset(&lNumberSet1, ulNumberIndex1))
								{
									auto const & stNumber = astNumber[stGroup.alNumberIndex[ulNumberIndex1]];

									lCandidateSet0 |= stNumber.lCandidateSet;
								}
							}
							if (__popcnt(lCandidateSet0) == __popcnt(lNumberSet0)) // the only condition of inference
							{
								long lNumberSet2 = (~lNumberSet0) & ((1 << TT::SUDOKU_CANDIDATE_COUNT) - 1);
								for (unsigned long ulNumberIndex2; _BitScanForward(&ulNumberIndex2, lNumberSet2); _bittestandreset(&lNumberSet2, ulNumberIndex2))
								{
									auto & stNumber = astNumber[stGroup.alNumberIndex[ulNumberIndex2]];

									if (stNumber.lCandidateSet & lCandidateSet0)
									{
										xmmGroupSet1 = _mm_or_si128(xmmGroupSet1, _mm_cvtsi32_si128(stNumber.lGroupSet));
										stNumber.lCandidateSet &= ~lCandidateSet0;
									}
								}
							}
						}
					}
				}
			}
		}
		long set(unsigned uX, unsigned uY, unsigned uZ)
		{
			auto & stNumber = astNumber[uX + uY * TT::SUDOKU_CANDIDATE_COUNT];

			if (uZ-- > 0)
			{
				if (stNumber.lCandidateSet != (1 << uZ))
				{
					stNumber.lCandidateSet = (1 << uZ);
					return stNumber.lGroupSet;
				}
			}
			return 0;
		}
	};
}

