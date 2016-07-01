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

			template <size_t uIndex>
			auto const & get() const
			{
				return std::get<uIndex>(m_Args);
			}
		};
		template <size_t uIndex, template <typename T> class Top, typename... Targs>
		auto get(Mop<Top, Targs...> const & tOp)
		{
			return std::get<uIndex>(tOp.m_Args);
		}

		template <typename T1, typename T2, typename Tenable = std::enable_if_t<(std::is_base_of<Mexpr, T1>::value && std::is_base_of<Mexpr, T2>::value)>>
		auto operator + (T1 const & t1, T2 const & t2)
		{
			return Mop<std::plus, T1, T2>(t1, t2);
		}
		template <typename T1, typename T2, typename Tenable = std::enable_if_t<(std::is_base_of<Mexpr, T1>::value && std::is_base_of<Mexpr, T2>::value)>>
		auto operator - (T1 const & t1, T2 const & t2)
		{
			return Mop<std::minus, T1, T2>(t1, t2);
		}
		template <typename T1, typename Tenable = std::enable_if_t<(std::is_base_of<Mexpr, T1>::value)>>
		auto operator - (T1 const & t1)
		{
			return Mop<std::negate, T1>(t1);
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
			return -(Mop<std::multiplies, T1, T2>(t1, get<0>(t2));
		}
		template <typename T1, typename T2, typename Tenable = std::enable_if_t<(std::is_base_of<Mexpr, T1>::value && std::is_base_of<Mexpr, T2>::value)>>
		auto operator * (T1 const & t1, T2 const & t2)
		{
			return Mop<std::multiplies, T1, T2>(t1, t2);
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

		template <typename Tmm, typename T>
		struct Mvar
		{
			Tmm m_mmValue;

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

			template <typename T1, typename T2>
			Mvar(Mop<std::plus, T1, Mop<std::negate, T2>> const & tOp)
				: m_mmValue(Mtraits<Tmm, T>::minus(Mvar(get<0>(tOp)), Mvar(get<0>(get<1>(tOp)))))
			{}
			template <typename T1, typename T2>
			Mvar(Mop<std::plus, T1, T2> const & tOp)
				: m_mmValue(Mtraits<Tmm, T>::plus(Mvar(get<0>(tOp)), Mvar(get<1>(tOp))))
			{}
			template <typename T1, typename T2>
			Mvar(Mop<std::minus, T1, Mop<std::negate, T2>> const & tOp)
				: m_mmValue(Mtraits<Tmm, T>::plus(Mvar(get<0>(tOp)), mVar(get<0>(get<1>(tOp)))))
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
		};
	}
}