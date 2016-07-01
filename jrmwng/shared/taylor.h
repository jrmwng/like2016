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
		template <size_t uN>
		struct taylor_traits;
		template <>
		struct taylor_traits<0>
		{
			template <typename T>
			static T power_to_factorial(T t)
			{
				return T(1);
			}
			template <template <size_t uNth> class taylor_function, typename T>
			static T eval(T tX, T tA)
			{
				return taylor_function<0>::eval(tA);
			}
			template <template <size_t uNth> class taylor_function, typename T, intmax_t nA>
			static T eval(T tX)
			{
				return taylor_function<0>::eval_t<nA>::value;
			}

			template <intmax_t nExpr>
			using power_to_factorial_t = std::ratio<0, 0>;
			template <template <size_t uNth> class taylor_function, intmax_t nA, intmax_t nX>
			using eval_t = taylor_function<0>::eval_t<nA>;
		};
		template <size_t uN>
		struct taylor_traits
		{
			template <typename T>
			static T power_to_factorial(T t)
			{
				return (t / T(uN)) * taylor_traits<uN - 1>::power_to_factorial(t);
			}
			template <template <size_t uNth> class taylor_function, typename T>
			static T eval(T tX, T tA)
			{
				return taylor_function<uN>::eval(tA) * power_to_factorial<T>(tX - tA) + taylor_traits<uN - 1>::eval<taylor_function>(tX, tA);
			}
			template <template <size_t uNth> class taylor_function, intmax_t nA, typename T>
			static T eval(T tX)
			{
				return taylor_function<uN>::eval_t<nA>::value * power_to_factorial<T>(tX - nA) + taylor_traits<uN - 1>::eval<taylor_function, nA>(tX);
			}

			template <intmax_t nExpr>
			using power_to_factorial_t = typename std::ratio_multiply<std::ratio<nExpr, uN>, taylor_traits<uN - 1>::power_to_factorial_t<nExpr>>::type;
			template <template <size_t uNth> class tayloer_function, intmax_t nA, intmax_t nX>
			using eval_t = typename std::ratio_add<typename std::ratio_multiply<taylor_function<uN>::eval_t<nA>, power_to_factorial_t<nX - nA>>::type, taylor_traits<uN - 1>::eval_t>::type;
		};

		template <template <size_t uN> class taylor_function, size_t uN, typename T>
		T taylor_eval(T tX, T tA)
		{
			return taylor_traits<uN>::eval<taylor_function>(tX, tA);
		}
		template <template <size_t uN> class taylor_function, size_t uN, intmax_t nA, typename T>
		T taylor_eval(T tX)
		{
			return taylor_traits<uN>::eval<taylor_function, nA>(tX);
		}
		template <template <size_t uNth> class taylor_function, size_t uN, intmax_t nA, intmax_t nX>
		using taylor_eval_t = taylor_traits<uN>::eval_t<taylor_function, nA, nX>;

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
				return taylor_traits<uN>::eval<taylor_sin_traits, 0>(tX);
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
				return taylor_traits<uN>::eval<taylor_cos_traits, 0>(tX);
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