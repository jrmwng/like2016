#pragma once

/* Author: jrmwng @ 2016 */

#include <memory>
#include <tuple>
#include <type_traits>
#include <functional>

namespace jrmwng
{
	class task_scheduler;

	class task_scheduler
		: std::enable_shared_from_this<task_scheduler>
	{
		struct impl;

		impl * const m_pImpl;
	public:
		struct item_t;
		typedef item_t * handle_t;

		task_scheduler();
		~task_scheduler();

		handle_t execute(void(*pfnTask)(void *pContext), void *pContext);
		bool synchronize(handle_t hTask);

		template <typename Tfunc>
		struct task_t
		{
			Tfunc m_tFunc;
			task_scheduler * m_pScheduler;
			handle_t m_hHandle;

			static void execute(void *pThis)
			{
				reinterpret_cast<task_t*>(pThis)->m_tFunc();
			}

			task_t(task_t && that)
				: m_tFunc(std::move(that.m_tFunc))
				, m_pScheduler(that.m_pScheduler)
				, m_hHandle(that.m_hHandle)
			{
				that.m_pScheduler = nullptr;
				that.m_hHandle = nullptr;
			}
			task_t(task_scheduler *pScheduler, Tfunc && tFunc)
				: m_tFunc(std::forward<Tfunc>(tFunc))
				, m_pScheduler(pScheduler)
				, m_hHandle(pScheduler->execute(&task_t::execute, this))
			{}
			~task_t()
			{
				if (m_pScheduler && m_hHandle)
				{
					m_pScheduler->synchronize(m_hHandle);
				}
			}
		};

		template <typename... Tfunc>
		auto for_each(Tfunc && ... tFunc)
		{
			return std::make_tuple(std::move(task_t<Tfunc>(this, std::forward<Tfunc>(tFunc)))...);
		}
		template <typename Tint, Tint... tInt, typename Tfunc>
		auto for_each(std::integer_sequence<Tint, tInt...>, Tfunc && tFunc)
		{
			return std::make_tuple(std::move(for_each([tFunc]()
			{
				tFunc(std::integral_constant<Tint, tInt>());
			}))...);
		}
	};
}
