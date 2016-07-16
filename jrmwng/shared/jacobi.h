#pragma once

/* Author: jrmwng @ 2016 */

// Jacobi Method

// Ax = b

#include <type_traits>

namespace jrmwng
{
	namespace jacobi
	{
		template <typename Tfunc, size_t... uIndex>
		void for_each(std::integer_sequence<size_t, uIndex...>, Tfunc && tFunc)
		{
			using type = int [];
			(void) type {
				(std::forward<Tfunc>(tFunc)(uIndex), 0)...
			};
		}
	}
	template <typename T, size_t uN>
	void jacobi_iterate(T const (&aatA)[uN][uN], T const (&atB)[uN], T const (&atX0)[uN], T(&atX1)[uN])
	{
		// X1[i] = X0[i] + (b[i] - sum(A[i][j] * X0[j])) / A[i][i]
		jacobi::for_each(std::make_index_sequence<uN>(), [&](auto const i)
		{
			T tDelta = atB[i];
			{
				jacobi::for_each(std::make_index_sequence<uN>(), [&](auto const j)
				{
					tDelta -= aatA[i][j] * atX0[j];
				});
			}
			atX1[i] = atX0[i] + tDelta / aatA[i][i];
		});
	}
}
