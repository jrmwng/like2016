#pragma once

/* Author: jrmwng @ 2015~2016 */

#include <intrin.h>
#include <stdarg.h>
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
			// 9 possible numbers in a cell
			SUDOKU_CANDIDATE_COUNT = 9,
			// 81 cells = 9 rows * 9 columns
			SUDOKU_CELL_COUNT = 81,
			// 27 groups = 9 box groups + 9 row groups + 9 column groups
			SUDOKU_GROUP_COUNT = 27,
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
		astCell[TT::SUDOKU_CELL_COUNT];

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
			long alCellIndex[TT::SUDOKU_CANDIDATE_COUNT]; // 9 numbers in each group
		}
		astGroup[TT::SUDOKU_GROUP_COUNT];

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
							auto & stCell = astCell[(x1 + x0 * TT::SUDOKU_SIZE) + (y1 + y0 * TT::SUDOKU_SIZE) * TT::SUDOKU_CANDIDATE_COUNT];
							auto & stGroupBox = astGroup[TT::SUDOKU_CANDIDATE_COUNT * 0 + x0 + y0 * TT::SUDOKU_SIZE];
							auto & stGroupCol = astGroup[TT::SUDOKU_CANDIDATE_COUNT * 1 + x1 + x0 * TT::SUDOKU_SIZE];
							auto & stGroupRow = astGroup[TT::SUDOKU_CANDIDATE_COUNT * 2 + y1 + y0 * TT::SUDOKU_SIZE];
							{
								stCell.lCandidateSet = (1 << TT::SUDOKU_CANDIDATE_COUNT) - 1;
								stCell.lGroupSet = 0;
								stCell.lGroupSet |= (1 << (&stGroupBox - astGroup));
								stCell.lGroupSet |= (1 << (&stGroupCol - astGroup));
								stCell.lGroupSet |= (1 << (&stGroupRow - astGroup));
								stGroupBox.alCellIndex[x1 + y1 * 3] = static_cast<long>(&stCell - astCell);
								stGroupCol.alCellIndex[y1 + y0 * 3] = static_cast<long>(&stCell - astCell);
								stGroupRow.alCellIndex[x1 + x0 * 3] = static_cast<long>(&stCell - astCell);
							}
						}
					}
				}
			}
		}
		void update(long lDirtyGroupSet)
		{
			// loop for dirty group-set
			for (__m128i xmmDirtyGroupSet = _mm_setzero_si128(); lDirtyGroupSet; lDirtyGroupSet = ((1 << TT::SUDOKU_GROUP_COUNT) - 1) & std::accumulate(std::cbegin(xmmDirtyGroupSet.m128i_i32), std::cend(xmmDirtyGroupSet.m128i_i32), 0, std::bit_or<int>()), xmmDirtyGroupSet = _mm_setzero_si128())
			{
				TT::printf(L"Dirty-Group-Set: %X\n", lDirtyGroupSet);

				// for each dirty group
				for (unsigned long ulDirtyGroupIndex; _BitScanForward(&ulDirtyGroupIndex, lDirtyGroupSet); _bittestandreset(&lDirtyGroupSet, ulDirtyGroupIndex))
				{
					auto const & stDirtyGroup = astGroup[ulDirtyGroupIndex];

					TT::printf(L"Dirty-Group: %lu\n", ulDirtyGroupIndex);

					if (TT::SUDOKU_CANDIDATE_COUNT == 9)
					{
						// union set of candidate sets of selected cell(s)
						std::tuple<__m128i, __m128i, __m128i> axmmUnionCandidateSet;
						// belonging group set of individual cell
						std::tuple<__m128i, __m128i, __m128i> axmmCellGroupSet;
						// candidate set of individual cell
						std::tuple<__m128i, __m128i, __m128i> axmmCellCandidateSet;
						{
							sudoku_for_each(std::make_index_sequence<3>(), [&](auto const i)
							{
								// alCell[0].lCandidateSet 0 alCell[0].lCandidateSet 0 alCell[0].lCandidateSet 0 alCell[0].lCandidateSet 0
								// [123456789] [] [123456789] [] [123456789] [] [123456789] []
								__m128i const xmmCellCandidateSet147 = _mm_unpacklo_epi16(_mm_setzero_si128(), _mm_shufflelo_epi16(_mm_cvtsi32_si128(astCell[stDirtyGroup.alCellIndex[i * 3 + 0]].lCandidateSet), _MM_SHUFFLE(0, 0, 0, 0)));
								// alCell[1].lCandidateSet alCell[1].lCandidateSet 0 0 alCell[1].lCandidateSet alCell[1].lCandidateSet 0 0
								// [123456789] [123456789] [] [] [123456789] [123456789] [] []
								__m128i const xmmCellCandidateSet258 = _mm_unpacklo_epi32(_mm_setzero_si128(), _mm_shufflelo_epi16(_mm_cvtsi32_si128(astCell[stDirtyGroup.alCellIndex[i * 3 + 1]].lCandidateSet), _MM_SHUFFLE(0, 0, 0, 0)));
								// alCell[2].lCandidateSet alCell[2].lCandidateSet alCell[2].lCandidateSet alCell[2].lCandidateSet 0 0 0 0
								// [123456789] [123456789] [123456789] [123456789] [] [] [] []
								__m128i const xmmCellCandidateSet369 = _mm_unpacklo_epi64(_mm_setzero_si128(), _mm_shufflelo_epi16(_mm_cvtsi32_si128(astCell[stDirtyGroup.alCellIndex[i * 3 + 2]].lCandidateSet), _MM_SHUFFLE(0, 0, 0, 0)));

								// 01FF 0000 01FF 0000 01FF 0000 01FF 0000
								// 01FF 01FF 0000 0000 01FF 01FF 0000 0000
								// 01FF 01FF 01FF 01FF 0000 0000 0000 0000
								__m128i const xmmUnionCandidateSet = _mm_or_si128(_mm_or_si128(xmmCellCandidateSet147, xmmCellCandidateSet258), xmmCellCandidateSet369);

								// 0 alCell[2].lGroupSet alCell[1].lGroupSet alCell[0].lGroupSet
								// 0 box[0]col[2]row[0] box[0]col[1]row[0] box[0]col[0]row[0]
								// 0 40801 40401 40201
								__m128i const xmmCellGroupSet = _mm_set_epi32(0, astCell[stDirtyGroup.alCellIndex[i * 3 + 2]].lGroupSet, astCell[stDirtyGroup.alCellIndex[i * 3 + 1]].lGroupSet, astCell[stDirtyGroup.alCellIndex[i * 3 + 0]].lGroupSet);

								// 0000 0000 0000 0000 0000 0000 0000 01FF
								// 0000 0000 0000 0000 0000 01FF 0000 0000
								// 0000 0000 0000 01FF 0000 0000 0000 0000
								__m128i const xmmCellCandidateSet = _mm_and_si128(_mm_shufflelo_epi16(xmmUnionCandidateSet, _MM_SHUFFLE(0, 2, 0, 1)), _mm_set_epi32(0, 0xFFFF, ~0, ~0));

								std::get<i.value>(axmmUnionCandidateSet) = xmmUnionCandidateSet;
								std::get<i.value>(axmmCellGroupSet) = xmmCellGroupSet;
								std::get<i.value>(axmmCellCandidateSet) = xmmCellCandidateSet;

								TT::printf(L"%S[%u]:\n\t%08X %08X %08X %08X\n", "axmmUnionCandidateSet", i.value, xmmUnionCandidateSet.m128i_u32[0], xmmUnionCandidateSet.m128i_u32[1], xmmUnionCandidateSet.m128i_u32[2], xmmUnionCandidateSet.m128i_u32[3]);
								//TT::printf(L"%S[%u]:\n\t%08X %08X %08X\n", "axmmCellGroupSet", i.value, xmmCellGroupSet.m128i_u32[0], xmmCellGroupSet.m128i_u32[1], xmmCellGroupSet.m128i_u32[2]);
								TT::printf(L"%S[%u]:\n\t%08X %08X %08X\n", "axmmCellCandidateSet", i.value, xmmCellCandidateSet.m128i_u32[0], xmmCellCandidateSet.m128i_u32[1], xmmCellCandidateSet.m128i_u32[2]);
							});
						}

						__m128i xmmLocalDirtyGroupSet = _mm_setzero_si128();
						for (long lCellSet789 = 0; lCellSet789 < (1 << TT::SUDOKU_CANDIDATE_COUNT); lCellSet789 += (1 << 6), std::get<2>(axmmUnionCandidateSet) = _mm_alignr_epi8(std::get<2>(axmmUnionCandidateSet), std::get<2>(axmmUnionCandidateSet), 2))
						{
							long const lCandidateSet789 = _mm_extract_epi16(std::get<2>(axmmUnionCandidateSet), 0);
							unsigned const uMinCntCell789 = __popcnt(lCellSet789);
							unsigned const uMaxCntCell789 = __popcnt(lCellSet789) + 6;
							unsigned const uMinCntCandidate789 = __popcnt(lCandidateSet789);
							unsigned const uMaxCntCandidate789 = __popcnt(lCandidateSet789 | _mm_extract_epi16(_mm_or_si128(std::get<1>(axmmUnionCandidateSet), std::get<0>(axmmUnionCandidateSet)), 7));

							if (uMinCntCandidate789 <= uMaxCntCell789 &&
								uMinCntCell789 <= uMaxCntCandidate789)
							{
								for (long lCellSet456789 = lCellSet789; lCellSet456789 < lCellSet789 + (1 << 6); lCellSet456789 += (1 << 3), std::get<1>(axmmUnionCandidateSet) = _mm_alignr_epi8(std::get<1>(axmmUnionCandidateSet), std::get<1>(axmmUnionCandidateSet), 2))
								{
									long const lCandidateSet456789 = lCandidateSet789 | _mm_extract_epi16(std::get<1>(axmmUnionCandidateSet), 0);
									unsigned const uMinCntCell456789 = __popcnt(lCellSet456789);
									unsigned const uMaxCntCell456789 = __popcnt(lCellSet456789) + 3;
									unsigned const uMinCntCandidate456789 = __popcnt(lCandidateSet456789);
									unsigned const uMaxCntCandidate456789 = __popcnt(lCandidateSet456789 | _mm_extract_epi16(std::get<0>(axmmUnionCandidateSet), 7));

									if (uMinCntCandidate456789 <= uMaxCntCell456789 &&
										uMinCntCell456789 <= uMaxCntCandidate456789)
									{
										for (long lCellSet123456789 = lCellSet456789; lCellSet123456789 < lCellSet456789 + (1 << 3); lCellSet123456789++, std::get<0>(axmmUnionCandidateSet) = _mm_alignr_epi8(std::get<0>(axmmUnionCandidateSet), std::get<0>(axmmUnionCandidateSet), 2))
										{
											long const lCandidateSet123456789 = lCandidateSet456789 | _mm_extract_epi16(std::get<0>(axmmUnionCandidateSet), 0);
											unsigned const uPopCntCell123456789 = __popcnt(lCellSet123456789);
											unsigned const uPopCntCandidate123456789 = __popcnt(lCandidateSet123456789);

											__m128i const xmmActiveCellSet = _mm_set1_epi32(lCellSet123456789);
											// 000001FF 000001FF 000001FF 000001FF
											__m128i const xmmUnionCandidateSet = _mm_set1_epi32(lCandidateSet123456789);

											if (uPopCntCandidate123456789 == uPopCntCell123456789)
											{
												sudoku_for_each(std::make_index_sequence<3>(), [&](auto const i)
												{
													// [i=0] 00000000 00000004 00000002 00000001
													// [i=1] 00000000 00000020 00000010 00000008
													// [i=2] 00000000 00000100 00000080 00000040
													__m128i const xmmActiveCellBit = _mm_and_si128(xmmActiveCellSet, _mm_set_epi32(0, (0x004 << (i * 3)), (0x002 << (i * 3)), (0x001 << (i * 3))));

													// FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF
													// FFFFFFFF FFFFFFFF FFFFFFFF 00000000
													// FFFFFFFF FFFFFFFF 00000000 FFFFFFFF
													// FFFFFFFF FFFFFFFF 00000000 00000000
													// FFFFFFFF 00000000 FFFFFFFF FFFFFFFF
													// FFFFFFFF 00000000 FFFFFFFF 00000000
													// FFFFFFFF 00000000 00000000 FFFFFFFF
													// FFFFFFFF 00000000 00000000 00000000
													__m128i const xmmInactiveCellMask = _mm_cmpeq_epi32(xmmActiveCellBit, _mm_setzero_si128());

													// 000001FF 000001FF 000001FF 00000000
													// 000001FF 000001FF 00000000 000001FF
													// 000001FF 00000000 000001FF 000001FF
													__m128i const xmmClearCandidateOfInactiveCell = _mm_and_si128(xmmInactiveCellMask, xmmUnionCandidateSet);

													__m128i const xmmNewCandidateSet = _mm_andnot_si128(xmmClearCandidateOfInactiveCell, std::get<i.value>(axmmCellCandidateSet));

													__m128i const xmmChangeGroupMask = _mm_cmplt_epi32(xmmNewCandidateSet, std::get<i.value>(axmmCellCandidateSet)); // because bit-31 is not used and we are clearing bit(s)

													std::get<i.value>(axmmCellCandidateSet) = xmmNewCandidateSet;

													__m128i const xmmChangeGroupSet = _mm_and_si128(xmmChangeGroupMask, std::get<i.value>(axmmCellGroupSet));

													xmmLocalDirtyGroupSet = _mm_or_si128(xmmLocalDirtyGroupSet, xmmChangeGroupSet);
												});
											}
										}
									}
								}
							}
						}
						xmmLocalDirtyGroupSet = _mm_and_si128(xmmLocalDirtyGroupSet, _mm_set1_epi32((1 << TT::SUDOKU_GROUP_COUNT) - 1));
						if (_mm_movemask_epi8(_mm_cmpeq_epi32(xmmLocalDirtyGroupSet, _mm_setzero_si128())) != 0xFFFF)
						{
							sudoku_for_each(std::make_index_sequence<3>(), [&](auto const i)
							{
								__m128i const xmmCellCandidateSet = std::get<i.value>(axmmCellCandidateSet);

								sudoku_for_each(std::make_index_sequence<3>(), [&](auto const j)
								{
									long const lCandidateSet = _mm_extract_epi32(xmmCellCandidateSet, j.value);

									astCell[stDirtyGroup.alCellIndex[i * 3 + j]].lCandidateSet = lCandidateSet;
								});

								TT::printf(L"%S[%u]:\n\t%08X %08X %08X\n", "axmmCellCandidateSet", i.value, xmmCellCandidateSet.m128i_u32[0], xmmCellCandidateSet.m128i_u32[1], xmmCellCandidateSet.m128i_u32[2]);
							});

							xmmDirtyGroupSet = _mm_or_si128(xmmDirtyGroupSet, xmmLocalDirtyGroupSet);
						}
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
									auto const & stCell = astCell[stDirtyGroup.alCellIndex[ulNumberIndex1]];

									lCandidateSet0 |= stCell.lCandidateSet;
								}
							}
							if (__popcnt(lCandidateSet0) == __popcnt(lNumberSet0)) // the only condition of inference
							{
								long lNumberSet2 = (~lNumberSet0) & ((1 << TT::SUDOKU_CANDIDATE_COUNT) - 1);
								for (unsigned long ulNumberIndex2; _BitScanForward(&ulNumberIndex2, lNumberSet2); _bittestandreset(&lNumberSet2, ulNumberIndex2))
								{
									auto & stCell = astCell[stDirtyGroup.alCellIndex[ulNumberIndex2]];

									if (stCell.lCandidateSet & lCandidateSet0)
									{
										xmmDirtyGroupSet = _mm_or_si128(xmmDirtyGroupSet, _mm_cvtsi32_si128(stCell.lGroupSet));
										stCell.lCandidateSet &= ~lCandidateSet0;
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
			auto & stCell = astCell[uX + uY * TT::SUDOKU_CANDIDATE_COUNT];

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

