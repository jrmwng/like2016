#pragma once

/* Author: jrmwng @ 2016 */

#include <intrin.h>
#include <utility>
#include <algorithm>

#include <Windows.h>

namespace jrmwng
{
	namespace
	{
		template <typename T>
		T * memory_align(void *pMem, size_t uAlign)
		{
			return reinterpret_cast<T*>((reinterpret_cast<uintptr_t>(pMem) + (uAlign - 1U)) & ~(uAlign - 1U));
		}
		template <typename T1, size_t u1, typename T2, size_t u2, typename Tfunc, size_t... uIndex>
		void memory_transform(T1 const (&at1)[u1], T2(&at2)[u2], Tfunc && tFunc, std::index_sequence<uIndex...>)
		{
			using type_t = int [];

			(void) type_t {
				(at2[uIndex] = std::forward<Tfunc>(tFunc)(at1[uIndex]), 0)...
			};
		}
		template <typename T1, size_t u1, typename T2, size_t u2, typename Tfunc>
		void memory_transform(T1 const (&at1)[u1], T2(&at2)[u2], Tfunc && tFunc)
		{
			memory_transform(at1, at2, std::forward<Tfunc>(tFunc), std::make_index_sequence<(u1 < u2 ? u1 : u2)>());
		}
		struct alignas(128) memory_stack
		{
			void * m_pBufferBegin;
			void * m_pBufferEnd;
			void * m_pBufferCurrent;

			struct frame
			{
				void *m_pBuffer;
				frame *m_pstFramePrev;
			};
			frame *m_pstFrameCurrent;

			memory_stack(char *pcBuffer, size_t uBufferSize)
				: m_pBufferBegin(pcBuffer)
				, m_pBufferEnd(pcBuffer + uBufferSize)
				, m_pBufferCurrent(pcBuffer)
				, m_pstFrameCurrent(nullptr)
			{}
			~memory_stack()
			{
				if (m_pBufferCurrent != m_pBufferBegin)
				{
					__debugbreak();
				}
			}

			bool push()
			{
				frame *pstFrame = memory_align<frame>(m_pBufferCurrent, alignof(frame));
				{
					if (!(pstFrame + 1 <= static_cast<void*>(m_pBufferEnd)))
					{
						return false;
					}
					pstFrame->m_pBuffer = m_pBufferCurrent;
					pstFrame->m_pstFramePrev = m_pstFrameCurrent;
				}

				m_pBufferCurrent = pstFrame + 1;
				m_pstFrameCurrent = pstFrame;
				return true;
			}
			bool pop()
			{
				frame *pstFrame = m_pstFrameCurrent;
				{
					if (pstFrame == nullptr)
					{
						return false;
					}
				}

				m_pBufferCurrent = pstFrame->m_pBuffer;
				m_pstFrameCurrent = pstFrame->m_pstFramePrev;
				return true;
			}

			void * malloc(size_t uSize, size_t uAlign = 4)
			{
				char *pcMem = memory_align<char>(m_pBufferCurrent, uAlign);
				{
					if (!(pcMem + uSize <= m_pBufferEnd))
					{
						return nullptr;
					}
				}
				m_pBufferCurrent = pcMem + uSize;
				return pcMem;
			}
		};

		static unsigned const g_auAlignMask [] =
		{
			0x00000000U, // 0x00
			0xFFFFFFFFU, // 0x01
			0x55555555U, // 0x02
			0x11111111U, // 0x03
			0x11111111U, // 0x04
			0x01010101U, // 0x05
			0x01010101U, // 0x06
			0x01010101U, // 0x07
			0x01010101U, // 0x08
			0x00010001U, // 0x09
			0x00010001U, // 0x0A
			0x00010001U, // 0x0B
			0x00010001U, // 0x0C
			0x00010001U, // 0x0D
			0x00010001U, // 0x0E
			0x00010001U, // 0x0F
			0x00010001U, // 0x10
			0x00000001U, // 0x11
			0x00000001U, // 0x12
			0x00000001U, // 0x13
			0x00000001U, // 0x14
			0x00000001U, // 0x15
			0x00000001U, // 0x16
			0x00000001U, // 0x17
			0x00000001U, // 0x18
			0x00000001U, // 0x19
			0x00000001U, // 0x20
			0x00000001U, // 0x21
			0x00000001U, // 0x22
			0x00000001U, // 0x23
			0x00000001U, // 0x24
			0x00000001U, // 0x25
			0x00000001U, // 0x26
			0x00000001U, // 0x27
			0x00000001U, // 0x28
			0x00000001U, // 0x29
			0x00000001U, // 0x30
			0x00000001U, // 0x31
			0x00000001U, // 0x32
		};
		template <unsigned uGRANULARITY, unsigned u1X>
		struct alignas(128) memory_heap
		{
			enum { u256X = (u1X + 255) / 256 };
			enum { u128X = (u1X + 127) / 128 };
			enum { u64X = (u1X + 63) / 64 };
			enum { u32X = (u1X + 31) / 32 };
			enum { u16X = (u1X + 15) / 16 };
			enum { u8X = (u1X + 7) / 8 };
			enum { u4X = (u1X + 3) / 4 };
			enum { u2X = (u1X + 1) / 2 };
			union
			{
				__m256i m_aymmBitLock[u256X];
				__m128i m_aaxmmBitLock[u256X][2];
				__m128i m_axmmBitLock[u256X * 2];
				long long volatile m_aaxlBitLock[u256X][4];
				long long volatile m_axlBitLock[u256X * 4];
				unsigned volatile m_aauBitLock[u256X][8];
				unsigned volatile m_auBitLock[u256X * 8];
				long volatile m_aalBitLock[u256X][8];
				long volatile m_alBitLock[u256X * 8];
				char volatile m_aacBitLock[u256X][32];
				char volatile m_acBitLock[u256X * 32];
			};

			union
			{
				void * const m_pBuffer;
				char * const m_pcBuffer;
			};

			memory_heap()
			{
				// NOP
			}
			memory_heap(void *pBuffer, unsigned uCount)
				: m_pBuffer(pBuffer)
			{
				unsigned const uCount256 = uCount / 256U;
				unsigned const uCount32 = (uCount % 256U) / 32U;
				unsigned const uCount1 = (uCount % 256U) % 32U;

				if (uCount256)
				{
					std::fill_n(m_aymmBitLock, uCount256, _mm256_setzero_si256());
				}
				if (uCount32)
				{
					std::fill_n(m_aymmBitLock[uCount256].m256i_u32, uCount32, 0U);
				}
				if (uCount1)
				{
					m_aymmBitLock[uCount256].m256i_u32[uCount32] = (~0U) + (1U << uCount);
				}
			}

			struct alloc_info
			{
				memory_heap *pHeap;
				unsigned uIndex;
				unsigned uCount;
			};

			template <typename Tmm, size_t uMulti>
			void * malloc_by_count(size_t uCount, size_t uAlign);
			template <>
			void * malloc_by_count<__m256i, 1>(size_t uCount1, size_t uAlign)
			{
				unsigned const uAlignMask = g_auAlignMask[uCount1];

				__m256i const uw16AlignMaskLSB = _mm256_set1_epi32(uAlignMask);
				__m256i const uw16AlignMaskMSB = _mm256_set1_epi32(uAlignMask << (uCount1 - 1U));

				__m256i aul8HitLock[u256X];
				{
					memory_transform(m_aymmBitLock, aul8HitLock, [&](__m256i const & ymmBitLock)->__m256i
					{
						__m256i const uw16OldLock0 = ymmBitLock;

						__m256i const uw16XorLock1 = _mm256_xor_si256(uw16OldLock0, uw16AlignMaskLSB);
						__m256i const uw16AvgLock1 = _mm256_avg_epu16(uw16OldLock0, uw16AlignMaskLSB);

						__m256i const uw16AvgLock2 = _mm256_avg_epu16(uw16XorLock1, _mm256_setzero_si256());

						__m256i const uw16XorLock3 = _mm256_xor_si256(uw16AvgLock1, uw16AvgLock2);

						__m256i const uw16AndLock4 = _mm256_and_si256(uw16XorLock3, uw16AlignMaskMSB);

						return uw16AndLock4;
					});
				}

				for (unsigned uIndex256 = u256X; 0 < uIndex256--; )
				{
					__m256i const ul8HitLock = aul8HitLock[uIndex256];

					__m256i const ul8CmpLock5 = _mm256_cmpeq_epi32(ul8HitLock, _mm256_setzero_si256());
					__m256i ul8HitLockLSB = _mm256_srl_epi32(ul8HitLock, _mm_cvtsi32_si128(uCount1 - 1));
					__m256i ul8HitLockMSB = ul8HitLock;

					int const nCmpLock6 = _mm256_movemask_epi8(ul8CmpLock5);

					if (~nCmpLock6 != 0)
					{
						for (unsigned uIndex64 = 0; uIndex64 < 8; uIndex64++)
						{
							unsigned long long uxlHitLockLSB = _mm_cvtsi128_si64(_mm256_castsi256_si128(ul8HitLockLSB));
							unsigned long long uxlHitLockMSB = _mm_cvtsi128_si64(_mm256_castsi256_si128(ul8HitLockMSB));

							ul8HitLockLSB = _mm256_permute4x64_epi64(ul8HitLockLSB, _MM_SHUFFLE(0, 3, 2, 1));
							ul8HitLockMSB = _mm256_permute4x64_epi64(ul8HitLockMSB, _MM_SHUFFLE(0, 3, 2, 1));

							if (uxlHitLockLSB)
							{
								unsigned const ulPattern = ~0U + (1U << uCount1);

								for (unsigned uIndex1 = _lzcnt_u64(uxlHitLockLSB); uIndex1 < 64; uIndex1 = _lzcnt_u64(uxlHitLockLSB))
								{
									unsigned const uIndex = uIndex256 * 256 + uIndex64 * 64 + uIndex1;

									alloc_info *pstInfo = memory_align<alloc_info>(m_pcBuffer + uGRANULARITY * uIndex + sizeof(alloc_info), uAlign) - 1;
									{
										_mm_prefetch(reinterpret_cast<char const*>(pstInfo), _MM_HINT_T0);

										unsigned volatile & ulBitLock = m_aauBitLock[uIndex256][uIndex64];
										long volatile & lBitLock = m_aalBitLock[uIndex256][uIndex64];

										unsigned const ulOldLock1 = ulBitLock;
										unsigned const ulPattern1 = ulPattern << uIndex1;

										_bittestandreset(reinterpret_cast<long*>(&uxlHitLockLSB), uIndex1);
										_bittestandreset(reinterpret_cast<long*>(&uxlHitLockMSB), uIndex1 + (uCount1 - 1));

										if ((ulOldLock1 & ulPattern1) != ulPattern1)
										{
											if (_InterlockedCompareExchange(&lBitLock, ulOldLock1 ^ ulPattern1, ulOldLock1) == ulOldLock1)
											{
												pstInfo->pHeap = this;
												pstInfo->uIndex = uIndex;
												pstInfo->uCount = uCount1;
												return pstInfo + 1;
											}
										}
									}
								}
							}
						}
					}
				}
				return nullptr;
			}
			template <>
			void * malloc_by_count<__m256i, 32>(size_t uCount32, size_t uAlign)
			{
				unsigned const uAlignMask32LSB = g_auAlignMask[uCount32];

				__m256i const uw16AlignMask32LSB = _mm256_set1_epi32(uAlignMask32LSB);
				__m256i const uw16AlignMask32MSB = _mm256_set1_epi32(uAlignMask32LSB << (uCount32 - 1U));

				__m256i ymmOldLock32;
				{
					static_assert(u256X <= 32, "u256X should not exceed 32 by design (ymmOldLock32)");

					memory_transform(m_aymmBitLock, ymmOldLock32.m256i_i8, [](__m256i const & ymmBitLock)->char
					{
						return static_cast<char>(_mm256_movemask_ps(_mm256_castsi256_ps(_mm256_cmpeq_epi32(ymmBitLock, _mm256_set1_epi32(~0)))));
					});
				}
				__m256i ymmHitLock32MSB;
				__m256i ymmHitLock32LSB;
				{
					__m256i const uw16OldLock0 = ymmOldLock32;

					__m256i const uw16XorLock1 = _mm256_xor_si256(uw16OldLock0, uw16AlignMask32LSB);
					__m256i const uw16AvgLock1 = _mm256_avg_epu16(uw16OldLock0, uw16AlignMask32LSB);

					__m256i const uw16AvgLock2 = _mm256_avg_epu16(uw16XorLock1, _mm256_setzero_si256());

					__m256i const uw16XorLock3 = _mm256_xor_si256(uw16AvgLock1, uw16AvgLock2);

					__m256i const uw16AndLock4 = _mm256_and_si256(uw16XorLock3, uw16AlignMask32MSB);

					ymmHitLock32MSB = uw16AndLock4;
					ymmHitLock32LSB = _mm256_srl_epi16(uw16AndLock4, _mm_cvtsi32_si128(uCount32 - 1));
				}
				__m256i const ymmCmpLock = _mm256_cmpeq_epi8(ymmHitLock32MSB, _mm256_setzero_si256());

				int const nCmpLock = _mm256_movemask_epi8(ymmCmpLock);

				if (~nCmpLock != 0)
				{
					for (unsigned uIndex2048 = 0; uIndex2048 * 8 < u256X; uIndex2048++) // 2048 = 32 * 64
					{
						unsigned long long const uxlHitLock8MSB = _mm_cvtsi128_si64(_mm256_castsi256_si128(ymmHitLock32MSB));
						unsigned long long const uxlHitLock8LSB = _mm_cvtsi128_si64(_mm256_castsi256_si128(ymmHitLock32LSB));

						ymmHitLock32LSB = _mm256_permute4x64_epi64(ymmHitLock32LSB, _MM_SHUFFLE(0, 3, 2, 1));
						ymmHitLock32MSB = _mm256_permute4x64_epi64(ymmHitLock32MSB, _MM_SHUFFLE(0, 3, 2, 1));

						for (unsigned uIndex64 = _tzcnt_u64(uxlHitLock8LSB); uIndex64 < 64; uIndex64 = _tzcnt_u64(uxlHitLock8LSB))
						{
							unsigned const uIndex = uIndex2048 * 2048U + uIndex64 * 64U;
							unsigned const uIndex32 = uIndex / 32U;

							_bittestandreset64(reinterpret_cast<long long*>(&uxlHitLock8LSB), uIndex64);
							_bittestandreset64(reinterpret_cast<long long*>(&uxlHitLock8MSB), uIndex64 + (uCount32 - 1U));

							alloc_info *pstInfo = memory_align<alloc_info>(pcBuffer + uGRANULARITY * uIndex + sizeof(alloc_info), uAlign) - 1;
							{
								_mm_prefetch(reinterpret_cast<char const*>(pstInfo), _MM_HINT_T0);

								long volatile * const plBitLockBegin = m_alBitLock + uIndex32;
								long volatile * const plBitLockEnd = m_alBitLock + uIndex32 + uCount32;

								unsigned const uXBEGIN = _xbegin();

								if (uXBEGIN != _XABORT_EXPLICIT &&
									std::all_of(plBitLockBegin, plBitLockEnd, [&](long volatile & lBitLock)->bool
								{
									if (_xtest())
									{
										if (lBitLock == 0L)
										{
											lBitLock = ~0L;
											return true;
										}
										else
										{
											_xabort(0);
											return false;
										}
									}
									else
									{
										if (_InterlockedCompareExchange(&lBitLock, ~0L, 0L) == 0L)
										{
											return true;
										}
										else
										{
											std::fill(plBitLockBegin, &lBitLock, 0L);
											return false;
										}
									}
								}))
								{
									if (uXBEGIN == _XBEGIN_STARTED)
									{
										_xend();
									}
									pstInfo->pHeap = this;
									pstInfo->uIndex = uIndex;
									pstInfo->uCount = uCount32 * 32U;
									return pstInfo + 1;
								}
							}
						}
					}
				}
				return nullptr;
			}
			template <>
			void * malloc_by_count<__m256i, 8>(size_t uCount8, size_t uAlign)
			{
				unsigned const uAlignMask8LSB = g_auAlignMask[uCount8];

				__m256i const uw16AlignMask8LSB = _mm256_set1_epi32(uAlignMask8LSB);
				__m256i const uw16AlignMask8MSB = _mm256_set1_epi32(uAlignMask8LSB << (uCount8 - 1U));

				__m256i ymmOldLock8;
				{
					static_assert(u256X <= 8, "u256X should not exceed 8 by design (ymmOldLock8)");

					memory_transform(m_aymmBitLock, ymmOldLock8.m256i_i32, [](__m256i const & ymmBitLock)->int
					{
						return _mm256_movemask_epi8(_mm256_cmpeq_epi8(ymmBitLock, _mm256_set1_epi8(0xFF)));
					});
				}
				__m256i ymmHitLock8MSB;
				__m256i ymmHitLock8LSB;
				{
					__m256i const uw16OldLock0 = ymmOldLock8;

					__m256i const uw16XorLock1 = _mm256_xor_si256(uw16OldLock0, uw16AlignMask8LSB);
					__m256i const uw16AvgLock1 = _mm256_avg_epu16(uw16OldLock0, uw16AlignMask8LSB);

					__m256i const uw16AvgLock2 = _mm256_avg_epu16(uw16XorLock1, _mm256_setzero_si256());

					__m256i const uw16XorLock3 = _mm256_xor_si256(uw16AvgLock1, uw16AvgLock2);

					__m256i const uw16AndLock4 = _mm256_and_si256(uw16XorLock3, uw16AlignMask8MSB);

					ymmHitLock8MSB = uw16AndLock4;
					ymmHitLock8LSB = _mm256_srl_epi16(uw16AndLock4, _mm_cvtsi32_si128(uCount8 - 1));
				}

				__m256i const ymmCmpLock = _mm256_cmpeq_epi8(ymmHitLock8MSB, _mm256_setzero_si256());

				int const nCmpLock = _mm256_movemask_epi8(ymmCmpLock);

				if (~nCmpLock != 0)
				{
					for (unsigned uIndex512 = 0; uIndex512 + uIndex512 < u256X; uIndex512++) // 512 = 8 * 64
					{
						unsigned long long uxlHitLock8LSB = _mm_cvtsi128_si64(_mm256_castsi256_si128(ymmHitLock8LSB));
						unsigned long long uxlHitLock8MSB = _mm_cvtsi128_si64(_mm256_castsi256_si128(ymmHitLock8MSB));

						ymmHitLock8LSB = _mm256_permute4x64_epi64(ymmHitLock8LSB, _MM_SHUFFLE(0, 3, 2, 1));
						ymmHitLock8MSB = _mm256_permute4x64_epi64(ymmHitLock8MSB, _MM_SHUFFLE(0, 3, 2, 1));

						__m128i const xmmPattern = _mm_alignr_epi8(_mm_setzero_si128(), _mm_set1_epi32(~0), 16U - uCount8);
						unsigned const uPattern = ~0U + (1U << uCount8);

						for (unsigned uIndex8 = _tzcnt_u64(uxlHitLock8LSB); uIndex8 < 64; uIndex8 = _tzcnt_u64(uxlHitLock8LSB))
						{
							unsigned const uIndex = uIndex512 * 512U + uIndex8 * 8U;
							unsigned const uIndex128 = uIndex / 128U;

							_bittestandreset(reinterpret_cast<long*>(&uxlHitLock8LSB), uIndex8);
							_bittestandreset(reinterpret_cast<long*>(&uxlHitLock8MSB), uIndex8 + (uCount8 - 1U));

							alloc_info *pstInfo = memory_align<alloc_info>(pcBuffer + uGRANULARITY * uIndex + sizeof(alloc_info), uAlign) - 1;
							{
								_mm_prefetch(reinterpret_cast<char const*>(pstInfo), _MM_HINT_T0);

								__m128i const xmmOldLock8 = m_axmmBitLock[uIndex128];
								__m128i const xmmPattern8 = _mm_alignr_epi8(xmmPattern, _mm_setzero_si128(), 16U - uIndex8 % 16);
								unsigned const uPattern8 = uPattern << uIndex8;

								__m128i const xmmNewLock8 = _mm_andnot_si128(xmmPattern8, xmmOldLock8);
								__m128i const xmmCmpLock8 = _mm_cmpeq_epi32(xmmPattern8, xmmOldLock8);

								int const nCmpLock8 = _mm_movemask_epi8(xmmCmpLock8);

								if ((nCmpLock8 & uPattern8) == uPattern8)
								{
									if (_InterlockedCompareExchange128(
										&m_axlBitLock[uIndex128 * 2],
										_mm_extract_epi64(xmmNewLock8, 1),
										_mm_extract_epi64(xmmNewLock8, 0),
										xmmOldLock8.m128i_i64))
									{
										pstInfo->pHeap = this;
										pstInfo->uIndex = uIndex;
										pstInfo->uCount = uCount8 * 8;
										return pstInfo + 1;
									}
								}
							}
						}
					}
				}
				return nullptr;
			}
			void * malloc(size_t uSize, size_t uAlign = 4)
			{
				_mm_prefetch(reinterpret_cast<char const*>(g_auAlignMask), _MM_HINT_T0);
				_mm_prefetch(reinterpret_cast<char const*>(m_aymmBitLock), _MM_HINT_T0);

				size_t const uActualSize = sizeof(alloc_info) + uSize + uAlign - 1;

				size_t const uCount1 = (uActualSize + uGRANULARITY - 1) / uGRANULARITY;
				size_t const uCount8 = (uActualSize + uGRANULARITY * 8 - 1) / (uGRANULARITY * 8);
				size_t const uCount32 = (uActualSize + uGRANULARITY * 32 - 1) / (uGRANULARITY * 32);

				if (uSize == 0)
				{
					return nullptr;
				}
				else if (uCount1 <= 16)
				{
					return malloc_by_count<__m256i, 1>(uCount1, uAlign);
				}
				else if (uCount8 <= 16)
				{
					return malloc_by_count<__m256i, 8>(uCount8, uAlign);
				}
				else if (uCount32 <= 16)
				{
					return malloc_by_count<__m256i, 32>(uCount32, uAlign);
				}
				else
				{
					return nullptr;
				}
			}
			void * calloc(size_t uSize, size_t uN, size_t uAlign = 4)
			{
				void *pMem = malloc(uSize * uN, uAlign);
				{
					memset(pMem, 0, uSize * uN);
				}
				return pMem;
			}
			void * realloc(void *pMem, size_t uSize, size_t uAlign = 4)
			{
				if (pMem == nullptr)
				{
					return malloc(uSize, uAlign);
				}
				if (uSize == 0)
				{
					return free(pMem) ? nullptr : pMem;
				}

				_mm_prefetch(m_acBitLock, _MM_HINT_T0);

				alloc_info *pstInfo = reinterpret_cast<alloc_info*>(pMem) - 1;
				{
					if (pstInfo->pHeap != this ||
						pstInfo->uIndex > 256U * u256X ||
						pstInfo->uIndex + pstInfo->uCount > 256U * u256X)
					{
						__debugbreak();
						return nullptr;
					}
				}

				// TODO
				return nullptr;
			}
			bool free(void *pMem)
			{
				if (pMem == nullptr)
				{
					return true;
				}

				_mm_prefetch(m_acBitLock, _MM_HINT_T0);

				alloc_info *pstInfo = reinterpret_cast<alloc_info*>(pMem) - 1;
				{
					if (pstInfo->pHeap != this ||
						pstInfo->uIndex > 256U * u256X ||
						pstInfo->uIndex + pstInfo->uCount > 256U * u256X)
					{
						__debugbreak();
						return false;
					}
				}

				unsigned const uIndex32 = pstInfo->uIndex / 32U;
				unsigned const uIndex1 = pstInfo->uIndex % 32U;
				unsigned const uCount32 = pstInfo->uCount / 32U;
				unsigned const uCount1 = pstInfo->uCount % 32U;
				unsigned const uPattern1 = ((~0U) + (1U << uCount1)) << uIndex1;
				_mm_clflush(pstInfo);

				unsigned const uPattern1 = uPattern << uIndex1;

				long volatile & lBitLock = m_alBitLock[uIndex32];

				if (uCount32)
				{
					_mm_sfence();
					std::fill_n(m_alBitLock + uIndex32, m_alBitLock + uIndex32 + uCount32, 0L);
				}

				if (uCount1)
				{
					if (_InterlockedXor(&m_alBitLock[uIndex32], uPattern1) & uPattern1)
					{
						__debugbreak();
					}
				}
				return true;
			}
		};
	}
	struct alignas(128) memory_manager
	{
		void * operator new (size_t uSize)
		{
			return VirtualAlloc(NULL, uSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		}
		void operator delete (void *pThis)
		{
			if (VirtualFree(pThis, 0, MEM_RELEASE) == FALSE)
			{
				__debugbreak();
			}
		}

		enum { LEVEL0_GRANULARITY = 64 }; // 64B -- cache-line size
		enum { LEVEL0_CAPACITY = 256 };
		enum { LEVEL1_GRANULARITY = LEVEL0_GRANULARITY * LEVEL0_CAPACITY }; // 16KB -- size of L1 cache
		enum { LEVEL1_CAPACITY = 256 };
		enum { LEVEL2_GRANULARITY = LEVEL1_GRANULARITY * LEVEL1_CAPACITY }; // 4MB -- size of LLC
		enum { LEVEL2_CAPACITY = 256 };
		enum { LEVEL3_GRANULARITY = LEVEL2_GRANULARITY * LEVEL2_CAPACITY }; // 1GB
		enum { LEVEL3_CAPACITY = 1 };

		using memory_heap_level0 = memory_heap<LEVEL0_GRANULARITY, LEVEL0_CAPACITY>;
		using memory_heap_level1 = memory_heap<LEVEL1_GRANULARITY, LEVEL1_CAPACITY>;
		using memory_heap_level2 = memory_heap<LEVEL2_GRANULARITY, LEVEL2_CAPACITY>;
		using memory_heap_level3 = memory_heap<LEVEL3_GRANULARITY, LEVEL3_CAPACITY>;

		using data_cache_level1 = memory_heap<64, 512>; // 64B * 512 = 32KB = 64B * 64sets * 8ways
		using data_cache_level2 = memory_heap<1024, 256>; // 1024B * 256 = 256KB = 64B * 1024sets * 4ways
		using data_cache_level3 = memory_heap<16384, 192>; // 16384B * 192 = 3MB = 64B * 4096sets * 12ways
		using data_cache_level4 = memory_heap<262144, 128>; // 

		//

		void * const m_pBuffer;

		memory_heap_level2 *m_apHeap2[LEVEL3_CAPACITY];
		memory_heap_level1 *m_apHeap1[LEVEL3_CAPACITY * LEVEL2_CAPACITY];
		memory_heap_level0 *m_apHeap0[LEVEL3_CAPACITY * LEVEL2_CAPACITY * LEVEL1_CAPACITY];

		//

		memory_heap_level2 m_aHeap[LEVEL3_CAPACITY];
		memory_heap_level1 m_aaHeap[LEVEL3_CAPACITY][LEVEL2_CAPACITY];
		memory_heap_level0 m_aaaHeap[LEVEL3_CAPACITY][LEVEL2_CAPACITY][LEVEL1_CAPACITY];

		memory_manager()
			: m_pBuffer(VirtualAlloc(NULL, LEVEL3_GRANULARITY * LEVEL3_CAPACITY, MEM_RESERVE, PAGE_NOACCESS))
		{
			if (m_pBuffer == NULL)
			{
				__debugbreak();
			}

			std::fill_n(m_apHeap2, std::size(m_apHeap2), nullptr);
			std::fill_n(m_apHeap1, std::size(m_apHeap1), nullptr);
			std::fill_n(m_apHeap0, std::size(m_apHeap0), nullptr);

			//

			char *pcBuffer = reinterpret_cast<char*>(m_pBuffer);

			for (unsigned uIndex3 = 0; uIndex3 < LEVEL3_CAPACITY; uIndex3++)
			{
				new (&m_aHeap[uIndex3]) memory_heap_level2(pcBuffer, LEVEL2_CAPACITY);

				for (unsigned uIndex2 = 0; uIndex2 < LEVEL2_CAPACITY; uIndex2++)
				{
					new (&m_aaHeap[uIndex3][uIndex2]) memory_heap_level1(pcBuffer, LEVEL1_CAPACITY);

					for (unsigned uIndex1 = 0; uIndex1 < LEVEL1_CAPACITY; uIndex1++)
					{
						new (&m_aaaHeap[uIndex3][uIndex2][uIndex1]) memory_heap_level0(pcBuffer, LEVEL0_CAPACITY);

						pcBuffer += LEVEL0_GRANULARITY * LEVEL0_CAPACITY;
					}
				}
			}
		}
		~memory_manager()
		{
			if (m_pBuffer)
			{
				if (VirtualFree(m_pBuffer, 0, MEM_RELEASE) == FALSE)
				{
					__debugbreak();
				}
			}
		}

		bool push_stack()
		{
			// TODO
			return false;
		}
		void * alloca(size_t uSize, size_t uAlign)
		{
			// TODO
			return nullptr;
		}
		bool pop_stack()
		{
			// TODO
			return false;
		}

		void * malloc(size_t uSize, size_t uAlign)
		{
			size_t const uCount0 = (uSize + uAlign - 1 + LEVEL0_GRANULARITY - 1) / LEVEL0_GRANULARITY;
			size_t const uCount1 = (uSize + uAlign - 1 + LEVEL1_GRANULARITY - 1) / LEVEL1_GRANULARITY;
			size_t const uCount2 = (uSize + uAlign - 1 + LEVEL2_GRANULARITY - 1) / LEVEL2_GRANULARITY;

			// TODO
			return nullptr;
		}
		void * calloc(size_t uSize, size_t uN, size_t uAlign)
		{
			// TODO
			return nullptr;
		}
		void * realloc(void *pMem, size_t uSize, size_t uAlign)
		{
			// TODO
			return nullptr;
		}
		bool free(void *pMem)
		{
			// TODO
			return false;
		}
	};
}