#pragma once

/* Author: jrmwng @ 2016 */

#include <utility>

namespace jrmwng
{
	template <typename... Targs>
	struct thread_local_call_s
	{
		static thread_local thread_local_call_s *g_pHead = nullptr;
		thread_local_call_s *m_pNext;

		void(*const m_pfnCallback)(void *pContext, Targs ... tArgs);
		void *const m_pContext;

		thread_local_call_s(void (*pfnCallback)(void *pContext), void *pContext)
			: m_pNext(g_pHead)
			, m_pfnCallback(pfnCallback)
			, m_pContext(pContext)
		{
			g_pHead = this;
		}
		~thread_local_call_s()
		{
			if (g_pHead == this)
			{
				g_pHead = m_pNext;
			}
		}
	};
	template <typename... Targs>
	void thread_local_call(Targs ... tArgs)
	{
		for (thread_local_call_s<Targs...> *pNode = g_pHead; pNode; pNode = pNode->m_pNext)
		{
			if (pNode->m_pfnCallback)
			{
				pNode->m_pfnCallback(m_pContext, tArgs...);
			}
		}
	}

	//

	template <typename Tcallback, typename... Targs>
	struct thread_local_callback_s
	{
		Tcallback m_Callback;

		thread_local_call_s<Targs...> m_Call;

		static void Callback(void *pContext, Targs ... tArgs)
		{
			(*reinterpret_cast<Tcallback*>(pContext))(tArgs...);
		}

		thread_local_callback_s(Tcallback && tCallback)
			: m_Callback(std::forward<Tcallback>(tCallback))
			, m_Call(&Callback, &m_Callback)
		{}
	};

	template <typename... Targs, typename Tcallback>
	auto thread_local_callback(Tcallback && tCallback)
	{
		return thread_local_callback_s<Tcallback, Targs...>(std::forward<Tcallback>(tCallback));
	}
}