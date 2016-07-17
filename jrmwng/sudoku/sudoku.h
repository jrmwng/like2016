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
			for (long lGroupSet1 = 0; lGroupSet0; lGroupSet0 = lGroupSet1, lGroupSet1 = 0)
			{
				// for each dirty group
				for (unsigned long ulGroupIndex0; _BitScanForward(&ulGroupIndex0, lGroupSet0); _bittestandreset(&lGroupSet0, ulGroupIndex0))
				{
					auto const & stGroup = astGridGroup[ulGroupIndex0];

					if (TT::SUDOKU_GROUP_CELL_COUNT == 9)
					{
						__m128i axmmCandidateSet[3];
						{
							for (unsigned i = 0, j = 0; i < 3; i++, j += 3)
							{
								__m128i const xmmCandidateSet0 = _mm_unpacklo_epi16(_mm_setzero_si128(), _mm_shufflelo_epi16(_mm_cvtsi32_si128(astGridCell[stGroup.auGridCellIndex[j + 0]].lCandidateSet), 0));
								__m128i const xmmCandidateSet1 = _mm_unpacklo_epi32(_mm_setzero_si128(), _mm_shufflelo_epi16(_mm_cvtsi32_si128(astGridCell[stGroup.auGridCellIndex[j + 1]].lCandidateSet), 0));
								__m128i const xmmCandidateSet2 = _mm_unpacklo_epi64(_mm_setzero_si128(), _mm_shufflelo_epi16(_mm_cvtsi32_si128(astGridCell[stGroup.auGridCellIndex[j + 2]].lCandidateSet), 0));

								axmmCandidateSet[i] = _mm_or_si128(_mm_or_si128(xmmCandidateSet0, xmmCandidateSet1), xmmCandidateSet2);
							}
						}

						for (long lNumberSet0 = 0; lNumberSet0 < (1 << TT::SUDOKU_GROUP_CELL_COUNT); lNumberSet0 += 8)
						{
							long const lCandidateSet0 = axmmCandidateSet[1].m128i_i16[(lNumberSet0 >> 3) & 7] | axmmCandidateSet[2].m128i_i16[(lNumberSet0 >> 6) & 7];

							if (__popcnt(lCandidateSet0) <= __popcnt(lNumberSet0) && __popcnt(lNumberSet0) <= __popcnt(lCandidateSet0) + 3)
							{
								for (long lNumberSet1 = 0; lNumberSet1 < 8; lNumberSet1++)
								{
									long const lCandidateSet1 = lCandidateSet0 | axmmCandidateSet[0].m128i_i16[lNumberSet1];

									if (__popcnt(lCandidateSet1) == __popcnt(lNumberSet0 + lNumberSet1))
									{
										long lNumberSet2 = (~(lNumberSet0 + lNumberSet1)) & ((1 << TT::SUDOKU_GROUP_CELL_COUNT) - 1);
										for (unsigned long ulNumberIndex2; _BitScanForward(&ulNumberIndex2, lNumberSet2); _bittestandreset(&lNumberSet2, ulNumberIndex2))
										{
											auto & stNumber = astGridCell[stGroup.auGridCellIndex[ulNumberIndex2]];

											if (stNumber.lCandidateSet & lCandidateSet1)
											{
												lGroupSet1 |= stNumber.lGroupSet;
												stNumber.lCandidateSet &= ~lCandidateSet1;
											}
										}
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

