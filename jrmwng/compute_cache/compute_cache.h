#pragma once

/* Author: jrmwng @ 2016 */

#include <memory>
#include <tuple>
#include <map>

namespace jrmwng
{
	class compute_cache_manager
	{
		std::map<void const *, std::shared_ptr<void>> m_mapComputeCache;

		template <typename T>
		T get_key(T && t)
		{
			return t;
		}
		template <typename T>
		T * get_key(std::shared_ptr<T> && sp)
		{
			return sp.get();
		}
		template <typename... Targs>
		auto make_key(Targs && ...tArgs)
		{
			return std::make_tuple(get_key(std::forward<Targs>(tArgs))...);
		}

		template <typename T, typename Tmap, typename Tkey>
		void install_evict_cache(T & t, std::shared_ptr<Tmap> && spCacheMap, Tkey const & keyCache)
		{
			// NOP
		}
		template <typename T, typename Tmap, typename Tkey>
		void install_evict_cache(std::shared_ptr<T> & sp, std::shared_ptr<Tmap> && spCacheMap, Tkey const & keyCache)
		{
			sp.reset(sp.get(), [sp, spCacheMap, keyCache](T *pt)
			{
				spCacheMap->erase(keyCache);
			});
		}
	public:
		template <typename Tcompute, typename... Targs>
		auto compute_cache(std::shared_ptr<Tcompute> & spCompute, Targs && ...tArgs)
		{
			auto keyCache = make_key(std::forward<Targs>(tArgs)...);

			using cache_map_t = std::map<decltype(keyCache), decltype((*spCompute)(std::forward<Targs>(tArgs)...))>;

			if (m_mapComputeCache.find(spCompute.get()) == m_mapComputeCache.end())
			{
				m_mapComputeCache[spCompute.get()].reset(new cache_map_t);
				spCompute.reset(spCompute.get(), [&, spCompute](Tcompute *ptCompute)
				{
					m_mapComputeCache.erase(ptCompute);
				});
			}
			std::shared_ptr<void> & m_spCacheMap = m_mapComputeCache[spCompute.get()];

			std::shared_ptr<cache_map_t> spCacheMap(m_spCacheMap, reinterpret_cast<cache_map_t*>(m_spCacheMap.get()));

			if (spCacheMap->find(keyCache) == spCacheMap->end())
			{
				(*spCacheMap)[keyCache] = std::move((*spCompute)(std::forward<Targs>(tArgs)...));
			}

			using for_each_t = int [];
			(void) for_each_t {
				(install_evict_cache(tArgs, std::forward<decltype(spCacheMap)>(spCacheMap), keyCache), 0)...
			};

			return (*spCacheMap)[keyCache];
		}

		static compute_cache_manager thread_local g_Instance;
	};
	compute_cache_manager thread_local compute_cache_manager::g_Instance;

	template <typename Tcompute, typename... Targs>
	auto compute_cache(std::shared_ptr<Tcompute> & spCompute, Targs && ...tArgs)
	{
		return compute_cache_manager::g_Instance.compute_cache(spCompute, std::forward<Targs>(tArgs)...);
	}

}