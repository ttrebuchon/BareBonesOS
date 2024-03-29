#pragma once
#include <iostream>
#include <Common.h>

namespace Drivers
{
	class Disk;
}

namespace Kernel::FS
{
	class Filesystem;
}

class QA
{
	private:
	static void Out_Init();
	static void MultiBoot_Init();
	static void Timer_Init();
	static void Filesystem_Init();
	static void Paging_Init();
	static void Processors_Init();
	static void Drivers_Init();
	
	static void* phys_start;
	static void* phys_end;
	
	static timer_t PIC_timer;
	static struct itimerspec PIC_config;
	
	protected:
	
	class _ostream;
	
	
	
	public:
	const static int PIC_signal;
	
	static std::ostream out;
	
	class Memory;
	class Paging;
	
	static void Init();
	
	static const void* physical_start() noexcept
	{
		return phys_start;
	}
	
	static const void* physical_end() noexcept
	{
		return phys_end;
	}
	
	static void DisableTimer();
	static void EnableTimer();
	static void SetTimerInterval_ns(unsigned long long nanoseconds);
	
	static void SetTimerInterval_ms(unsigned long long milliseconds)
	{
		SetTimerInterval_ns(milliseconds*1000*1000);
	}
	
	static void CheckPIC();
	
	static timer_t GetPICTimer()
	{ return PIC_timer; }
	
	static Drivers::Disk* QADrive(const char* file, const size_t size);
	static Drivers::Disk* QADrive(const char* file);
	static Drivers::Disk* QACheckReadOnlyDrive(const char* file, const size_t size);
	static Drivers::Disk* QACheckReadOnlyDrive(const char* file);
	static void Destroy_QADrive(Drivers::Disk*);
	
	static Kernel::FS::Filesystem* TempFilesystem(const char* storage_file, const size_t size);
	static Kernel::FS::Filesystem* TempFilesystem(Drivers::Disk* disk);
	static void Destroy_Filesystem(Kernel::FS::Filesystem*);
	static Kernel::FS::Filesystem* MountTempFilesystem(const char* mntPath, const char* mntName, const char* diskName, const size_t diskSize, Kernel::FS::Filesystem* rootFS = nullptr);
	static bool UnmountTempFilesystem(const char* mntPath, const char* mntName, Kernel::FS::Filesystem* fs = nullptr, Kernel::FS::Filesystem* rootFS = nullptr);
	
	static bool NativeFileMap(const char* path, const char* native, bool delete_if_exists = false);
	
	static void EnableMemPool() noexcept;
	static void DisableMemPool() noexcept;
	static bool MemPoolEnabled() noexcept;
	
	
	static void EnablePrintAllocs();
	static void DisablePrintAllocs();
	
	
	static void SetupFolders();
	
	private:
	template <class... Types>
	static const char* get_types_string()
	{ return __func__; }
	static int type_func_prefix_count;
	static int type_func_suffix_count;
	static std::ostream& write_types_string(std::ostream&, const char*, size_t type_count);
	
	public:
	
	template <class... Types>
	static std::ostream& write_type()
	{
		return write_type<Types...>(out);
	}
	
	template <class... Types>
	static std::ostream& write_type(std::ostream& os)
	{
		return write_types_string(os, get_types_string<Types...>(), sizeof...(Types));
	}
	
	static std::ostream& write_break(std::ostream&);
	static std::ostream& write_divider(std::ostream&);
	static std::ostream& write_break();
	static std::ostream& write_divider();
	static const char* get_break();
	static const char* get_divider();
	static std::ostream& split_sections(std::ostream&);
	static std::ostream& split_sections();
	static std::ostream& split_sections(std::ostream&, const char* name);
	static std::ostream& split_sections(const char* name);
	
	__attribute__((__always_inline__))
	static std::ostream& lbreak(std::ostream& os)
	{
		return write_break(os);
	}
	
	__attribute__((__always_inline__))
	static std::ostream& lnbreak(std::ostream& os)
	{
		return lbreak(os);
	}
	
	__attribute__((__always_inline__))
	static std::ostream& br(std::ostream& os)
	{
		return lbreak(os);
	}
	
	__attribute__((__always_inline__))
	static std::ostream& BR(std::ostream& os)
	{
		return lbreak(os);
	}
	
	
	__attribute__((__always_inline__))
	static std::ostream& div(std::ostream& os)
	{
		return write_divider(os);
	}
	
	__attribute__((__always_inline__))
	static std::ostream& hr(std::ostream& os)
	{
		return write_divider(os);
	}
};