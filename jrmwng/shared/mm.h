#pragma once

/* Author: jrmwng @ 2016 */

#include <intrin.h>
#include <tuple>
#include <functional>
#include <type_traits>

namespace jrmwng
{
	namespace mm
	{
		template <typename Tmm, typename T>
		struct Mtraits;
		template <typename T>
		struct Mtraits<T, T>
		{
			static T setzero()
			{
				return T(0);
			}
			static T set1(T t)
			{
				return t;
			}
			static T plus(T const & tA, T const & tB)
			{
				return tA + tB;
			}
			static T minus(T const & tA, T const & tB)
			{
				return tA - tB;
			}
			static T multiplies(T const & tA, T const & tB)
			{
				return tA * tB;
			}
			static T divides(T const & tA, T const & tB)
			{
				return tA / tB;
			}
		};
		template <>
		struct Mtraits<__m256d, double>
		{
			static __m256d setzero()
			{
				return _mm256_setzero_pd();
			}
			static __m256d set1(double lr)
			{
				return _mm256_set1_pd(lr);
			}
			static __m256d plus(__m256d const & lr4A, __m256d const & lr4B)
			{
				return _mm256_add_pd(lr4A, lr4B);
			}
			static __m256d minus(__m256d const & lr4A, __m256d const & lr4B)
			{
				return _mm256_sub_pd(lr4A, lr4B);
			}
			static __m256d multiplies(__m256d const & lr4A, __m256d const & lr4B)
			{
				return _mm256_mul_pd(lr4A, lr4B);
			}
			static __m256d divides(__m256d const & lr4A, __m256d const & lr4B)
			{
				return _mm256_div_pd(lr4A, lr4B);
			}
		};
		template <>
		struct Mtraits<__m256, float>
		{
			static __m256 setzero()
			{
				return _mm256_setzero_ps();
			}
			static __m256 set1(float r)
			{
				return _mm256_set1_ps(r);
			}
			static __m256 set1(int n)
			{
				return _mm256_cvtepi32_ps(_mm256_set1_epi32(n));
			}
			static __m256 plus(__m256 const & r8A, __m256 const & r8B)
			{
				return _mm256_add_ps(r8A, r8B);
			}
			static __m256 minus(__m256 const & r8A, __m256 const & r8B)
			{
				return _mm256_sub_ps(r8A, r8B);
			}
			static __m256 multiplies(__m256 const & r8A, __m256 const & r8B)
			{
				return _mm256_mul_ps(r8A, r8B);
			}
			static __m256 divides(__m256 const & r8A, __m256 const & r8B)
			{
				return _mm256_div_ps(r8A, r8B);
			}
		};
		template <>
		struct Mtraits<__m256i, int>
		{
			static __m256i setzero()
			{
				return _mm256_setzero_si256();
			}
			static __m256i set1(int n)
			{
				return _mm256_set1_epi32(n);
			}
			static __m256i plus(__m256i const & n8A, __m256i const & n8B)
			{
				return _mm256_add_epi32(n8A, n8B);
			}
			static __m256i minus(__m256i const & n8A, __m256i const & n8B)
			{
				return _mm256_sub_epi32(n8A, n8B);
			}
			static __m256i multiplies(__m256i const & n8A, __m256i const & n8B)
			{
				return _mm256_mullo_epi32(n8A, n8B);
			}
			static __m256i divides(__m256i const & n8A, __m256i const & n8B)
			{
				__m256d const lr4AL = _mm256_cvtepi32_pd(_mm256_extracti128_si256(n8A, 0));
				__m256d const lr4AH = _mm256_cvtepi32_pd(_mm256_extracti128_si256(n8A, 1));
				__m256d const lr4BL = _mm256_cvtepi32_pd(_mm256_extracti128_si256(n8B, 0));
				__m256d const lr4BH = _mm256_cvtepi32_pd(_mm256_extracti128_si256(n8B, 1));
				__m256d const lr4AdivBL = _mm256_div_pd(lr4AL, lr4BL);
				__m256d const lr4AdivBH = _mm256_div_pd(lr4AH, lr4BH);
				__m128i const n4AdivBL = _mm256_cvtpd_epi32(lr4AdivBL);
				__m128i const n4AdivBH = _mm256_cvtpd_epi32(lr4AdivBH);
				return _mm256_set_m128i(n4AdivBH, n4AdivBL);
			}
		};
		template <>
		struct Mtraits<__m256i, short>
		{
			static __m256i setzero()
			{
				return _mm256_setzero_si256();
			}
			static __m256i set1(short w)
			{
				return _mm256_set1_epi16(w);
			}
			static __m256i plus(__m256i const & w16A, __m256i const & w16B)
			{
				return _mm256_add_epi16(w16A, w16B);
			}
			static __m256i minus(__m256i const & w16A, __m256i const & w16B)
			{
				return _mm256_sub_epi16(w16A, w16B);
			}
			static __m256i multiplies(__m256i const & w16A, __m256i const & w16B)
			{
				return _mm256_mullo_epi16(w16A, w16B);
			}
			static __m256i divides(__m256i const & w16A, __m256i const & w16B)
			{
				__m128i const w8AL = _mm256_extracti128_si256(w16A, 0);
				__m128i const w8AH = _mm256_extracti128_si256(w16A, 1);
				__m128i const w8BL = _mm256_extracti128_si256(w16B, 0);
				__m128i const w8BH = _mm256_extracti128_si256(w16B, 1);
				__m256i const n8AL = _mm256_cvtepi16_epi32(w8AL);
				__m256i const n8AH = _mm256_cvtepi16_epi32(w8AH);
				__m256i const n8BL = _mm256_cvtepi16_epi32(w8BL);
				__m256i const n8BH = _mm256_cvtepi16_epi32(w8BH);
				__m256 const r8AL = _mm256_cvtepi32_ps(n8AL);
				__m256 const r8AH = _mm256_cvtepi32_ps(n8AH);
				__m256 const r8BL = _mm256_cvtepi32_ps(n8BL);
				__m256 const r8BH = _mm256_cvtepi32_ps(n8BH);
				__m256 const r8ALdivBL = _mm256_div_ps(r8AL, r8BL);
				__m256 const r8AHdivBH = _mm256_div_ps(r8AH, r8BH);
				__m256i const n4ALdivBL = _mm256_cvtps_epi32(r8ALdivBL);
				__m256i const n4AHdivBH = _mm256_cvtps_epi32(r8AHdivBH);
				return _mm256_packs_epi32(n4ALdivBL, n4AHdivBH);
			}
		};
		template <>
		struct Mtraits<__m128d, double>
		{
			static __m128d setzero()
			{
				return _mm_setzero_pd();
			}
			static __m128d set1(double lr)
			{
				return _mm_set1_pd(lr);
			}
			static __m128d plus(__m128d const & lr2A, __m128d const & lr2B)
			{
				return _mm_add_pd(lr2A, lr2B);
			}
			static __m128d minus(__m128d const & lr2A, __m128d const & lr2B)
			{
				return _mm_sub_pd(lr2A, lr2B);
			}
			static __m128d multiplies(__m128d const & lr2A, __m128d const & lr2B)
			{
				return _mm_mul_pd(lr2A, lr2B);
			}
			static __m128d divides(__m128d const & lr2A, __m128d const & lr2B)
			{
				return _mm_div_pd(lr2A, lr2B);
			}
		};
		template <>
		struct Mtraits<__m128, float>
		{
			static __m128 setzero()
			{
				return _mm_setzero_ps();
			}
			static __m128 set1(float r)
			{
				return _mm_set1_ps(r);
			}
			static __m128 set1(int n)
			{
				return _mm_cvtepi32_ps(_mm_set1_epi32(n));
			}
			static __m128 plus(__m128 const & r4A, __m128 const & r4B)
			{
				return _mm_add_ps(r4A, r4B);
			}
			static __m128 minus(__m128 const & r4A, __m128 const & r4B)
			{
				return _mm_sub_ps(r4A, r4B);
			}
			static __m128 multiplies(__m128 const & r4A, __m128 const & r4B)
			{
				return _mm_mul_ps(r4A, r4B);
			}
			static __m128 divides(__m128 const & r4A, __m128 const & r4B)
			{
				return _mm_div_ps(r4A, r4B);
			}
		};
		template <>
		struct Mtraits<__m128i, int>
		{
			static __m128i setzero()
			{
				return _mm_setzero_si128();
			}
			static __m128i set1(int n)
			{
				return _mm_set1_epi32(n);
			}
			static __m128i plus(__m128i const & n4A, __m128i const & n4B)
			{
				return _mm_add_epi32(n4A, n4B);
			}
			static __m128i minus(__m128i const & n4A, __m128i const & n4B)
			{
				return _mm_sub_epi32(n4A, n4B);
			}
			static __m128i multiplies(__m128i const & n4A, __m128i const & n4B)
			{
				return _mm_mullo_epi32(n4A, n4B);
			}
			static __m128i divides(__m128i const & n4A, __m128i const & n4B)
			{
				__m128 const r4A = _mm_cvtepi32_ps(n4A);
				__m128 const r4B = _mm_cvtepi32_ps(n4B);
				__m128 const r4AdivB = _mm_div_ps(r4A, r4B);
				return _mm_cvtps_epi32(r4AdivB);
			}
		};
		template <>
		struct Mtraits<__m128i, short>
		{
			static __m128i setzero()
			{
				return _mm_setzero_si128();
			}
			static __m128i set1(short w)
			{
				return _mm_set1_epi16(w);
			}
			static __m128i plus(__m128i const & w8A, __m128i const & w8B)
			{
				return _mm_add_epi16(w8A, w8B);
			}
			static __m128i minus(__m128i const & w8A, __m128i const & w8B)
			{
				return _mm_sub_epi16(w8A, w8B);
			}
			static __m128i multiplies(__m128i const & w8A, __m128i const & w8B)
			{
				return _mm_mullo_epi16(w8A, w8B);
			}
			static __m128i divides(__m128i const & w8A, __m128i const & w8B)
			{
				__m128i const n4AL = _mm_cvtepi16_epi32(w8A);
				__m128i const n4AH = _mm_shuffle_epi32(w8A, _MM_SHUFFLE(1, 0, 3, 2));
				__m128i const n4BL = _mm_cvtepi16_epi32(w8B);
				__m128i const n4BH = _mm_shuffle_epi32(w8B, _MM_SHUFFLE(1, 0, 3, 2));
				__m128 const r4AL = _mm_cvtepi32_ps(n4AL);
				__m128 const r4AH = _mm_cvtepi32_ps(n4AH);
				__m128 const r4BL = _mm_cvtepi32_ps(n4BL);
				__m128 const r4BH = _mm_cvtepi32_ps(n4BH);
				__m128 const r4ALdivBL = _mm_div_ps(r4AL, r4BL);
				__m128 const r4AHdivBH = _mm_div_ps(r4AH, r4BH);
				__m128i const n4ALdivBL = _mm_cvtps_epi32(r4ALdivBL);
				__m128i const n4AHdivBH = _mm_cvtps_epi32(r4AHdivBH);
				return _mm_packs_epi32(n4ALdivBL, n4AHdivBH);
			}
		};

		//

		struct Mexpr
		{};

		//template <typename Tmm, typename T>
		//struct Mzero
		//	: Mexpr
		//{
		//	operator Tmm () const
		//	{
		//		return Mtraits<Tmm, T>::setzero();
		//	}
		//};
		//template <typename Tmm, typename T>
		//struct Mone
		//	: Mexpr
		//{
		//	operator Tmm () const
		//	{
		//		return Mtraits<Tmm, T>::set1(1);
		//	}
		//};

		template <template <typename T> class Top, typename... Targs>
		struct Mop
			: Mexpr
		{
			using type = Mop<Top, Targs...>;

			std::tuple<Targs...> m_Args;

			Mop(Targs const &... tArgs)
				: m_Args(tArgs...)
			{}
		};
		template <size_t uIndex, template <typename T> class Top, typename... Targs>
		auto get(Mop<Top, Targs...> const & tOp)
		{
			return std::get<uIndex>(tOp.m_Args);
		}

		//template <typename T>
		//auto operator - (std::integral_constant<T, 0> const & tZero)
		//{
		//	return std::integral_constant<T, 0>();
		//}
		template <typename T1, typename T2>
		auto operator - (Mop<std::minus, T1, T2> const & tOp)
		{
			return Mop<std::minus, T2, T1>(get<1>(tOp), get<0>(tOp));
		}
		template <typename T1>
		auto operator - (Mop<std::negate, T1> const & t1)
		{
			return get<0>(t1);
		}
		template <typename T1, typename Tenable = std::enable_if_t<(std::is_base_of<Mexpr, T1>::value)>>
		auto operator - (T1 const & t1)
		{
			return Mop<std::negate, T1>(t1);
		}

		//template <typename T>
		//auto operator + (std::integral_constant<T, 0> const & t1, std::integral_constant<T, 0> const & t2)
		//{
		//	return std::integral_constant<T, 0>();
		//}
		template <typename T, typename T1>
		auto operator + (Mop<std::negate, T1> const & t1, std::integral_constant<T, 0> const & t2)
		{
			return t1;
		}
		template <typename T, typename T2>
		auto operator + (std::integral_constant<T, 0> const & t1, Mop<std::negate, T2> const & t2)
		{
			return t2;
		}
		template <typename T, typename T1, typename Tenable = std::enable_if_t<(std::is_base_of<Mexpr, T1>::value)>>
		auto operator + (T1 const & t1, std::integral_constant<T, 0> const & t2)
		{
			return t1;
		}
		template <typename T, typename T2, typename Tenable = std::enable_if_t<(std::is_base_of<Mexpr, T2>::value)>>
		auto operator + (std::integral_constant<T, 0> const & t1, T2 const & t2)
		{
			return t2;
		}
		template <typename T1, typename T2>
		auto operator + (Mop<std::negate, T1> const & t1, Mop<std::negate, T2> const & t2)
		{
			return -(Mop<std::plus, T1, T2>(get<0>(t1), get<0>(t2)));
		}
		template <typename T1, typename T2>
		auto operator + (Mop<std::negate, T1> const & t1, T2 const & t2)
		{
			return Mop<std::minus, T2, T1>(t2, get<0>(t1));
		}
		template <typename T1, typename T2>
		auto operator + (T1 const & t1, Mop<std::negate, T2> const & t2)
		{
			return Mop<std::minus, T1, T2>(t1, get<0>(t2));
		}
		template <typename T1, typename T2, typename Tenable = std::enable_if_t<(std::is_base_of<Mexpr, T1>::value || std::is_base_of<Mexpr, T2>::value)>>
		auto operator + (T1 const & t1, T2 const & t2)
		{
			return Mop<std::plus, T1, T2>(t1, t2);
		}

		//template <typename T>
		//auto operator - (std::integral_constant<T, 0> const & t1, std::integral_constant<T, 0> const & t2)
		//{
		//	return std::integral_constant<T, 0>();
		//}
		template <typename T, typename T1>
		auto operator - (Mop<std::negate, T1> const & t1, std::integral_constant<T, 0> const & t2)
		{
			return t1;
		}
		template <typename T, typename T2>
		auto operator - (std::integral_constant<T, 0> const & t1, Mop<std::negate, T2> const & t2)
		{
			return get<0>(t2);
		}
		template <typename T, typename T1, typename Tenable = std::enable_if_t<(std::is_base_of<Mexpr, T1>::value)>>
		auto operator - (T1 const & t1, std::integral_constant<T, 0> const & t2)
		{
			return t1;
		}
		template <typename T, typename T2, typename Tenable = std::enable_if_t<(std::is_base_of<Mexpr, T2>::value)>>
		auto operator - (std::integral_constant<T, 0> const & t1, T2 const & t2)
		{
			return -t2;
		}
		template <typename T1, typename T2>
		auto operator - (Mop<std::negate, T1> const & t1, Mop<std::negate, T2> const & t2)
		{
			return Mop<std::minus, T2, T1>(get<0>(t2), get<0>(t1));
		}
		template <typename T1, typename T2>
		auto operator - (Mop<std::negate, T1> const & t1, T2 const & t2)
		{
			return -(Mop<std::plus, T1, T2>(get<0>(t1), t2));
		}
		template <typename T1, typename T2>
		auto operator - (T1 const & t1, Mop<std::negate, T2> const & t2)
		{
			return Mop<std::plus, T1, T2>(t1, get<0>(t2));
		}
		template <typename T1, typename T2, typename Tenable = std::enable_if_t<(std::is_base_of<Mexpr, T1>::value || std::is_base_of<Mexpr, T2>::value)>>
		auto operator - (T1 const & t1, T2 const & t2)
		{
			return Mop<std::minus, T1, T2>(t1, t2);
		}

		//template <typename T>
		//auto operator * (std::integral_constant<T, 0> const & t1, std::integral_constant<T, 0> const & t2)
		//{
		//	return std::integral_constant<T, 0>();
		//}
		template <typename T, typename T1>
		auto operator * (Mop<std::negate, T1> const & t1, std::integral_constant<T, 0> const & t2)
		{
			return std::integral_constant<T, 0>();
		}
		template <typename T, typename T2>
		auto operator * (std::integral_constant<T, 0> const & t1, Mop<std::negate, T2> const & t2)
		{
			return std::integral_constant<T, 0>();
		}
		template <typename T, typename T1, typename Tenable = std::enable_if_t<(std::is_base_of<Mexpr, T1>::value)>>
		auto operator * (T1 const & t1, std::integral_constant<T, 0> const & t2)
		{
			return std::integral_constant<T, 0>();
		}
		template <typename T, typename T2, typename Tenable = std::enable_if_t<(std::is_base_of<Mexpr, T2>::value)>>
		auto operator * (std::integral_constant<T, 0> const & t1, T2 const & t2)
		{
			return std::integral_constant<T, 0>();
		}
		//template <typename T>
		//auto operator * (std::integral_constant<T, 1> const & t1, std::integral_constant<T, 1> const & t2)
		//{
		//	return std::integral_constant<T, 1>();
		//}
		template <typename T, typename T1>
		auto operator * (Mop<std::negate, T1> const & t1, std::integral_constant<T, 1> const & t2)
		{
			return t1;
		}
		template <typename T, typename T2>
		auto operator * (std::integral_constant<T, 1> const & t1, Mop<std::negate, T2> const & t2)
		{
			return t2;
		}
		template <typename T, typename T1, typename Tenable = std::enable_if_t<(std::is_base_of<Mexpr, T1>::value)>>
		auto operator * (T1 const & t1, std::integral_constant<T, 1> const & t2)
		{
			return t1;
		}
		template <typename T, typename T2, typename Tenable = std::enable_if_t<(std::is_base_of<Mexpr, T2>::value)>>
		auto operator * (std::integral_constant<T, 1> const & t1, T2 const & t2)
		{
			return t2;
		}
		//template <typename T>
		//auto operator * (std::integral_constant<T, 1> const & t1, std::integral_constant<T, 0> const & t2)
		//{
		//	return std::integral_constant<T, 0>();
		//}
		//template <typename T>
		//auto operator * (std::integral_constant<T, 0> const & t1, std::integral_constant<T, 1> const & t2)
		//{
		//	return std::integral_constant<T, 0>();
		//}
		template <typename T1, typename T2>
		auto operator * (Mop<std::negate, T1> const & t1, Mop<std::negate, T2> const & t2)
		{
			return Mop<std::multiplies, T1, T2>(get<0>(t1), get<0>(t2));
		}
		template <typename T1, typename T2>
		auto operator * (Mop<std::negate, T1> const & t1, T2 const & t2)
		{
			return -(Mop<std::multiplies, T1, T2>(get<0>(t1), t2));
		}
		template <typename T1, typename T2>
		auto operator * (T1 const & t1, Mop<std::negate, T2> const & t2)
		{
			return -(Mop<std::multiplies, T1, T2>(t1, get<0>(t2)));
		}
		template <typename T1, typename T2, typename Tenable = std::enable_if_t<(std::is_base_of<Mexpr, T1>::value || std::is_base_of<Mexpr, T2>::value)>>
		auto operator * (T1 const & t1, T2 const & t2)
		{
			return Mop<std::multiplies, T1, T2>(t1, t2);
		}

		//template <typename T>
		//auto operator / (std::integral_constant<T, 0> const & t1, std::integral_constant<T, 0> const & t2)
		//{
		//	return std::integral_constant<T, 0>();
		//}
		//template <typename T, typename T1>
		//auto operator / (Mop<std::negate, T1> const & t1, std::integral_constant<T, 0> const & t2)
		//{
		//	return t1;
		//}
		template <typename T, typename T2>
		auto operator / (std::integral_constant<T, 0> const & t1, Mop<std::negate, T2> const & t2)
		{
			return std::integral_constant<T, 0>();
		}
		//template <typename T, typename T1>
		//auto operator / (T1 const & t1, std::integral_constant<T, 0> const & t2)
		//{
		//	return t1;
		//}
		template <typename T, typename T2, typename Tenable = std::enable_if_t<(std::is_base_of<Mexpr, T2>::value)>>
		auto operator / (std::integral_constant<T, 0> const & t1, T2 const & t2)
		{
			return std::integral_constant<T, 0>();
		}
		//template <typename T>
		//auto operator / (std::integral_constant<T, 1> const & t1, std::integral_constant<T, 1> const & t2)
		//{
		//	return std::integral_constant<T, 1>();
		//}
		template <typename T, typename T1>
		auto operator / (Mop<std::negate, T1> const & t1, std::integral_constant<T, 1> const & t2)
		{
			return t1;
		}
		template <typename T, typename T2>
		auto operator / (std::integral_constant<T, 1> const & t1, Mop<std::negate, T2> const & t2)
		{
			return -(t1 / get<0>(t2));
		}
		template <typename T, typename T1, typename Tenable = std::enable_if_t<(std::is_base_of<Mexpr, T1>::value)>>
		auto operator / (T1 const & t1, std::integral_constant<T, 1> const & t2)
		{
			return t1;
		}
		template <typename T, typename T2, typename Tenable = std::enable_if_t<(std::is_base_of<Mexpr, T2>::value)>>
		auto operator / (std::integral_constant<T, 1> const & t1, T2 const & t2)
		{
			return t2;
		}
		template <typename T1, typename T2>
		auto operator / (Mop<std::negate, T1> const & t1, Mop<std::negate, T2> const & t2)
		{
			return Mop<std::divides, T1, T2>(get<0>(t1), get<0>(t2));
		}
		template <typename T1, typename T2>
		auto operator / (Mop<std::negate, T1> const & t1, T2 const & t2)
		{
			return -(Mop<std::divides, T1, T2>(get<0>(t1), t2));
		}
		template <typename T1, typename T2>
		auto operator / (T1 const & t1, Mop<std::negate, T2> const & t2)
		{
			return -(Mop<std::divides, T1, T2>(t1, get<0>(t2)));
		}
		template <typename T1, typename T2, typename Tenable = std::enable_if_t<(std::is_base_of<Mexpr, T1>::value || std::is_base_of<Mexpr, T2>::value)>>
		auto operator / (T1 const & t1, T2 const & t2)
		{
			return Mop<std::divides, T1, T2>(t1, t2);
		}

		//

		template <typename Tmm, typename T>
		struct Mvar
			: Mexpr
		{
			Tmm m_mmValue;

			operator Tmm const & () const
			{
				return m_mmValue;
			}

			//

			Mvar()
				: m_mmValue(Mtraits<Tmm, T>::setzero())
			{}
			template <typename T1>
			Mvar(T1 t1)
				: m_mmValue(Mtraits<Tmm, T>::set1(t1))
			{}
			Mvar(Tmm const & mm)
				: m_mmValue(mm)
			{}

			//

			template <typename T1>
			Mvar(Mop<std::negate, T1> const & tOp)
				: m_mmValue(Mtraits<Tmm, T>::minus(Mtraits<Tmm, T>::setzero(), Mvar(get<0>(tOp))))
			{}
			template <typename T1, typename T2>
			Mvar(Mop<std::plus, T1, T2> const & tOp)
				: m_mmValue(Mtraits<Tmm, T>::plus(Mvar(get<0>(tOp)), Mvar(get<1>(tOp))))
			{}
			template <typename T1, typename T2>
			Mvar(Mop<std::minus, T1, T2> const & tOp)
				: m_mmValue(Mtraits<Tmm, T>::minus(Mvar(get<0>(tOp)), Mvar(get<1>(tOp))))
			{}
			template <typename T1, typename T2>
			Mvar(Mop<std::multiplies, T1, T2> const & tOp)
				: m_mmValue(Mtraits<Tmm, T>::multiplies(Mvar(get<0>(tOp)), Mvar(get<1>(tOp))))
			{}
			template <typename T1, typename T2>
			Mvar(Mop<std::divides, T1, T2> const & tOp)
				: m_mmValue(Mtraits<Tmm, T>::divides(Mvar(get<0>(tOp)), Mvar(get<1>(tOp))))
			{}

			template <typename T1>
			Mvar & operator = (T1 const & t1)
			{
				m_mmValue = Mvar(t1);
				return *this;
			}

			template <typename T1>
			Mvar & operator += (T1 const & t1)
			{
				m_mmValue = Mvar(*this + t1);
				return *this;
			}
			template <typename T1>
			Mvar & operator -= (T1 const & t1)
			{
				m_mmValue = Mvar(*this - t1);
				return *this;
			}
			template <typename T1>
			Mvar & operator *= (T1 const & t1)
			{
				m_mmValue = Mvar(*this * t1);
				return *this;
			}
			template <typename T1>
			Mvar & operator /= (T1 const & t1)
			{
				m_mmValue = Mvar(*this / t1);
				return *this;
			}
		};
	}
}