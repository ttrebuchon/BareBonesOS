#ifndef INCLUDED_FILESYSTEM_NODE_HH
#define INCLUDED_FILESYSTEM_NODE_HH

#include <Common.h>
#include "NodeType.hh"
#include <Utils/string>

namespace Kernel { namespace Filesystem {
	
	class DirEnt;
	
	class Node
	{
		
		private:
		static uint32_t counter;
		
		protected:
		NodeType _type;
		
		Node(const NodeType);
		
		public:
		uint32_t inode;
		Utils::string name;
		const NodeType& type;

		virtual uint32_t read(uint32_t, uint32_t, uint8_t*) = 0;
		virtual uint32_t write(uint32_t, uint32_t, const uint8_t*) = 0;
		virtual void open() = 0;
		virtual void close() = 0;
		virtual DirEnt* readDir(uint32_t) = 0;
		virtual Node* findDir(const char* name) = 0;
		
		
		bool isKind(const NodeType n) const noexcept
		{
			return (type & n) != 0;
		}

    };




}
}
#endif