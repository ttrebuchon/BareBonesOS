#ifndef INCLUDED_RESOURCEHANDLE_HH
#define INCLUDED_RESOURCEHANDLE_HH

namespace Kernel
{
	class ResourceHandle
	{
		protected:
		
		public:
		
		ResourceHandle() = default;
		ResourceHandle(const ResourceHandle&) = delete;
		ResourceHandle(ResourceHandle&&) = default;
		
		virtual ~ResourceHandle() = default;
		
		
		ResourceHandle& operator=(const ResourceHandle&) = delete;
		ResourceHandle& operator=(ResourceHandle&&) = default;
		
		virtual void cleanup() = 0;
	};
}

#endif