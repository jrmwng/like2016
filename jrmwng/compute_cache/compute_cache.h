#pragma once

/* Author: jrmwng @ 2016 */

#include <memory>
#include <tuple>
#include <map>
#include <type_traits>

namespace jrmwng
{
	struct compute_cache_manager_base
	{
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
		template <typename... Targs>
		static auto make_compute(Targs && ...tArgs)
		{
			return std::make_tuple(get_key(std::forward<Targs>(tArgs))...);
		}

		template <typename Tfunc, typename... Targs>
		static auto make_cache(Tfunc && tFunc, Targs && ...tArgs)
		{
			return std::forward<Tfunc>(tFunc)(std::forward<Targs>(tArgs)...);
		}
		template <typename Tfunc, typename... Targs>
		static auto make_cache(std::shared_ptr<Tfunc> & spFunc, Targs && ...tArgs)
		{
			return (*spFunc)(std::forward<Targs>(tArgs)...);
		}
	};
	template <typename Tmap, typename... Targs>
	class compute_cache_manager
		: compute_cache_manager_base
	{
		std::shared_ptr<Tmap> const m_spComputeCacheMap;

		template <typename T, typename Tkey>
		void install_eviction(T && t, Tkey & keyCache)
		{
			// NOP
		}
		template <typename T, typename Tkey>
		void install_eviction(std::shared_ptr<T> & sp, Tkey keyComputeCache)
		{
			if (sp)
			{
				T *pt = sp.get();
				sp.reset(pt, [spCopy = std::move(sp), spComputeCacheMap = m_spComputeCacheMap, keyComputeCache](T *pt)
				{
					spComputeCacheMap->erase(keyComputeCache);
				});
			}
		}

	public:
		compute_cache_manager()
			: m_spComputeCacheMap(new Tmap)
		{}

		auto compute_cache(Targs && ...tArgs)
		{
			auto keyComputeCache = make_compute(std::forward<Targs>(tArgs)...);

			if (m_spComputeCacheMap->find(keyComputeCache) == m_spComputeCacheMap->end())
			{
				(*m_spComputeCacheMap)[keyComputeCache] = std::move(make_cache(std::forward<Targs>(tArgs)...));

				using for_each_t = int [];
				(void) for_each_t {
					(install_eviction(std::forward<Targs>(tArgs), keyComputeCache), 0)...
				};
			}

			return (*m_spComputeCacheMap)[keyComputeCache];
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

}