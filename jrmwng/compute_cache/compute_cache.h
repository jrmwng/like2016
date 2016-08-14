#pragma once

/* Author: jrmwng @ 2016 */

#include <memory>
#include <array>
#include <tuple>
#include <map>

namespace jrmwng
{
	class compute_cache_manager_base
	{
		static compute_cache_manager_base thread_local *g_pHead;
		compute_cache_manager_base *m_pNext;
	public:
		compute_cache_manager_base()
			: m_pNext(nullptr)
		{}

		void register_once()
		{
			if (m_pNext == nullptr)
			{
				m_pNext = std::exchange(g_pHead, this);
			}
		}
		virtual void evict_cache() = 0;

		static void flush()
		{
			for (compute_cache_manager_base *pNode = std::exchange(g_pHead, nullptr); pNode; pNode = std::exchange(pNode->m_pNext, nullptr))
			{
				pNode->evict_cache();
			}
		}

		template <typename T, typename Tenable = std::less<T>>
		static T get_key(T t)
		{
			return t;
		}
		template <typename T>
		static T * get_key(std::shared_ptr<T> volatile && sp)
		{
			return sp.get();
		}
		template <typename T>
		static T * get_key(std::shared_ptr<T> const && sp)
		{
			return sp.get();
		}
		template <typename T>
		static T * get_key(std::shared_ptr<T> && sp)
		{
			return sp.get();
		}
		template <typename T>
		static T * get_key(std::shared_ptr<T> volatile & sp)
		{
			return sp.get();
		}
		template <typename T>
		static T * get_key(std::shared_ptr<T> const & sp)
		{
			return sp.get();
		}
		template <typename T>
		static T * get_key(std::shared_ptr<T> & sp)
		{
			return sp.get();
		}
		template <typename Tcompute, typename... Targs>
		static auto make_compute(Tcompute && tCompute, Targs && ...tArgs)
		{
			std::array<char, sizeof(Tcompute)> arrayCompute;
			{
				memcpy(arrayCompute.data(), std::addressof(tCompute), sizeof(Tcompute));
			}
			return std::make_tuple(std::forward<std::array<char, sizeof(Tcompute)>>(arrayCompute), get_key(std::forward<Targs>(tArgs))...);
		}

		template <typename Tfunc, typename... Targs>
		static auto make_cache(Tfunc && tFunc, Targs && ...tArgs)
		{
			return std::forward<Tfunc>(tFunc)(std::forward<Targs>(tArgs)...);
		}
	};
	compute_cache_manager_base thread_local *compute_cache_manager_base::g_pHead = nullptr;

	template <typename Tmap, typename... Targs>
	class compute_cache_manager
		: compute_cache_manager_base
	{
		Tmap m_mapComputeCache;

	public:
		compute_cache_manager()
		{}

		virtual void evict_cache()
		{
			m_mapComputeCache.clear();
		}

		auto compute_cache(Targs && ...tArgs)
		{
			auto keyComputeCache = make_compute(std::forward<Targs>(tArgs)...);

			auto itComputeCache = m_mapComputeCache.find(keyComputeCache);

			if (itComputeCache != m_mapComputeCache.end())
			{
				return itComputeCache->second;
			}
			else
			{
				register_once();
				return m_mapComputeCache[keyComputeCache] = std::move(make_cache(std::forward<Targs>(tArgs)...));
			}
		}

		static compute_cache_manager thread_local g_Instance;
	};

	template <typename Tmap, typename... Targs>
	compute_cache_manager<Tmap, Targs...> thread_local compute_cache_manager<Tmap, Targs...>::g_Instance;

	template <typename... Targs>
	auto compute_cache(Targs && ...tArgs)
	{
		using compute_t = decltype(compute_cache_manager_base::make_compute(std::forward<Targs>(tArgs)...));
		using cache_t = decltype(compute_cache_manager_base::make_cache(std::forward<Targs>(tArgs)...));
		using map_t = std::map<compute_t, cache_t>;

		return compute_cache_manager<map_t, Targs...>::g_Instance.compute_cache(std::forward<Targs>(tArgs)...);
	}
	void compute_cache_flush()
	{
		compute_cache_manager_base::flush();
	}
}