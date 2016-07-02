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
		struct Mexpr
		{};

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
		template <typename T1, typename T2, typename Tenable = std::enable_if_t<(std::is_base_of<Mexpr, T1>::value && std::is_base_of<Mexpr, T2>::value)>>
		auto operator + (T1 const & t1, T2 const & t2)
		{
			return Mop<std::plus, T1, T2>(t1, t2);
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
		template <typename T1, typename T2, typename Tenable = std::enable_if_t<(std::is_base_of<Mexpr, T1>::value && std::is_base_of<Mexpr, T2>::value)>>
		auto operator - (T1 const & t1, T2 const & t2)
		{
			return Mop<std::minus, T1, T2>(t1, t2);
		}
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
		template <typename T1, typename T2, typename Tenable = std::enable_if_t<(std::is_base_of<Mexpr, T1>::value && std::is_base_of<Mexpr, T2>::value)>>
		auto operator * (T1 const & t1, T2 const & t2)
		{
			return Mop<std::multiplies, T1, T2>(t1, t2);
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
		template <typename T1, typename T2, typename Tenable = std::enable_if_t<(std::is_base_of<Mexpr, T1>::value && std::is_base_of<Mexpr, T2>::value)>>
		auto operator / (T1 const & t1, T2 const & t2)
		{
			return Mop<std::divides, T1, T2>(t1, t2);
		}

		//

		template <typename Tmm, typename T>
		struct Mtraits;
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
			Mvar(T t)
				: m_mmValue(Mtraits<Tmm, T>::set1(t))
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