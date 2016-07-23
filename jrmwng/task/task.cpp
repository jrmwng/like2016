#include "task.h"

#include <intrin.h>
#include <Windows.h>
#include <algorithm>
#include <numeric>
#include <atomic>

namespace
{
	struct manual_event
	{
		HANDLE m_hHandle;

		operator HANDLE () const
		{
			return m_hHandle;
		}

		manual_event()
			: m_hHandle(NULL)
		{
			m_hHandle = CreateEvent(NULL, TRUE, FALSE, NULL);
			if (m_hHandle == NULL)
			{
				__debugbreak();
			}
		}
		~manual_event()
		{
			if (m_hHandle != NULL)
			{
				if (CloseHandle(m_hHandle) == FALSE)
				{
					__debugbreak();
				}
			}
		}
	};
}

namespace jrmwng
{
	struct alignas(128) task_scheduler::item_t
	{
		void(*m_pfnTask)(void *);
		void *m_pContext;

		std::atomic_bool *m_pbDoneEvent;

		std::atomic<HANDLE> m_hDoneEvent;

		item_t()
			: m_pfnTask(nullptr)
			, m_pContext(nullptr)
			, m_pbDoneEvent(nullptr)
		{}

		void setup(void(*pfnTask)(void*), void *pContext, std::atomic_bool *pbDoneEvent)
		{
			m_pfnTask = pfnTask;
			m_pContext = pContext;
			m_pbDoneEvent = pbDoneEvent;
			m_hDoneEvent.store(NULL, std::memory_order_relaxed);
			_mm_clflush(this); // reduce stall time of worker thread
		}
		bool execute()
		{
			m_pfnTask(m_pContext); // execute the actual work item

			_mm_sfence();
			m_pbDoneEvent->store(true, std::memory_order_release);
			_mm_clflush(m_pbDoneEvent);
			_mm_prefetch(reinterpret_cast<char const*>(m_pbDoneEvent), _MM_HINT_T2);

			HANDLE hDoneEvent = m_hDoneEvent.load(std::memory_order_relaxed);
			if (hDoneEvent)
			{
				if (SetEvent(hDoneEvent) == FALSE)
				{
					__debugbreak();
				}
			}

			return true;
		}
		void synchronize(std::atomic_bool & bDoneEvent, HANDLE hDoneEvent)
		{
			if (bDoneEvent.load(std::memory_order_relaxed) == false)
			{
				if (ResetEvent(hDoneEvent) == FALSE)
				{
					__debugbreak();

					while (!bDoneEvent)
					{
						Sleep(1);
					}
				}
				else
				{
					m_hDoneEvent.store(hDoneEvent, std::memory_order_relaxed);
					_mm_clflush(this); // reduce stall time of worker thread

					_mm_sfence();
					if (bDoneEvent.load(std::memory_order_release))
					{
						DWORD dwWait = WaitForSingleObject(hDoneEvent, INFINITE);

						if (dwWait != WAIT_OBJECT_0)
						{
							__debugbreak();
						}
					}
				}
			}
		}
	};
	struct alignas(4096) task_scheduler_impl
	{
		enum { MAX_NUM_OF_THREAD = 4 };
		enum { MAX_NUM_OF_ITEM = 32 };

		using item_t = task_scheduler::item_t;

		//

		std::atomic_bool m_bExitEvent;
		std::atomic_uint m_uWakeCount;
		manual_event m_hWakeEvent;

		manual_event m_ahDoneEvent[MAX_NUM_OF_ITEM];

		struct alignas(128) thread_t
		{
			HANDLE hThread;
			DWORD dwThreadId;
		};
		alignas(128) thread_t m_astThread[MAX_NUM_OF_THREAD];

		alignas(128) item_t m_astItem[MAX_NUM_OF_ITEM];

		alignas(128) std::atomic_bool m_abLock[MAX_NUM_OF_ITEM];
		alignas(128) std::atomic_bool m_abExec[MAX_NUM_OF_ITEM];
		alignas(128) std::atomic_bool m_abDone[MAX_NUM_OF_ITEM];

		task_scheduler_impl()
			: m_bExitEvent(false)
			, m_uWakeCount(0)
		{
			for (size_t uIndex = 0; m_astThread + uIndex < std::end(m_astThread); uIndex++)
			{
				DWORD dwThreadId;
				HANDLE hThread = CreateThread(NULL, 0, &task_scheduler_impl::StartRoutine, this, CREATE_SUSPENDED, &dwThreadId);
				{
					if (hThread == NULL)
					{
						__debugbreak();
					}
					else
					{
						if (ResumeThread(hThread) == FALSE)
						{
							__debugbreak();
						}
					}
				}
				m_astThread[uIndex].hThread = hThread;
				m_astThread[uIndex].dwThreadId = dwThreadId;
			}
			std::fill_n(m_abLock, std::size(m_abLock), false);
			std::fill_n(m_abExec, std::size(m_abExec), false);
			std::fill_n(m_abDone, std::size(m_abDone), false);
		}
		~task_scheduler_impl()
		{
			m_bExitEvent.store(true, std::memory_order_relaxed);

			if (SetEvent(m_hWakeEvent) == FALSE)
			{
				__debugbreak();
			}

			HANDLE ahThread[MAX_NUM_OF_THREAD];
			DWORD dwThreadCount = 0;
			{
				for (size_t uIndex = 0; m_astThread + uIndex < std::end(m_astThread); uIndex++)
				{
					HANDLE hThread = m_astThread[uIndex].hThread;
					if (hThread != NULL)
					{
						ahThread[dwThreadCount++] = hThread;
					}
				}
			}
			DWORD const dwWait = WaitForMultipleObjects(dwThreadCount, ahThread, TRUE, INFINITE);

			if (dwWait != WAIT_OBJECT_0)
			{
				__debugbreak();
			}
		}

		void * operator new (size_t uSize)
		{
			return _aligned_malloc(uSize, 4096);
		}
		void operator delete (void *p)
		{
			_aligned_free(p);
		}

		DWORD start_routine()
		{
			DWORD dwWait = WAIT_OBJECT_0;
			for (bool bExitEvent = false; bExitEvent == false && ((m_bExitEvent.load(std::memory_order_relaxed) || (dwWait = WaitForSingleObject(m_hWakeEvent, INFINITE)) == WAIT_OBJECT_0));)
			{
				bExitEvent = m_bExitEvent.load(std::memory_order_acquire); // reads of 'm_abExec' follow this statement
				do
				{
					m_uWakeCount.fetch_add(1, std::memory_order_relaxed);
					if (SetEvent(m_hWakeEvent) == FALSE)
					{
						__debugbreak();
					}

					while (std::count_if(std::begin(m_abExec), std::end(m_abExec), [&](std::atomic_bool & bExec)->bool
					{
						return bExec.load(std::memory_order_relaxed) && bExec.exchange(0, std::memory_order_acquire) && m_astItem[&bExec - m_abExec].execute();
					}) > 0)
					{
						_mm_pause();
					}

					if (ResetEvent(m_hWakeEvent) == FALSE)
					{
						__debugbreak();
					}
					m_uWakeCount.fetch_sub(1, std::memory_order_acquire);
				}
				while (std::any_of(std::cbegin(m_abExec), std::cend(m_abExec), [&](std::atomic_bool const & bExec)->bool
				{
					return bExec;
				}));
			}
			return dwWait == WAIT_OBJECT_0 ? EXIT_SUCCESS : EXIT_FAILURE;
		}
		static DWORD WINAPI StartRoutine(LPVOID lpParameter)
		{
			return reinterpret_cast<task_scheduler_impl*>(lpParameter)->start_routine();
		}

		item_t * execute(void(*pfnTask)(void *), void *pContext)
		{
			auto itLock = std::find_if(std::begin(m_abLock), std::end(m_abLock), [&](std::atomic_bool & bLock)->bool
			{
				if (bLock || bLock.exchange(true, std::memory_order_acquire))
				{
					return false;
				}
				else
				{
					size_t uIndex = &bLock - m_abLock;

					auto & stItem = m_astItem[uIndex];
					{
						stItem.setup(pfnTask, pContext, &m_abDone[uIndex]);
					}

					m_abDone[uIndex].store(false, std::memory_order_relaxed);
					_mm_sfence();
					m_abExec[uIndex].store(true, std::memory_order_release); // this statement follows preceeding writes

					if (m_uWakeCount == 0)
					{
						if (SetEvent(m_hWakeEvent) == FALSE)
						{
							__debugbreak();
						}
					}
					return true;
				}
			});
			return itLock == std::end(m_abLock) ? nullptr : &m_astItem[itLock - m_abLock];
		}
		bool synchronize(item_t *pstItem)
		{
			if (pstItem)
			{
				size_t uIndex = pstItem - m_astItem;

				if (uIndex < MAX_NUM_OF_ITEM)
				{
					pstItem->synchronize(m_abDone[uIndex], m_ahDoneEvent[uIndex]);
					_mm_sfence();
					m_abLock[uIndex].store(true, std::memory_order_release);
					return true;
				}
			}
			return false;
		}
	};
	struct alignas(4096) task_scheduler::impl
		: task_scheduler_impl
	{};
	task_scheduler::task_scheduler()
		: m_pImpl(new impl)
	{
	}
	task_scheduler::~task_scheduler()
	{
		delete m_pImpl;
	}
	task_scheduler::handle_t task_scheduler::execute(void(*pfnTask)(void *pContext), void *pContext)
	{
		unsigned long long const uxlTsc0 = __rdtsc();
		auto hTask = m_pImpl->execute(pfnTask, pContext);
		unsigned long long const uxlTsc1 = __rdtsc();
		printf("%s: %I64u%s\n", "execute", uxlTsc1 - uxlTsc0, "cycles");
		return hTask;
	}
	bool task_scheduler::synchronize(task_scheduler::handle_t hTask)
	{
		unsigned long long const uxlTsc0 = __rdtsc();
		auto bSync = m_pImpl->synchronize(hTask);
		unsigned long long const uxlTsc1 = __rdtsc();
		printf("%s: %I64u%s\n", "synchronize", uxlTsc1 - uxlTsc0, "cycles");
		return bSync;
	}
}

int main()
{
	jrmwng::task_scheduler ts;
	ts.for_each(
		[&]()
		{
			printf("Hello World!\n");
		},
		[&]()
		{
			printf("Oh\n");
		},
		[&]()
		{
			printf("Haa\n");
		}
	);
	ts.for_each(std::make_integer_sequence<unsigned, 10>(), [&](auto const i)
	{
		printf("%u\n", i.value);
	});
	Sleep(100);
	return 0;
}