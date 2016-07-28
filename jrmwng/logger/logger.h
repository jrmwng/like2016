#pragma once

/* Author: jrmwng @ 2016 */

#include <stdarg.h>
#include <stdio.h>
#include <chrono>
#include <atomic>
#include <tuple>

namespace jrmwng
{
	namespace
	{
#pragma pack(push,1)
		template <typename... Targs>
		struct alignas(sizeof(void*)) logger_tuple;

		template <typename Targ1, typename... Targs>
		struct alignas(sizeof(void*)) logger_tuple<Targ1, Targs...>
		{
			logger_tuple<Targ1> m_tFirst;
			logger_tuple<Targs...> m_tOthers;

			logger_tuple(Targ1 tFirst, Targs... tOthers)
				: m_tFirst(tFirst)
				, m_tOthers(tOthers...)
			{}
		};
		template <typename Targ>
		struct alignas(sizeof(void*)) logger_tuple<Targ>
		{
			std::conditional_t<std::is_floating_point<Targ>::value, double, Targ> m_tArg;

			logger_tuple(Targ tArg)
				: m_tArg(tArg)
			{}
		};
#pragma pack(pop)

		struct logger_entry
		{
			std::atomic<unsigned> m_uSize;
			std::chrono::time_point<std::chrono::steady_clock> m_stTimePoint;
			void *m_pRetAddr;

			logger_entry(void *pRetAddr)
				: m_uSize(0)
				, m_stTimePoint(std::chrono::steady_clock::now())
				, m_pRetAddr(pRetAddr)
			{}
		};
	}


	struct logger
	{
		enum { BUFFER_CAPACITY = 1000 * 1000 };

		std::atomic<size_t> m_uPosition;
		unsigned char m_aubBuffer[BUFFER_CAPACITY];

		logger()
			: m_uPosition(0)
		{
			__stosb(m_aubBuffer, 0, std::size(m_aubBuffer));
		}

		template <typename... Tfunc>
		void play(Tfunc &&... tFunc)
		{
			for (size_t uPosition = 0; uPosition < m_uPosition.load(std::memory_order_relaxed);)
			{
				auto const & tEntry = reinterpret_cast<std::pair<logger_entry, logger_tuple<wchar_t*>>&>(m_aubBuffer[uPosition]);
				{
					using for_each_t = int [];
					(void) for_each_t {
						(std::forward<Tfunc>(tFunc)((va_list) (&tEntry.second.m_tArg)), 0)...
					};
				}
				uPosition += tEntry.first.m_uSize;
			}
		}

		template <typename... Targs>
		void record(Targs... tArgs)
		{
			using entry_t = std::pair<logger_entry, logger_tuple<Targs...>>;

			size_t const uPosition = m_uPosition.fetch_add(sizeof(entry_t), std::memory_order_acquire);

			entry_t & tEntry = reinterpret_cast<entry_t &>(m_aubBuffer[uPosition]);
			{
				new (&tEntry.first) logger_entry(_ReturnAddress());
				new (&tEntry.second) logger_tuple<Targs...>(tArgs...);
				tEntry.first.m_uSize.store(sizeof(entry_t), std::memory_order_release);
			}
		}
	};
}