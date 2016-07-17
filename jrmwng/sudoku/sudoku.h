#pragma once

/* Author: jrmwng @ 2015~2016 */

#include <intrin.h>
#include <stdarg.h>
#include <numeric>
#include <algorithm>
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
		template <typename T, typename Tint, Tint... tInt, typename Tfunc>
		T sudoku_accumulate(std::integer_sequence<Tint, tInt...>, T tAccumulator, Tfunc && tFunc)
		{
			using type = int [];
			(void) type {
				(tAccumulator = std::forward<Tfunc>(tFunc)(std::integral_constant<Tint, tInt>(), tAccumulator), 0)...
			};
			return tAccumulator;
		}
	}
	struct sudoku_9x9_traits
	{
		enum
		{
			SUDOKU_SIZE = 3,
			// 9 possible numbers in a cell
			SUDOKU_GROUP_CELL_COUNT = 9,
			// 81 cells = 9 rows * 9 columns
			SUDOKU_GRID_CELL_COUNT = 81,
			// 27 groups = 9 box groups + 9 row groups + 9 column groups
			SUDOKU_GRID_GROUP_COUNT = 27,
		};

		static void printf(wchar_t const *pcFormat, ...)
		{
#ifdef _DEBUG
			va_list pArgs;

			va_start(pArgs, pcFormat);
			vwprintf_s(pcFormat, pArgs);
			va_end(pArgs);
#endif
		}
	};

	template <typename TT>
	struct sudoku_t
	{
		struct cell_t
		{
			long lCandidateSet; // bits of potential values
			long lGroupSet; // bits of belonging groups
		}
		astGridCell[TT::SUDOKU_GRID_CELL_COUNT];

		// group in box (3x3)
		// 1 2 3
		// 4 5 6
		// 7 8 9
		//
		// group in row (1x9)
		// 1 2 3 4 5 6 7 8 9
		//
		// group in column (9x1)
		// 1
		// 2
		// 3
		// 4
		// 5
		// 6
		// 7
		// 8
		// 9
		struct group_t
		{
			// index of astCell
			unsigned auGridCellIndex[TT::SUDOKU_GROUP_CELL_COUNT]; // 9 numbers in each group
		}
		astGridGroup[TT::SUDOKU_GRID_GROUP_COUNT];

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
							auto & stCell = astGridCell[(x1 + x0 * TT::SUDOKU_SIZE) + (y1 + y0 * TT::SUDOKU_SIZE) * TT::SUDOKU_GROUP_CELL_COUNT];
							auto & stGroupBox = astGridGroup[TT::SUDOKU_GROUP_CELL_COUNT * 0 + x0 + y0 * TT::SUDOKU_SIZE];
							auto & stGroupCol = astGridGroup[TT::SUDOKU_GROUP_CELL_COUNT * 1 + x1 + x0 * TT::SUDOKU_SIZE];
							auto & stGroupRow = astGridGroup[TT::SUDOKU_GROUP_CELL_COUNT * 2 + y1 + y0 * TT::SUDOKU_SIZE];
							{
								stCell.lCandidateSet = (1 << TT::SUDOKU_GROUP_CELL_COUNT) - 1;
								stCell.lGroupSet = 0;
								stCell.lGroupSet |= (1 << (&stGroupBox - astGridGroup));
								stCell.lGroupSet |= (1 << (&stGroupCol - astGridGroup));
								stCell.lGroupSet |= (1 << (&stGroupRow - astGridGroup));
								stGroupBox.auGridCellIndex[x1 + y1 * 3] = static_cast<long>(&stCell - astGridCell);
								stGroupCol.auGridCellIndex[y1 + y0 * 3] = static_cast<long>(&stCell - astGridCell);
								stGroupRow.auGridCellIndex[x1 + x0 * 3] = static_cast<long>(&stCell - astGridCell);
							}
						}
					}
				}
			}
		}
		template <typename Tfunc>
		void update(long lGroupSet0, Tfunc && tFunc)
		{
			// loop for dirty group-set
			for (__m128i xmmGroupSet1 = _mm_setzero_si128(); lGroupSet0; lGroupSet0 = std::accumulate(std::cbegin(xmmGroupSet1.m128i_i32), std::cend(xmmGroupSet1.m128i_i32), 0, std::bit_or<int>()), xmmGroupSet1 = _mm_setzero_si128())
			{
				// for each dirty group
				for (unsigned long ulGroupIndex0; _BitScanForward(&ulGroupIndex0, lGroupSet0); _bittestandreset(&lGroupSet0, ulGroupIndex0))
				{
					auto const & stGroup = astGridGroup[ulGroupIndex0];

					if (TT::SUDOKU_GROUP_CELL_COUNT == 9)
					{
						long lGroupSet2 = 0;
						__m128i xmmCandidateSet0;
						__m128i xmmCandidateSet1;
						__m128i xmmCandidateSet2;
						__m128i xmmGroupSetA;
						__m128i xmmGroupSetB;
						__m128i xmmGroupSetC;
						__m128i axmmGroupSet[3];
						{
							__m128i axmmCandidateSet[3];
							{
								for (unsigned i = 0, j = 0; i < 3; i++, j += 3)
								{
									__m128i const xmmCandidateSet0 = _mm_unpacklo_epi16(_mm_setzero_si128(), _mm_shufflelo_epi16(_mm_cvtsi32_si128(astGridCell[stGroup.auGridCellIndex[j + 0]].lCandidateSet), 0));
									__m128i const xmmCandidateSet1 = _mm_unpacklo_epi32(_mm_setzero_si128(), _mm_shufflelo_epi16(_mm_cvtsi32_si128(astGridCell[stGroup.auGridCellIndex[j + 1]].lCandidateSet), 0));
									__m128i const xmmCandidateSet2 = _mm_unpacklo_epi64(_mm_setzero_si128(), _mm_shufflelo_epi16(_mm_cvtsi32_si128(astGridCell[stGroup.auGridCellIndex[j + 2]].lCandidateSet), 0));

									axmmCandidateSet[i] = _mm_or_si128(_mm_or_si128(xmmCandidateSet0, xmmCandidateSet1), xmmCandidateSet2);
									axmmGroupSet[i] = _mm_set_epi32(0, astGridCell[stGroup.auGridCellIndex[j + 2]].lGroupSet, astGridCell[stGroup.auGridCellIndex[j + 1]].lGroupSet, astGridCell[stGroup.auGridCellIndex[j + 0]].lGroupSet);
								}
							}
							xmmCandidateSet0 = axmmCandidateSet[0];
							xmmCandidateSet1 = axmmCandidateSet[1];
							xmmCandidateSet2 = axmmCandidateSet[2];
							xmmGroupSetA = axmmGroupSet[0];
							xmmGroupSetB = axmmGroupSet[1];
							xmmGroupSetC = axmmGroupSet[2];
						}
						__m128i xmmCandidateSetA = _mm_and_si128(_mm_shufflelo_epi16(xmmCandidateSet0, _MM_SHUFFLE(0, 2, 0, 1)), _mm_set_epi32(0, 0xFFFF, ~0, ~0));
						__m128i xmmCandidateSetB = _mm_and_si128(_mm_shufflelo_epi16(xmmCandidateSet1, _MM_SHUFFLE(0, 2, 0, 1)), _mm_set_epi32(0, 0xFFFF, ~0, ~0));
						__m128i xmmCandidateSetC = _mm_and_si128(_mm_shufflelo_epi16(xmmCandidateSet2, _MM_SHUFFLE(0, 2, 0, 1)), _mm_set_epi32(0, 0xFFFF, ~0, ~0));

						for (long lNumberSet2 = 0; lNumberSet2 < (1 << TT::SUDOKU_GROUP_CELL_COUNT); lNumberSet2 += (1 << 6), xmmCandidateSet2 = _mm_alignr_epi8(xmmCandidateSet2, xmmCandidateSet2, 2))
						{
							long const lCandidateSet2 = _mm_extract_epi16(xmmCandidateSet2, 0);
							unsigned const uMinCntNumber2 = __popcnt(lNumberSet2);
							unsigned const uMaxCntNumber2 = __popcnt(lNumberSet2) + 6;
							unsigned const uMinCntCandidate2 = __popcnt(lCandidateSet2);
							unsigned const uMaxCntCandidate2 = __popcnt(lCandidateSet2 | _mm_extract_epi16(_mm_or_si128(xmmCandidateSet1, xmmCandidateSet0), 7));

							if (uMinCntCandidate2 <= uMaxCntNumber2 &&
								uMinCntNumber2 <= uMaxCntCandidate2)
							{
								for (long lNumberSet1 = lNumberSet2; lNumberSet1 < lNumberSet2 + (1 << 6); lNumberSet1 += (1 << 3), xmmCandidateSet1 = _mm_alignr_epi8(xmmCandidateSet1, xmmCandidateSet1, 2))
								{
									long const lCandidateSet1 = lCandidateSet2 | _mm_extract_epi16(xmmCandidateSet1, 0);
									unsigned const uMinCntNumber1 = __popcnt(lNumberSet1);
									unsigned const uMaxCntNumber1 = __popcnt(lNumberSet1) + 3;
									unsigned const uMinCntCandidate1 = __popcnt(lCandidateSet1);
									unsigned const uMaxCntCandidate1 = __popcnt(lCandidateSet1 | _mm_extract_epi16(xmmCandidateSet0, 7));

									if (uMinCntCandidate1 <= uMaxCntNumber1 &&
										uMinCntNumber1 <= uMaxCntCandidate1)
									{
										for (long lNumberSet0 = lNumberSet1; lNumberSet0 < lNumberSet1 + (1 << 3); lNumberSet0++, xmmCandidateSet0 = _mm_alignr_epi8(xmmCandidateSet0, xmmCandidateSet0, 2))
										{
											long const lCandidateSet0 = lCandidateSet1 | _mm_extract_epi16(xmmCandidateSet0, 0);
											unsigned const uPopCntNumber0 = __popcnt(lNumberSet0);
											unsigned const uPopCntCandidate0 = __popcnt(lCandidateSet0);

											__m128i const xmmNumberSet = _mm_set1_epi32(lNumberSet0);
											__m128i const xmmCandidateSet = _mm_set1_epi32(lCandidateSet0);

											if (uPopCntCandidate0 == uPopCntNumber0)
											{
												__m128i const xmmNotNumberBitA = _mm_andnot_si128(xmmNumberSet, _mm_set_epi32(0, 0x004, 0x002, 0x001));
												__m128i const xmmNotNumberBitB = _mm_andnot_si128(xmmNumberSet, _mm_set_epi32(0, 0x020, 0x010, 0x008));
												__m128i const xmmNotNumberBitC = _mm_andnot_si128(xmmNumberSet, _mm_set_epi32(0, 0x100, 0x080, 0x040));

												__m128i const xmmNotNumberNullA = _mm_cmpeq_epi32(xmmNotNumberBitA, _mm_setzero_si128());
												__m128i const xmmNotNumberNullB = _mm_cmpeq_epi32(xmmNotNumberBitB, _mm_setzero_si128());
												__m128i const xmmNotNumberNullC = _mm_cmpeq_epi32(xmmNotNumberBitC, _mm_setzero_si128());

												__m128i const xmmCandidateClearA = _mm_andnot_si128(xmmNotNumberNullA, xmmCandidateSet);
												__m128i const xmmCandidateClearB = _mm_andnot_si128(xmmNotNumberNullB, xmmCandidateSet);
												__m128i const xmmCandidateClearC = _mm_andnot_si128(xmmNotNumberNullC, xmmCandidateSet);

												__m128i const xmmCandidateNewA = _mm_andnot_si128(xmmCandidateClearA, xmmCandidateSetA);
												__m128i const xmmCandidateNewB = _mm_andnot_si128(xmmCandidateClearB, xmmCandidateSetB);
												__m128i const xmmCandidateNewC = _mm_andnot_si128(xmmCandidateClearC, xmmCandidateSetC);

												__m128i const xmmCandidateSameA = _mm_cmpeq_epi32(xmmCandidateSetA, xmmCandidateNewA);
												__m128i const xmmCandidateSameB = _mm_cmpeq_epi32(xmmCandidateSetB, xmmCandidateNewB);
												__m128i const xmmCandidateSameC = _mm_cmpeq_epi32(xmmCandidateSetC, xmmCandidateNewC);

												xmmCandidateSetA = xmmCandidateNewA;
												xmmCandidateSetB = xmmCandidateNewB;
												xmmCandidateSetC = xmmCandidateNewC;
												xmmGroupSetA = _mm_and_si128(xmmCandidateSameA, xmmGroupSetA);
												xmmGroupSetB = _mm_and_si128(xmmCandidateSameB, xmmGroupSetB);
												xmmGroupSetC = _mm_and_si128(xmmCandidateSameC, xmmGroupSetC);
											}
										}
									}
								}
							}
						}
						__m128i const xmmGroupMaskA = _mm_cmpeq_epi32(xmmGroupSetA, _mm_setzero_si128());
						__m128i const xmmGroupMaskB = _mm_cmpeq_epi32(xmmGroupSetB, _mm_setzero_si128());
						__m128i const xmmGroupMaskC = _mm_cmpeq_epi32(xmmGroupSetC, _mm_setzero_si128());

						xmmGroupSetA = _mm_xor_si128(xmmGroupSetA, axmmGroupSet[0]);
						xmmGroupSetB = _mm_xor_si128(xmmGroupSetB, axmmGroupSet[1]);
						xmmGroupSetC = _mm_xor_si128(xmmGroupSetC, axmmGroupSet[2]);

						int anGroupMask[3];
						__m128i axmmCandidateSet[3];
						{
							anGroupMask[0] = _mm_movemask_epi8(xmmGroupMaskA) & 0xFFF;
							anGroupMask[1] = _mm_movemask_epi8(xmmGroupMaskB) & 0xFFF;
							anGroupMask[2] = _mm_movemask_epi8(xmmGroupMaskC) & 0xFFF;
							axmmCandidateSet[0] = xmmCandidateSetA;
							axmmCandidateSet[1] = xmmCandidateSetB;
							axmmCandidateSet[2] = xmmCandidateSetC;
						}
						for (int i = 0; i < std::size(anGroupMask); i++)
						{
							if (anGroupMask[i])
							{
								for (int j = 0; j < 3; j++)
								{
									astGridCell[stGroup.auGridCellIndex[j + i * 3]].lCandidateSet = axmmCandidateSet[i].m128i_i32[j];
								}
							}
						}

						xmmGroupSet1 = _mm_or_si128(_mm_or_si128(xmmGroupSet1, xmmGroupSetA), _mm_or_si128(xmmGroupSetB, xmmGroupSetC));
					}
					else // general case follows
					{
						for (long lNumberSet0 = 1; lNumberSet0 < (1 << TT::SUDOKU_GROUP_CELL_COUNT) - 1; lNumberSet0++)
						{
							long lCandidateSet0 = 0;
							{
								long lNumberSet1 = lNumberSet0;

								for (unsigned long ulNumberIndex1; _BitScanForward(&ulNumberIndex1, lNumberSet1); _bittestandreset(&lNumberSet1, ulNumberIndex1))
								{
									auto const & stNumber = astGridCell[stGroup.auGridCellIndex[ulNumberIndex1]];

									lCandidateSet0 |= stNumber.lCandidateSet;
								}
							}
							if (__popcnt(lCandidateSet0) == __popcnt(lNumberSet0)) // the only condition of inference
							{
								long lNumberSet2 = (~lNumberSet0) & ((1 << TT::SUDOKU_GROUP_CELL_COUNT) - 1);
								for (unsigned long ulNumberIndex2; _BitScanForward(&ulNumberIndex2, lNumberSet2); _bittestandreset(&lNumberSet2, ulNumberIndex2))
								{
									auto & stNumber = astGridCell[stGroup.auGridCellIndex[ulNumberIndex2]];

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
			auto & stCell = astGridCell[uX + uY * TT::SUDOKU_GROUP_CELL_COUNT];

			if (uZ-- > 0)
			{
				if (stCell.lCandidateSet != (1 << uZ))
				{
					stCell.lCandidateSet = (1 << uZ);
					return stCell.lGroupSet;
				}
			}
			return 0;
		}
	};
}

