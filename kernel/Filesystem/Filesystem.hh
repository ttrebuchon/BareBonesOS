#ifndef INCLUDED_FILESYSTEM_HH
#define INCLUDED_FILESYSTEM_HH

#include <Common.h>
#include <Utils/string>
#include "Path_Fwd.hh"
#include "node_ptr.hh"

namespace Drivers
{
	class Disk;
}

namespace Kernel { namespace FS {
	
	class Node;
	class Factory;
	class DirectoryNode_v;
	
	
	class Filesystem
	{
		public:
		typedef node_ptr<> node_type;
		typedef const node_ptr<>& ref_node_type;
		
		
		private:
		template <class T>
		using ptr_type = node_ptr<T>;
		
		template <class T>
		using rptr_type = const node_ptr<T>&;
		
		protected:
		node_type _root;
		
		Filesystem() noexcept;
		
		public:
		static Filesystem* Current;
		
		virtual node_type getNode(const Utils::string& path, const Utils::string& relative = Utils::string());
		virtual node_type getNode(const Path_t& path);
		
		
		virtual node_type root() const noexcept __attribute__((__always_inline__))
		{
			return _root;
		}
		
		virtual ptr_type<DirectoryNode_v> rootd() const noexcept;
		
		virtual Factory& factory() noexcept = 0;
	};
	
}
}

#endif