#ifndef INCLUDED_FILESYSTEM_INITRD_NATIVEFILE_HH
#define INCLUDED_FILESYSTEM_INITRD_NATIVEFILE_HH

#ifdef TESTING

#include <Common.h>
#include <kernel/Filesystem/FileNode.hh>
#include "RefNode.hh"

namespace Kernel { namespace FS { 

class InitRD_FS;

namespace Init_RD {
	class NativeFileNode : public FS::FileNode, public RefNode
    {
		protected:
		int fd;
		InitRD_FS* fs;
		
		public:
		NativeFileNode(InitRD_FS*, const Utils::string& name, const Utils::string& nativePath) noexcept;
		~NativeFileNode();


		virtual uint64_t read(uint64_t, uint64_t, void*) override;
		virtual uint64_t write(uint64_t, uint64_t, const void*) override;
		virtual void open() override;
		virtual void close() override;
		
		virtual size_t size() const noexcept override;
		
		virtual Filesystem* get_filesystem() const noexcept override;
	};
}
}
}
#endif

#endif