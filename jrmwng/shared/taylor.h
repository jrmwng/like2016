#pragma once

/* Author: jrmwng @ 2016 */

#include <type_traits>
#include <ratio>

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
	namespace taylor
	{
		template <typename Tinteger, Tinteger... tInteger, typename Tfunc>
		void for_each(std::integer_sequence<Tinteger, tInteger...>, Tfunc && tFunc)
		{
			using type = int [];
			(void) type {
				(std::forward<Tfunc>(tFunc)(std::integral_constant<Tinteger, tInteger>()), 0)...
			};
		}

		struct taylor_traits
		{
			template <size_t uN, typename T>
			static T power_to_factorial(T t)
			{
				T tProduct(1);
				{
					for_each(std::make_index_sequence<uN>(), [&](auto const n)
					{
						tProduct *= t / T(n + 1);
					});
				}
				return tProduct;
			}
			template <template <size_t uNth> class taylor_function, size_t uN, typename T>
			static T eval(T tX, T tA)
			{
				T tSum(0);
				{
					for_each(std::make_index_sequence<uN>(), [&](auto const n)
					{
						tSum += taylor_function<n>::eval(tA) * power_to_factorial<n, T>(tX - tA);
					});
				}
				return tSum;
			}
			template <template <size_t uNth> class taylor_function, size_t uN, intmax_t nA, typename T>
			static T eval(T tX)
			{
				T tSum(0);
				{
					for_each(std::make_index_sequence<uN>(), [&](auto const n)
					{
						tSum += taylor_function<n>::eval_t<nA>::value * power_to_factorial<n, T>(tX - nA);
					});
				}
				return tSum;
			}
		};

		template <template <size_t uN> class taylor_function, size_t uN, typename T>
		T taylor_eval(T tX, T tA)
		{
			return taylor_traits::eval<taylor_function, uN>(tX, tA);
		}
		template <template <size_t uN> class taylor_function, size_t uN, intmax_t nA, typename T>
		T taylor_eval(T tX)
		{
			return taylor_traits::eval<taylor_function, uN, nA>(tX);
		}

		//

		template <size_t uNth>
		struct taylor_sin_traits;
		template <size_t uNth>
		struct taylor_cos_traits;

		template <>
		struct taylor_sin_traits<0>
		{
			template <intmax_t nX>
			struct eval_t;
			template <>
			struct eval_t<0>
				: std::integral_constant<intmax_t, 0>
			{};

			template <size_t uN, typename T>
			static T eval(T tX)
			{
				return taylor_eval<taylor_sin_traits, uN, 0>(tX);
			}
		};
		template <>
		struct taylor_cos_traits<0>
		{
			template <intmax_t nX>
			struct eval_t;
			template <>
			struct eval_t<0>
				: std::integral_constant<intmax_t, 1>
			{};

			template <size_t uN, typename T>
			static T eval(T tX)
			{
				return taylor_eval<taylor_cos_traits, uN, 0>(tX);
			}
		};
		template <size_t uNth>
		struct taylor_sin_traits
			: std::conditional_t<(uNth % 2), taylor_cos_traits<0>, taylor_sin_traits<0>>
		{};
		template <size_t uNth>
		struct taylor_cos_traits
			: std::conditional_t<(uNth % 2), taylor_sin_traits<0>, taylor_cos_traits<0>>
		{};
		template <size_t uN, typename T>
		T taylor_sin(T tX)
		{
			return taylor_sin_traits<0>::eval<uN>(tX);
		}
		template <size_t uN, typename T>
		T taylor_cos(T tX)
		{
			return taylor_cos_traits<0>::eval<uN>(tX);
		}
	}
}