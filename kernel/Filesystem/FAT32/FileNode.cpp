#include "FileNode.hh"

namespace Kernel::FS
{
	
	
	FAT32FileNode::FAT32FileNode(FAT32& fs, FAT32DirEnt_t* entity, DirectoryNode_v* parent) : Node(NodeType::File), FileNode(), FAT32Node(fs, NodeType::File, entity, parent)
	{
		
	}
	
	
	uint64_t FAT32FileNode::read(uint64_t off, uint64_t len, void* buf)
	{
		if (len == 0)
		{
			return 0;
		}
		assert(buf);
		assert(off + len < size());
		
		
		const uint32_t clu_sz = fs->cluster_size();
		
		
		uint32_t cluster_num = ((uint32_t)entity->cluster_high) << 16;
		cluster_num |= (uint32_t)entity->cluster_low;
		
		uint64_t cluster_off = off / clu_sz;
		uint64_t cluster_read_count = len / clu_sz;
		
		
		
		uint64_t cluster_index = off / clu_sz;
		uint8_t* buf_it = (uint8_t*)buf;
		if (off % clu_sz)
		{
			uint8_t tmp[clu_sz];
			int res = fs->read_cluster(tmp, cluster_index);
			assert(res == clu_sz);
			if (len + (off % clu_sz) < clu_sz)
			{
				memcpy(buf, tmp + (off % clu_sz), len);
				return len;
			}
			else
			{
				memcpy(buf, tmp + (off % clu_sz), clu_sz - (off % clu_sz));
				buf_it += (clu_sz - (off % clu_sz));
				++cluster_index;
			}
		}
		
		uint64_t last_clust = (off + len) / clu_sz;
		
		
		
		while (cluster_index < last_clust)
		{
			int res = fs->read_cluster(buf_it, cluster_index);
			assert(res == clu_sz);
			buf_it += clu_sz;
			++cluster_index;
		}
		
		if ((off + len) % clu_sz != 0)
		{
			uint8_t tmp[clu_sz];
			int res = fs->read_cluster(tmp, cluster_index);
			assert(res == clu_sz);
			memcpy(buf_it, tmp, (off + len) % clu_sz);
		}
		
		return len;
	}
	
	
	uint64_t FAT32FileNode::write(uint64_t, uint64_t, const void*)
	{
		assert(NOT_IMPLEMENTED);
	}
	
}