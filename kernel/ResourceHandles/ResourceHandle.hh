#ifndef INCLUDED_RESOURCEHANDLE_HH
#define INCLUDED_RESOURCEHANDLE_HH

#include <Common.h>
#include <Utils/unique_ptr>
#include "Resource_Deleter.hh"

namespace Kernel
{
	class ResourceHandle
	{
		protected:
		
		virtual void __cleanup() noexcept = 0;
		virtual void __makeActive() noexcept = 0;
		virtual void __makeInactive() noexcept = 0;
		bool __active;
		bool __cleaned;
		
		public:
		
		ResourceHandle() = default;
		ResourceHandle(const ResourceHandle&) = delete;
		ResourceHandle(ResourceHandle&&) = default;
		
		virtual ~ResourceHandle() noexcept;
		
		
		ResourceHandle& operator=(const ResourceHandle&) = delete;
		ResourceHandle& operator=(ResourceHandle&&) = default;
		
		void cleanup();
		void makeActive();
		void makeInactive();
	};
	
	template <class T = ResourceHandle>
	using ResourcePtr = Utils::unique_ptr<T, Resource_Deleter>;
	
	
}

#endif