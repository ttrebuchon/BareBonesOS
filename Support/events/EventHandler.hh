#ifndef INCLUDED_SUPPORT_EVENTS_EVENTHANDLER_HH
#define INCLUDED_SUPPORT_EVENTS_EVENTHANDLER_HH

#include <Utils/functional>
#include <Utils/unordered_set>
#include <Utils/mutex>
#include <Utils/shared_mutex>
#include <Utils/list>

namespace Support { namespace events
{
	
	template <class... Args>
	class EventHandler
	{
		protected:
		#ifdef THREAD_SAFE_EVENTS
		mutable Utils::shared_timed_mutex handlers_m;
		#endif
		Utils::list<Utils::function<void(Args...)>> handlers;
		
		public:
		
		template <class T>
		EventHandler& operator+=(const Utils::function<T(Args...)> handler)
		{
			#ifdef THREAD_SAFE_EVENTS
			Utils::lock_guard<Utils::shared_timed_mutex> lock(handlers_m);
			#endif
			return *this += Utils::function<void(Args...)>(handler);
		}
		
		EventHandler& operator+=(const Utils::function<void(Args...)> handler)
		{
			#ifdef THREAD_SAFE_EVENTS
			Utils::lock_guard<Utils::shared_timed_mutex> lock(handlers_m);
			#endif
			handlers.push_back(handler);
			return *this;
		}
		
		EventHandler& operator+=(const Utils::function<void()> handler)
		{
			#ifdef THREAD_SAFE_EVENTS
			Utils::lock_guard<Utils::shared_timed_mutex> lock(handlers_m);
			#endif
			handlers.push_back([=](auto... args) { handler(); });
			return *this;
		}
		
		virtual void operator()(Args... args) const
		{
			#ifdef THREAD_SAFE_EVENTS
			Utils::shared_lock<Utils::shared_timed_mutex> lock(handlers_m);
			#endif
			for (const auto& hand : handlers)
			{
				hand(args...);
			}
		}
		
		void clear()
		{
			#ifdef THREAD_SAFE_EVENTS
			Utils::lock_guard<Utils::shared_timed_mutex> lock(handlers_m);
			#endif
			handlers.clear();
		}
		
		size_t size() const
		{
			return handlers.size();
		}
	};
	
	
	
	
	template <>
	class EventHandler<>
	{
		protected:
		mutable Utils::shared_timed_mutex handlers_m;
		Utils::list<Utils::function<void()>> handlers;
		
		public:
		
		template <class T>
		EventHandler& operator+=(const Utils::function<T()> handler)
		{
			Utils::lock_guard<Utils::shared_timed_mutex> lock(handlers_m);
			return *this += Utils::function<void()>(handler);
		}
		
		EventHandler& operator+=(const Utils::function<void()> handler)
		{
			Utils::lock_guard<Utils::shared_timed_mutex> lock(handlers_m);
			handlers.push_back(handler);
			return *this;
		}
		
		virtual void operator()() const
		{
			Utils::shared_lock<Utils::shared_timed_mutex> lock(handlers_m);
			for (const auto& hand : handlers)
			{
				hand();
			}
		}
		
		void clear()
		{
			Utils::lock_guard<Utils::shared_timed_mutex> lock(handlers_m);
			handlers.clear();
		}
		
		size_t size() const
		{
			return handlers.size();
		}
	};
}
}

#endif