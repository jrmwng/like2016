#include "task.h"

#include <intrin.h>
#include <Windows.h>
#include <algorithm>
#include <numeric>
#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>

namespace
{
	struct logger;

	static thread_local logger *g_pLogger = nullptr;
	struct logger
	{
		char *m_pcBuffer;
		DWORD m_uSize;
		DWORD m_uCapacity;

		logger(char *pcBuffer, DWORD uCapacity)
			: m_pcBuffer(pcBuffer)
			, m_uSize(0)
			, m_uCapacity(uCapacity)
		{
			g_pLogger = this;
		}
		~logger()
		{
			g_pLogger = nullptr;
		}

		void printf(char const *pcFormat, ...)
		{
			va_list pArgs;

			va_start(pArgs, pcFormat);
			int const nCount = vsprintf_s(m_pcBuffer + m_uSize, m_uCapacity - m_uSize, pcFormat, pArgs);
			va_end(pArgs);

			if (nCount > 0)
			{
				m_uSize += nCount;
			}
		}
	};
	template <DWORD uCapacity>
	struct logger_t
		: logger
	{
		char m_acBuffer[uCapacity];

		logger_t()
			: logger(m_acBuffer, uCapacity)
		{}
		~logger_t()
		{
			WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), m_pcBuffer, m_uSize, NULL, NULL);
			//OutputDebugStringA(m_acBuffer);
		}
	};

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

		void set()
		{
			if (SetEvent(m_hHandle) == FALSE)
			{
				__debugbreak();
			}
		}
		void reset()
		{
			if (ResetEvent(m_hHandle) == FALSE)
			{
				__debugbreak();
			}
		}
		bool wait()
		{
			DWORD const dwWait = WaitForSingleObject(m_hHandle, INFINITE);
			{
				if (dwWait == WAIT_FAILED)
				{
					__debugbreak();
				}
			}
			return dwWait == WAIT_OBJECT_0;
		}
	};
	/*
	struct stream_bool
	{
		int m_nValue;

		stream_bool()
			: m_nValue(0)
		{}

		stream_bool & operator = (bool bNewValue)
		{
			m_nValue = bNewValue;
			return *this;
		}

		void store(bool bNewValue, std::memory_order emMemoryOrder)
		{
			if (emMemoryOrder == std::memory_order_release ||
				emMemoryOrder == std::memory_order_acq_rel ||
				emMemoryOrder == std::memory_order_seq_cst)
			{
				_mm_sfence();
			}

			_mm_stream_si32(&m_nValue, bNewValue);
			//m_nValue = bNewValue;

			if (emMemoryOrder == std::memory_order_acquire ||
				emMemoryOrder == std::memory_order_acq_rel ||
				emMemoryOrder == std::memory_order_seq_cst)
			{
				_mm_lfence();
			}
		}

		bool exchange(bool bNewValue, std::memory_order emMemoryOrder)
		{
			return _InterlockedExchange(reinterpret_cast<long volatile*>(&m_nValue), bNewValue) != 0;
		}

		bool load(std::memory_order emMemoryOrder) const
		{
			if (emMemoryOrder == std::memory_order_release ||
				emMemoryOrder == std::memory_order_acq_rel ||
				emMemoryOrder == std::memory_order_seq_cst)
			{
				_mm_sfence();
			}

			int nValue = m_nValue;

			if (emMemoryOrder == std::memory_order_acquire ||
				emMemoryOrder == std::memory_order_acq_rel ||
				emMemoryOrder == std::memory_order_seq_cst)
			{
				_mm_lfence();
			}

			return nValue != 0;
		}
	};
	*/
	using stream_bool = std::atomic_bool;

	unsigned long long rdtsc()
	{
		return __rdtsc();
	}
}

namespace jrmwng
{
	struct alignas(128) task_scheduler::item_t
	{
		void(*m_pfnTask)(void *);
		void *m_pContext;

		stream_bool *m_pbDoneEvent;

		std::atomic<HANDLE> m_hDoneEvent;

		item_t()
			: m_pfnTask(nullptr)
			, m_pContext(nullptr)
			, m_pbDoneEvent(nullptr)
		{}

		void setup(void(*pfnTask)(void*), void *pContext, stream_bool *pbDoneEvent, manual_event const & hDoneEvent)
		{
			m_pfnTask = pfnTask;
			m_pContext = pContext;
			m_pbDoneEvent = pbDoneEvent;
			m_hDoneEvent.store(NULL, std::memory_order_relaxed);
			_mm_clflush(this);
			_mm_prefetch(reinterpret_cast<char const*>(this), _MM_HINT_NTA); // 'this' will be read by another (worker) thread
		}
		bool execute()
		{
			unsigned long long const uxlTsc0 = rdtsc();
			void(*pfnTask)(void*) = m_pfnTask;
			void *pContext = m_pContext;
			stream_bool *pbDoneEvent = m_pbDoneEvent;
			unsigned long long const uxlTsc1 = rdtsc();

			_mm_prefetch(reinterpret_cast<char const*>(pbDoneEvent), _MM_HINT_T0); // '*pbDoneEvent' will be written by this (worker) thread soon

			pfnTask(pContext); // execute the actual work item

			unsigned long long const uxlTsc2 = rdtsc();
			pbDoneEvent->store(true, std::memory_order_release);
			HANDLE hDoneEvent = m_hDoneEvent.load(std::memory_order_relaxed);
			_mm_clflush(pbDoneEvent);
			_mm_prefetch(reinterpret_cast<char const*>(pbDoneEvent), _MM_HINT_NTA); // '*pbDoneEvent' will be read by another (user) thread
			unsigned long long const uxlTsc3 = rdtsc();

			if (hDoneEvent)
			{
				if (SetEvent(hDoneEvent) == FALSE)
				{
					__debugbreak();
				}
			}

			unsigned long long const uxlTsc4 = rdtsc();

			g_pLogger->printf("%016I64X: %s: %I64u%s = %I64u%s + %I64u%s + %I64u%s (%p)\n",
				uxlTsc0, "work-done",
				(uxlTsc4 - uxlTsc0) - (uxlTsc2 - uxlTsc1), "cycles",
				uxlTsc1 - uxlTsc0, "cycles",
				uxlTsc3 - uxlTsc2, "cycles",
				uxlTsc4 - uxlTsc3, "cycles", hDoneEvent);
			return true;
		}
		void synchronize(stream_bool & bDone, manual_event & hDoneEvent, stream_bool & bLock)
		{
			unsigned long long uxlTsc0 = rdtsc();
			unsigned long long uxlTsc1 = uxlTsc0;
			unsigned long long uxlTsc2 = uxlTsc0;
			unsigned long long uxlTsc3 = uxlTsc0;

			if (bDone.load(std::memory_order_relaxed) == false)
			{
				_mm_prefetch(reinterpret_cast<char const*>(&m_hDoneEvent), _MM_HINT_T0); // 'm_hDoneEvent' will be written by this (user) thread

				hDoneEvent.reset();

				uxlTsc1 = rdtsc();
				uxlTsc2 = uxlTsc1;
				uxlTsc3 = uxlTsc1;

				m_hDoneEvent.store(hDoneEvent, std::memory_order_relaxed);
				_mm_clflush(&m_hDoneEvent);
				_mm_prefetch(reinterpret_cast<char const*>(this), _MM_HINT_NTA); // 'm_hDoneEvent' will be read by another (worker) thread

				//_mm_sfence();
				if (bDone.load(std::memory_order_release) == false)
				{
					_mm_clflush(&bDone);
					_mm_prefetch(reinterpret_cast<char const*>(&bDone), _MM_HINT_T2); // 'bDone' will be written by another (worker) thread

					uxlTsc2 = rdtsc();
					uxlTsc3 = uxlTsc2;

					hDoneEvent.wait();

					uxlTsc3 = rdtsc();
				}
			}

			bLock.store(false, std::memory_order_release);
			_mm_clflush(&bLock);
			_mm_prefetch(reinterpret_cast<char const*>(&bLock), _MM_HINT_T2); // 'bLock' will be accessed by another (user or worker) thread

			unsigned long long uxlTsc4 = rdtsc();

			g_pLogger->printf(
				"%016I64X: %s: %I64u%s = %I64u%s + %I64u%s + %I64u%s + %I64u%s\n",
				uxlTsc0, "user-sync",
				uxlTsc4 - uxlTsc0, "cycles",
				uxlTsc1 - uxlTsc0, "cycles",
				uxlTsc2 - uxlTsc1, "cycles",
				uxlTsc3 - uxlTsc2, "cycles",
				uxlTsc4 - uxlTsc3, "cycles");
		}
	};
	struct alignas(4096) task_scheduler_impl
	{
		enum { MAX_NUM_OF_THREAD = 3 };
		enum { MAX_NUM_OF_ITEM = 64 };

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

		alignas(128) stream_bool m_abLock[MAX_NUM_OF_ITEM]; // user threads access. user threads write
		alignas(128) stream_bool m_abExec[MAX_NUM_OF_ITEM]; // user threads write. worker threads access
		alignas(128) stream_bool m_abDone[MAX_NUM_OF_ITEM]; // user threads write. worker threads write. user threads read

		task_scheduler_impl()
			: m_bExitEvent(false)
			, m_uWakeCount(0)
		{
			if (VirtualLock(this, sizeof(*this)) == FALSE)
			{
				__debugbreak();
			}

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
						// TODO: affinity mask, thread priority
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

			m_hWakeEvent.set();

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

			if (VirtualUnlock(this, sizeof(*this)) == FALSE)
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

			for (bool bExitEvent = false; bExitEvent == false && ((m_bExitEvent.load(std::memory_order_relaxed) || m_hWakeEvent.wait()));)
			{
				logger_t<512 * 1024> stLogger;

				bExitEvent = m_bExitEvent.load(std::memory_order_acquire); // reads of 'm_abExec' follow this statement
				do
				{
					m_uWakeCount.fetch_add(1, std::memory_order_relaxed);
					m_hWakeEvent.set();

					unsigned long long uxlTsc0 = rdtsc();
					while (std::count_if(std::begin(m_abExec), std::end(m_abExec), [&](stream_bool & bExec)->bool
					{
						if (bExec.load(std::memory_order_relaxed) && bExec.exchange(0, std::memory_order_acquire))
						{
							unsigned long long uxlTsc1 = rdtsc();
							g_pLogger->printf("%016I64X: %s: %I64u%s\n", uxlTsc0, "work-find", uxlTsc1 - uxlTsc0, "cycles");

							m_astItem[&bExec - m_abExec].execute();

							uxlTsc0 = rdtsc();
							return true;
						}
						else
						{
							return false;
						}
					}) > 0)
					{
						_mm_pause();
					}

					m_hWakeEvent.reset();
					m_uWakeCount.fetch_sub(1, std::memory_order_acquire);
				}
				while (std::any_of(std::cbegin(m_abExec), std::cend(m_abExec), [&](stream_bool const & bExec)->bool
				{
					return bExec.load(std::memory_order_relaxed);
				}));
			}

			delete std::exchange(g_pLogger, nullptr);
			return dwWait == WAIT_OBJECT_0 ? EXIT_SUCCESS : EXIT_FAILURE;
		}
		static DWORD WINAPI StartRoutine(LPVOID lpParameter)
		{
			return reinterpret_cast<task_scheduler_impl*>(lpParameter)->start_routine();
		}

		item_t * execute(void(*pfnTask)(void *), void *pContext)
		{
			unsigned long long const uxlTsc0 = rdtsc();
			_mm_prefetch(reinterpret_cast<char const*>(&m_uWakeCount), _MM_HINT_T0);

			auto itLock = std::find_if(std::begin(m_abLock), std::end(m_abLock), [&](stream_bool & bLock)->bool
			{
				size_t uIndex = &bLock - m_abLock;

				if (bLock.load(std::memory_order_relaxed) || bLock.exchange(true, std::memory_order_acquire))
				{
					return false;
				}
				else
				{
					unsigned long long const uxlTsc1 = rdtsc();

					auto & stItem = m_astItem[uIndex];
					{
						stItem.setup(pfnTask, pContext, &m_abDone[uIndex], m_ahDoneEvent[uIndex]);
					}

					unsigned long long const uxlTsc2 = rdtsc();

					m_abDone[uIndex].store(false, std::memory_order_relaxed);
					_mm_clflush(&m_abDone[uIndex]);
					_mm_prefetch(reinterpret_cast<char const*>(&m_abDone[uIndex]), _MM_HINT_T2); // 'm_abDone[uIndex]' will be written by another (worker) thread

					m_abExec[uIndex].store(true, std::memory_order_release); // this statement follows preceeding writes
					_mm_clflush(&m_abExec[uIndex]);
					_mm_prefetch(reinterpret_cast<char const*>(&m_abExec[uIndex]), _MM_HINT_T2); // 'm_abExec[uIndex]' will be access by another (worker) thread

					unsigned long long const uxlTsc3 = rdtsc();

					if (m_uWakeCount == 0)
					{
						m_hWakeEvent.set();
					}

					unsigned long long const uxlTsc4 = rdtsc();

					g_pLogger->printf(
						"%016I64X: %s: %I64u%s = %I64u%s + %I64u%s + %I64u%s + %I64u%s\n",
						uxlTsc0, "user-exec",
						uxlTsc4 - uxlTsc0, "cycles",
						uxlTsc1 - uxlTsc0, "cycles",
						uxlTsc2 - uxlTsc1, "cycles",
						uxlTsc3 - uxlTsc2, "cycles",
						uxlTsc4 - uxlTsc3, "cycles");
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
					pstItem->synchronize(m_abDone[uIndex], m_ahDoneEvent[uIndex], m_abLock[uIndex]);
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
		return m_pImpl->execute(pfnTask, pContext);
	}
	bool task_scheduler::synchronize(task_scheduler::handle_t hTask)
	{
		return m_pImpl->synchronize(hTask);
	}
}

int main()
{
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

	logger_t<512 * 1024> stLogger;
	{
		jrmwng::task_scheduler ts;

		Sleep(100);

		auto sync0 = ts.for_each(
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

		Sleep(100);

		auto sync1 = ts.for_each(std::make_integer_sequence<unsigned, 50>(), [&](auto const i)
		{
			printf("%u\n", i.value);
		});
		Sleep(1);
	}
	return 0;
}