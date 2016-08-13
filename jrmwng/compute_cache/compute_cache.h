#pragma once

/* Author: jrmwng @ 2016 */

#include <memory>
#include <tuple>
#include <map>
#include <array>
#include <type_traits>

namespace jrmwng
{
	struct compute_cache_manager_base
	{
		template <typename T>
		static T get_key(T && t)
		{
			return t;
		}
		template <typename T>
		static T * get_key(std::shared_ptr<T> && sp)
		{
			return sp.get();
		}
		template <typename Tfunc, typename... Targs>
		static auto make_key(Tfunc && tFunc, Targs && ...tArgs)
		{
			return std::make_tuple(get_key(std::forward<Tfunc>(tFunc)), get_key(std::forward<Targs>(tArgs))...);
		}

		template <typename Tfunc, typename... Targs>
		static auto compute(Tfunc && tFunc, Targs && ...tArgs)
		{
			return std::forward<Tfunc>(tFunc)(tArgs...);
		}
		template <typename Tfunc, typename... Targs>
		static auto compute(std::shared_ptr<Tfunc> && spFunc, Targs && ...tArgs)
		{
			return (*spFunc)(tArgs...);
		}
	};
	template <typename Tmap, typename Tcompute, typename... Targs>
	class compute_cache_manager
		: compute_cache_manager_base
	{
		std::shared_ptr<Tmap> m_spComputeCacheMap;

		template <typename T, typename Tkey>
		void install_evict_cache(T && t, Tkey const & keyCache)
		{
			// NOP
		}
		template <typename T, typename Tkey>
		void install_evict_cache(std::shared_ptr<T> && sp, Tkey const & keyCache)
		{
			std::shared_ptr<Tmap> spComputeCacheMap(m_spComputeCacheMap);

			sp.reset(sp.get(), [sp, spComputeCacheMap, keyCache](T *pt)
			{
				spComputeCacheMap->erase(keyCache);
			});
		}

	public:
		template <typename Tcompute, typename... Targs>
		auto compute_cache(Tcompute && tCompute, Targs && ...tArgs)
		{
			auto keyCache = make_key(std::forward<Tcompute>(tCompute), std::forward<Targs>(tArgs)...);

			if (!m_spComputeCacheMap)
			{
				m_spComputeCacheMap.reset(new Tmap);
			}

			if (m_spComputeCacheMap->find(keyCache) == m_spComputeCacheMap->end())
			{
				(*m_spComputeCacheMap)[keyCache] = std::move(compute(std::forward<Tcompute>(tCompute), std::forward<Targs>(tArgs)...));
			}

			using for_each_t = int [];
			(void) for_each_t {
				(install_evict_cache(std::forward<Tcompute>(tCompute), keyCache), 0),
				(install_evict_cache(std::forward<Targs>(tArgs), keyCache), 0)...
			};

			return (*m_spComputeCacheMap)[keyCache];
		}

		static compute_cache_manager thread_local g_Instance;
	};

	template <typename Tmap, typename Tcompute, typename... Targs>
	compute_cache_manager<Tmap, Tcompute, Targs...> thread_local compute_cache_manager<Tmap, Tcompute, Targs...>::g_Instance;

	template <typename Tcompute, typename... Targs>
	auto compute_cache(Tcompute && tCompute, Targs && ...tArgs)
	{
		using cache_map_t = std::map<decltype(compute_cache_manager_base::make_key(std::forward<Tcompute>(tCompute), std::forward<Targs>(tArgs)...)), decltype(compute_cache_manager_base::compute(std::forward<Tcompute>(tCompute), std::forward<Targs>(tArgs)...))>;

		return compute_cache_manager<cache_map_t, Tcompute, Targs...>::g_Instance.compute_cache(std::forward<Tcompute>(tCompute), std::forward<Targs>(tArgs)...);
	}

}