#pragma once

/* Author: jrmwng @ 2016 */

#include <type_traits>

namespace jrmwng
{
	// f(x) = f(a) + [f'(a) / 1! * (x - a)] + [f''(a) / 2! * (x - a)^2] + [f'''(a) / 3! * (x - a)^3] + ...

	// f'(x) = 0 + [f'(a) / 1! * 1] + [f''(a) / 2! * 2 * (x - a)] + [f'''(a) / 3! * 3 * (x - a)^2] + ...
	// f'(x) = f'(a) + [f''(a) / 1! * (x - a)] + [f'''(a) / 2! * (x - a)^2] + ...

	// f(x) = f(x-1) + [f'(x-1) / 1! * 1] + [f''(x-1) / 2! * 1^2] + [f'''(x-1) / 3! * 1^3] + ...
	// f(x) = f(x-1) + f'(x-1) / 1! + f''(x-1) / 2! + f'''(x-1) / 3! + ...
	// f(x) = f(x+1) - f'(x+1) / 1! + f''(x+1) / 2! + f'''(x+1) / 3! + ...
	// f(x) + f(x) = [f(x-1) + f(x+1)] + [f'(x-1) - f'(x+1)] / 1! + [f''(x-1) + f''(x+1)] / 2! + [f'''(x-1) - f'''(x+1)] / 3! + ...

	// f(x) = f(2x) - [f'(2x) / 1! * x] + [f''(2x) / 2! * x^2] - [f'''(2x) / 3! * x^3] + ...
	// f(x) = f(0x) + [f'(0x) / 1! * x] + [f''(0x) / 2! * x^2] + [f'''(0x) / 3! * x^3] + ...
	// f(x) + f(x) = [f(0x) + f(2x)] + [f'(0x) - f'(2x)] / 1! * x + [f''(0x) + f''(2x)] / 2! * x^2 + [f'''(0x) - f'''(2x)] / 3! * x^3 + ...

	// f(x) = f(x-0.5) + [f'(x-0.5) / 1! * 0.5] + [f''(x-0.5) / 2! * 0.5^2] + [f'''(x-0.5) / 3! * 0.5^3] + ...
	namespace
	{
		template <typename Tinteger, Tinteger... tInteger, typename Tfunc>
		void taylor_for_each(std::integer_sequence<Tinteger, tInteger...>, Tfunc && tFunc)
		{
			using type = int [];
			(void) type {
				(std::forward<Tfunc>(tFunc)(std::integral_constant<Tinteger, tInteger>()), 0)...
			};
		}
		template <typename Tinteger, typename Tfunc>
		void taylor_for_each(std::integer_sequence<Tinteger>, Tfunc && tFunc)
		{
			// NOP
		}

		struct taylor_traits
		{
			template <int nN, typename T>
			static T power_to_factorial(T const & t)
			{
				T tProduct(1);
				{
					taylor_for_each(std::make_integer_sequence<int, nN>(), [&](auto const n)
					{
						tProduct *= t / T(n + 1);
					});
				}
				return tProduct;
			}
			template <template <int nNth> class taylor_function, int nN, typename T>
			static T eval(T const & tX, T const & tA)
			{
				T tSum(0);
				{
					taylor_for_each(std::make_integer_sequence<int, nN>(), [&](auto const n)
					{
						tSum += taylor_function<n.value>::eval(tA) * power_to_factorial<n.value, T>(tX - tA);
					});
				}
				return tSum;
			}
			template <template <int nNth> class taylor_function, int nN, int nA, typename T>
			static T eval(T const & tX)
			{
				T tSum(0);
				{
					taylor_for_each(std::make_integer_sequence<int, nN>(), [&](auto const n)
					{
						tSum += taylor_function<n.value>::template eval_t<nA>() * power_to_factorial<n.value, T>(tX - nA);
					});
				}
				return tSum;
			}
		};
	}
	template <template <int nN> class taylor_function, int nN, typename T>
	T taylor_eval(T const & tX, T const & tA)
	{
		return taylor_traits::eval<taylor_function, nN>(tX, tA);
	}
	template <template <int nN> class taylor_function, int nN, int nA, typename T>
	T taylor_eval(T const & tX)
	{
		return taylor_traits::eval<taylor_function, nN, nA>(tX);
	}

	//

	namespace
	{
		template <typename Ttraits>
		struct taylor_negative_traits;

		template <typename Ttraits>
		struct taylor_negative_traits
		{
			template <int nNth>
			using derivative_t = taylor_negative_traits<typename Ttraits::template derivative_t<nNth>>;

			template <int nX>
			struct eval_t
				: std::integral_constant<int, -Ttraits::template eval_t<nX>::value>
			{};

			template <int nN, typename T>
			static T eval(T const & tX)
			{
				return -Ttraits::eval<nN, T>(tX);
			}
		};

		template <size_t uCase, typename... T>
		struct taylor_switch;
		template <size_t uCase, typename T>
		struct taylor_switch<uCase, T>
		{
			using type = T;
		};
		template <size_t uCase, typename T, typename... TN>
		struct taylor_switch<uCase, T, TN...>
		{
			using type = std::conditional_t<(uCase == 0), T, typename taylor_switch<uCase - 1, TN...>::type>;
		};
		template <size_t uCase, typename... T>
		using taylor_switch_t = typename taylor_switch<uCase, T...>::type;

		struct taylor_sin_traits;
		struct taylor_cos_traits;

		struct taylor_sin_traits
		{
			template <int nNth>
			using derivative_t = taylor_switch_t<(nNth % 4), taylor_sin_traits, taylor_cos_traits, taylor_negative_traits<taylor_sin_traits>, taylor_negative_traits<taylor_cos_traits>>;

			template <int nX>
			struct eval_t;
			template <>
			struct eval_t<0>
				: std::integral_constant<int, 0>
			{};

			template <int nN, typename T>
			static T eval(T const & tX)
			{
				return taylor_eval<derivative_t, nN, 0>(tX);
			}
		};
		struct taylor_cos_traits
		{
			template <int nNth>
			using derivative_t = taylor_switch_t<(nNth % 4), taylor_cos_traits, taylor_negative_traits<taylor_sin_traits>, taylor_negative_traits<taylor_cos_traits>, taylor_sin_traits>;

			template <int nX>
			struct eval_t;
			template <>
			struct eval_t<0>
				: std::integral_constant<int, 1>
			{};

			template <int nN, typename T>
			static T eval(T const & tX)
			{
				return taylor_eval<derivative_t, nN, 0>(tX);
			}
		};
	}
	template <int nN, typename T>
	T taylor_sin(T const & tX)
	{
		return taylor_sin_traits::eval<nN>(tX);
	}
	template <int nN, typename T>
	T taylor_cos(T const & tX)
	{
		return taylor_cos_traits::eval<nN>(tX);
	}
}