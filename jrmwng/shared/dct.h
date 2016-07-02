#pragma once

/* Author: jrmwng @ 2016 */

#include <intrin.h>
#include <math.h>
#include <utility>
#include <numeric>

namespace jrmwng
{
	namespace
	{
		template <typename Tinteger, Tinteger... tInteger, typename Tfunc>
		void dct_for_each(std::integer_sequence<Tinteger, tInteger...>, Tfunc && tFunc)
		{
			using type = int [];
			(void) type {
				(std::forward<Tfunc>(tFunc)(std::integral_constant<Tinteger, tInteger>()), 0)...
			};
		}
		double dct_cos(double lr)
		{
			return cos(lr);
		}
		float dct_cos(float r)
		{
			return cosf(r);
		}
		template <typename T>
		T dct_pi();
		template <>
		double dct_pi<double>()
		{
			return atan(1);
		}
		template <>
		float dct_pi<float>()
		{
			return atanf(1.0F);
		}
		template <typename T, size_t uN, typename Tq, typename Tn>
		T dct_coefficient(Tq tQ, Tn tN)
		{
			return dct_cos((dct_pi<T>() * T(4) * (T(2) * T(tN) + T(1)) * T(tQ)) / (T(2) * T(uN)));
		}
	}
	template <typename T, size_t uN>
	struct dct_matrix
	{
		T m_aat[uN][uN];

		auto const & operator [] (size_t uIndex) const
		{
			return m_aat[uIndex];
		}

		dct_matrix()
		{
			dct_for_each(std::make_index_sequence<uN>(), [&](auto const tN)
			{
				dct_for_each(std::make_index_sequence<uN>(), [&, tN](auto const tQ)
				{
					m_aat[tN][tQ] = dct_coefficient<T, uN>(tQ, tN);
				});
			});
		}

		static dct_matrix g_Instance;
	};
	template <typename T, size_t uN>
	dct_matrix<T, uN> dct_matrix<T, uN>::g_Instance;

	// X[k] = SUM[n=0:N-1]( x[n] * cos((PI/N) * (n + 1/2) * k) )
	template <typename T, size_t uN>
	void dct(T const (&atInput)[uN], T(&atOutput)[uN])
	{
		auto const & dctMatrix = dct_matrix<T, uN>::g_Instance;

		dct_for_each(std::make_index_sequence<uN>(), [&](auto const k)
		{
			T tAcc(0);
			{
				dct_for_each(std::make_index_sequence<uN>(), [&, k](auto const n)
				{
					tAcc += atInput[n] * dctMatrix[n][k];
				});
			}
			atOutput[k] = tAcc;
		});
	}
	template <typename T, size_t uM, size_t uN>
	void dct(T const (&aatInput)[uM][uN], T(&aatOutput)[uM][uN])
	{
		auto const & dctMatrixM = dct_matrix<T, uM>::g_Instance;
		auto const & dctMatrixN = dct_matrix<T, uN>::g_Instance;

		for (unsigned k = 0; k < uN; k++)
		{
			T atAcc1[uM];
			{
				dct_for_each(std::make_index_sequence<uM>(), [&](auto const m)
				{
					T tAcc1(0);
					{
						dct_for_each(std::make_index_sequence<uN>(), [&](auto const n)
						{
							tAcc1 += aatInput[m][n] * dctMatrixN[n][k];
						});
					}
					atAcc1[m] = tAcc1;
				});
			}

			dct_for_each(std::make_index_sequence<uM>(), [&](auto const j)
			{
				T tAcc0(0);
				{
					dct_for_each(std::make_index_sequence<uM>(), [&](auto const m)
					{
						tAcc0 += atAcc1[m] * dctMatrixM[m][j];
					});
				}
				aatOutput[j][k] = tAcc0;
			});
		}
	}
	template <size_t uN4>
	void dct(__m256d const (&alr4Input)[uN4], __m256d(&alr4Output)[uN4])
	{
		auto const & dctMatrix = dct_matrix<double, (uN4 * 4)>::g_Instance;

		dct_for_each(std::make_index_sequence<uN4>(), [&](auto const k4)
		{
			__m256d lr4Acc = _mm256_setzero_pd();
			{
				dct_for_each(std::make_index_sequence<uN4>(), [&](auto const n4)
				{
					__m256d const lr4InputABCD = alr4Input[n4];

					dct_for_each(std::make_index_sequence<4>(), [&](auto const n1)
					{
						__m256d const lr4Input = _mm256_permute4x64_pd(lr4InputABCD, _MM_SHUFFLE(n1.value, n1.value, n1.value, n1.value));
						__m256d const lr4Coeff = _mm256_load_pd(dctMatrix[n4 * 4 + n1] + k4 * 4);
						__m256d const lr4Next = _mm256_fmadd_pd(lr4Input, lr4Coeff, lr4Acc);

						lr4Acc = lr4Next;
					});
				});
			}
			alr4Output[k4] = lr4Acc;
		});
	}
	template <size_t uN8>
	void dct(__m256 const (&ar8Input)[uN8], __m256(&ar8Output)[uN8])
	{
		auto const & dctMatrix = dct_matrix<float, (uN8 * 8)>::g_Instance;

		dct_for_each(std::make_index_sequence<uN8>(), [&](auto const k8)
		{
			__m256 r8Acc = _mm256_setzero_ps();
			{
				dct_for_each(std::make_index_sequence<uN8>(), [&](auto const n8)
				{
					__m256 const r8InputABCDEFGH = ar8Input[n8];

					dct_for_each(std::make_integer_sequence<unsigned, 8>(), [&](auto const n1)
					{
						__m256 const r8Input = _mm256_permutevar8x32_ps(r8InputABCDEFGH, _mm256_set1_epi32(n1));
						__m256 const r8Coeff = _mm256_load_ps(dctMatrix[n8 * 8 + n1] + k8 * 8);
						__m256 const r8Next = _mm256_fmadd_ps(r8Input, r8Coeff, r8Acc);

						r8Acc = r8Next;
					});
				});
			}
			ar8Output[k8] = r8Acc;
		});
	}
	template <size_t uN2>
	void dct(__m128d const (&alr2Input)[uN2], __m128d(&alr2Output)[uN2])
	{
		auto const & dctMatrix = dct_matrix<double, (uN2 * 2)>::g_Instance;

		dct_for_each(std::make_index_sequence<uN2>(), [&](auto const k2)
		{
			__m128d lr2Acc = _mm_setzero_pd();
			{
				dct_for_each(std::make_index_sequence<uN2>(), [&](auto const n2)
				{
					__m128d const lr2InputAB = alr2Input[n2];

					dct_for_each(std::make_index_sequence<2>(), [&](auto const n1)
					{
						__m128d const lr2Input = _mm_shuffle_pd(lr2InputAB, lr2InputAB, _MM_SHUFFLE2(n1.value, n1.value));
						__m128d const lr2Coeff = _mm_load_pd(dctMatrix[n2 * 2 + n1] + k2 * 2);
						__m128d const lr2Next = _mm_fmadd_pd(lr2Input, lr2Coeff, lr2Acc);

						lr2Acc = lr2Next;
					});
				});
			}
			alr2Output[k2] = lr2Acc;
		});
	}
	template <size_t uN4>
	void dct(__m128 const (&ar4Input)[uN4], __m128(&ar4Output)[uN4])
	{
		auto const & dctMatrix = dct_matrix<float, (uN4 * 4)>::g_Instance;

		dct_for_each(std::make_index_sequence<uN4>(), [&](auto const k4)
		{
			__m128 r4Acc = _mm_setzero_ps();
			{
				dct_for_each(std::make_index_sequence<uN4>(), [&](auto const n4)
				{
					__m128 const r4InputABCD = ar4Input[n4];

					dct_for_each(std::make_index_sequence<4>(), [&](auto const n1)
					{
						__m128 const r4Input = _mm_shuffle_ps(r4InputABCD, r4InputABCD, _MM_SHUFFLE(n1.value, n1.value, n1.value, n1.value));
						__m128 const r4Coeff = _mm_load_ps(dctMatrix[n4 * 4 + n1] + k4 * 4);
						__m128 const r4Next = _mm_fmadd_ps(r4Input, r4Coeff, r4Acc);

						r4Acc = r4Next;
					});
				});
			}
			ar4Output[k4] = r4Acc;
		});
	}
}