#include "QA.hh"
#include "Out.hh"
#include <QUtils/Output/MultiStream.h>
#include <fstream>
#include <boot/MultiBoot.hh>
#include <sys/sysinfo.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "../Tests.hh"
#include <kernel/Memory/PhysicalMemory.hh>
#include <kernel/Memory/Managers/Basic_Physical.hh>
#include <kernel/Memory/PageDir.hh>
#include <kernel/Memory/Paging.hh>
#include <kernel/Memory/PageRegions/DynamicCodeRegion.hh>
#include "QADrive.hh"
#include <kernel/Processor.h>


void QA::Init()
{
	Out_Init();
	out << "Multiboot_Init()" << std::endl;
	MultiBoot_Init();
	out << "Timer_Init()" << std::endl;
	Timer_Init();
	SetTimerInterval_ns(50*1000*1000);
	out << "Filesystem_Init()" << std::endl;
	Filesystem_Init();
	out << "Paging_Init()" << std::endl;
	Paging_Init();
	out << "Processors_Init()" << std::endl;
	Processors_Init();
}

//static QA::_ostream _out;

std::ostream QA::out(nullptr);
void* QA::phys_start = nullptr;
void* QA::phys_end = nullptr;
const int QA::PIC_signal = SIGUSR1;
timer_t QA::PIC_timer;
struct itimerspec QA::PIC_config;


static QUtils::Output::MultiBuf* mb = nullptr;
static std::ofstream* file = nullptr;

void QA::Out_Init()
{
	if (mb)
	{
		return;
	}
	mb = new QUtils::Output::MultiBuf();
	file = new std::ofstream("QA_Out.txt");
	mb->push(file->rdbuf());
	mb->push(std::cout.rdbuf());
	out.rdbuf(mb);
	std::clog.rdbuf(mb);
	std::cout.rdbuf(mb);
	std::cerr.rdbuf(mb);
	//out.rdbuf(std::cerr.rdbuf());
	//std::cerr.rdbuf(nullptr);
}

void QA::MultiBoot_Init()
{
	struct sysinfo sinfo;
	assert(::sysinfo(&sinfo) == 0);
	
	auto free_ram = sinfo.freehigh;
	
	auto pgs = free_ram / PAGE_SIZE;
	
	auto dzero = ::open("/dev/zero", O_RDWR | O_APPEND);
	
	addr_t addrStart = 0xD0000000;
	addr_t addrEnd;
	#ifdef __USE_MEM_POOL__
	addrEnd = addrStart + 0x100000000 - 1;
	#else
	addrEnd = addrStart + 0x100000 - 1;
	#endif
	assert(addrEnd > addrStart);
	size_t len = addrEnd - addrStart;
	auto phys = ::mmap((void*)(addr_t)addrStart, len, PROT_READ | PROT_WRITE, MAP_PRIVATE, dzero, 0);
	assert(phys != MAP_FAILED);
	
	phys_start = phys;
	phys_end = (void*)((addr_t)phys + len);
	
	assert((uint32_t)(addr_t)phys == (addr_t)phys);
	for (unsigned char* ptr = (unsigned char*)addrStart; ptr != (unsigned char*)addrEnd; ++ptr)
	{
		assert(*ptr == 0);
	}
	
	
	auto mb = (multiboot*)malloc(sizeof(multiboot));
	memset(mb, 0, sizeof(multiboot));
	mb->flags |= (1 << 6);
	
	auto map = (multiboot_mmap_t*)::mmap((void*)0xC0000000, 2*sizeof(multiboot_mmap_t), PROT_READ | PROT_WRITE, MAP_PRIVATE, dzero, 0);
	assert(map != MAP_FAILED);
	//auto map = (multiboot_mmap_t*)malloc(sizeof(multiboot_mmap_t));
	
	map->size = sizeof(multiboot_mmap_t);
	map->base_addr = 0x0;
	map->len = (uint64_t)(void*)phys_start;
	map->type = 0;
	
	//auto map2 = &map[1];
	auto map2 = (multiboot_mmap_t*)((addr_t)map + map[0].size + sizeof(map[0].size));
	map2->size = sizeof(multiboot_mmap_t);
	map2->base_addr = (uint64_t)(void*)phys_start;
	map2->len = len;
	map2->type = 1;
	
	mb->mmap_addr = (uint32_t)(uint64_t)(void*)map;
	
	assert((void*)(uint64_t)mb->mmap_addr == map);
	mb->mmap_length = 2*sizeof(multiboot_mmap_t);
	
	boot::mboot = new boot::MultiBoot(mb);
}

extern "C" void timer_callback(Registers_t);
extern "C" void task_switch()
{
	// Dummy function
}

static int time_counter = 0;

static void timer_fwd(int)
{
	//signal(QA::PIC_signal, timer_fwd);
	/*QA::CheckPIC();
	if (time_counter++ % 100 == 0)
	{
		QA::out << "time_counter: " << time_counter << std::endl;
	}
	assert(time_counter < 1000);
	assert(time_counter < 1000000);*/
	timer_callback(Registers_t());
	
}

static void timer_fwd(sigval_t)
{
	assert(false);
	timer_callback(Registers_t());
}

void QA::Timer_Init()
{
	sigset_t signal_toggle_set;
	sigemptyset(&signal_toggle_set);
	sigaddset(&signal_toggle_set, PIC_signal);
	
	if (sigprocmask(SIG_UNBLOCK, &signal_toggle_set, nullptr) != 0)
	{
		auto e = errno;
		out << strerror(e) << std::endl;
		assert(false);
	}
	
	::stack_t sigstack;
	
	if (sigaltstack(nullptr, &sigstack) != 0)
	{
		auto e = errno;
		out << strerror(e) << std::endl;
		assert(false);
	}
	
	//sigstack.ss_flags &= SS_DISABLE;
	//sigstack.ss_flags |= SS_AUTODISARM;
	
	if (sigaltstack(&sigstack, nullptr) != 0)
	{
		auto e = errno;
		out << strerror(e) << std::endl;
		assert(false);
	}
	
	
	struct sigevent event;
	
	event.sigev_notify = SIGEV_SIGNAL;
	//event.sigev_notify_function = timer_fwd;
	event.sigev_signo = PIC_signal;
	
	
	
	
	if (timer_create(CLOCK_THREAD_CPUTIME_ID, &event, &PIC_timer) != 0)
	{
		auto e = errno;
		out << strerror(e) << std::endl;
		assert(false);
	}
	
	PIC_config.it_interval.tv_sec = 0;
	PIC_config.it_interval.tv_nsec = 1000*1000;
	
	
	PIC_config.it_value.tv_sec = PIC_config.it_interval.tv_sec;
	PIC_config.it_value.tv_nsec = PIC_config.it_interval.tv_nsec + 2;
	
	
	if (timer_settime(PIC_timer, 0, &PIC_config, nullptr) != 0)
	{
		auto e = errno;
		out << strerror(e) << std::endl;
		assert(false);
	}
	
	
	
	struct sigaction oldAct, newAct;
	
	sigaction(PIC_signal, nullptr, &newAct);
	newAct.sa_handler = &timer_fwd;
	newAct.sa_flags |= SA_NODEFER;
	//newAct.sa_flags |= SA_ONSTACK;
	sigemptyset(&newAct.sa_mask);
	
	
	if (sigaction(PIC_signal, &newAct, &oldAct) != 0)
	{
		auto e = errno;
		out << strerror(e) << std::endl;
		assert(false);
	}
	
}

void QA::SetTimerInterval_ns(unsigned long long _nanoseconds)
{
	int seconds = _nanoseconds / (1000*1000*1000);
	int nanoseconds = _nanoseconds % (1000*1000*1000);
	
	
	PIC_config.it_interval.tv_sec = seconds;
	PIC_config.it_interval.tv_nsec = nanoseconds;
	
	if (timer_settime(PIC_timer, 0, &PIC_config, nullptr) != 0)
	{
		auto e = errno;
		out << strerror(e) << std::endl;
		assert(false);
	}
}

void QA::CheckPIC()
{
	/*sigset_t usr1set;
	sigemptyset(&usr1set);
	sigaddset(&usr1set, PIC_signal);
	sigprocmask(SIG_BLOCK, &usr1set, nullptr);
	
	auto oldAct = signal(SIGUSR1, SIG_IGN);
	
	if (oldAct == SIG_ERR)
	{
		auto e = errno;
		out << strerror(e) << std::endl;
		assert(false);
	}
	
	
	
		
	if (sigprocmask(SIG_UNBLOCK, &usr1set, nullptr) != 0)
	{
		auto e = errno;
		out << strerror(e) << std::endl;
		assert(false);
	}
	
	
	
	
	
	if (signal(SIGUSR1, oldAct) == SIG_ERR)
	{
		auto e = errno;
		out << strerror(e) << std::endl;
		assert(false);
	}
	
	auto hndl = signal(PIC_signal, SIG_IGN);
	assert(hndl == (decltype(hndl))&timer_fwd);
	assert(signal(PIC_signal, hndl) == SIG_IGN);*/
	
	//ASSERTEQ(timer_getoverrun(PIC_timer), 0);
}

#include <kernel/Filesystem/Filesystem.hh>
#include <kernel/Filesystem/initrd.hh>

void QA::Filesystem_Init()
{
	using namespace Kernel;
	using namespace Kernel::FS;
	
	// FS_INITRD_TEST_IMG defined
	// in Makefile
	FILE* file = fopen(FS_INITRD_TEST_IMG, "r");
	assert(file);
	
	fseek(file, 0, SEEK_END);
	size_t fsize = ftell(file);
	fseek(file, 0, SEEK_SET);
	
	auto buffer = new unsigned char[fsize];
	assert(buffer);
	fread(buffer, 1, fsize, file);
	fclose(file);
	
	
	auto initrd = new InitRD_FS(buffer);
	Filesystem::Current = initrd;
	
	
	
}

#include <kernel/Memory/Heaps/UnifyHeap.hh>

void QA::Paging_Init()
{
	using namespace Kernel::Memory;
	//PhysicalMemory::Use<Basic_Physical>();
	
	auto pd = new PageDirectory;
	pd->switch_to();
	kernel_dir = pd;
	
	assert(boot::mboot);
	size_t mmap_free;
	auto seg = boot::mboot->free_segments(mmap_free);
	assert(seg);
	addr_t last = 0x0;
	for (size_t i = 0; i < mmap_free; ++i)
	{
		assert(seg[i]->size == sizeof(multiboot_mmap_t));
		assert(seg[i]->type == 1);
		while (last < seg[i]->base_addr)
		{
			auto pg = pd->at((void*)last, true);
			if (!pg->present())
			{
				pg->present(true);
			}
			last += 4096;
		}
		assert(last % 4096 == 0);
		
		last = seg[i]->base_addr + seg[i]->len;
		last = (last / 4096) * 4096;
	}
	
	
	auto cr = new PageRegions::Dynamic_Code(PageDirectory::Current);
	PageDirectory::Regions[CODE_MEM_REGION] = cr;
	
	
	PhysicalMemory::Use<Basic_Physical>();
	
	extern Heap* kheap;
	kheap = nullptr;
	
	auto kheap_tmp = new UnifyHeap<cross_proc_allocator<void>>(PageDirectory::Current, false, false, 4096);
	kheap = kheap_tmp;
	
}

void QA::Processors_Init()
{
	auto proc = (Kernel::Processor_t*)malloc(sizeof(Kernel::Processor_t));
	assert(proc);
	memset(proc, 0, sizeof(Kernel::Processor_t));
	assert((addr_t)proc % alignof(Kernel::Processor_t) == 0);
	Kernel::current_processor = proc;
}

static size_t get_file_size(const char* filename)
{
	FILE* f = ::fopen(filename, "r");
	assert(f);
	size_t exist_size;
	fseek(f, 0, SEEK_END);
	exist_size = ftell(f);
	fclose(f);
	return exist_size;
}

Drivers::Disk* QA::QADrive(const char* filename, const size_t size)
{
	return new TestUtils::QADrive(filename, size);
}

Drivers::Disk* QA::QADrive(const char* filename)
{
	return QA::QADrive(filename, get_file_size(filename));
}

Drivers::Disk* QA::QACheckReadOnlyDrive(const char* filename, const size_t size)
{
	return new TestUtils::QACheckReadOnlyDrive(filename, size);
}

Drivers::Disk* QA::QACheckReadOnlyDrive(const char* filename)
{
	return QA::QACheckReadOnlyDrive(filename, get_file_size(filename));
}


void QA::EnablePrintAllocs()
{
	Memory::SetPrintAllocs(true);
}

void QA::DisablePrintAllocs()
{
	Memory::SetPrintAllocs(false);
}




int QA::type_func_prefix_count = -1;
int QA::type_func_suffix_count = -1;

std::ostream& QA::write_types_string(std::ostream& os, const char* fstring, size_t type_count)
{
	if (type_func_prefix_count == -1)
	{
		auto rstring = get_types_string<>();
		auto len = strlen(rstring);
		
		auto ptr = strstr(rstring, "Types");
		assert(ptr);
		ptr += 5;
		auto index = ptr - rstring;
		while (rstring[index] != '\0' && rstring[index] != '{')
		{
			++index;
		}
		assert(rstring[index] == '{');
		++index;
		type_func_prefix_count = index;
		
		while (rstring[index] != '}' && rstring[index] != '\0')
		{
			++index;
		}
		assert(rstring[index] == '}');
		type_func_suffix_count = len - index;
		
	}
	os.rdbuf()->sputn(fstring + type_func_prefix_count, strlen(fstring) - type_func_prefix_count - type_func_suffix_count);
	//os << fstring;
	return os;
}




std::ostream& QA::write_break(std::ostream& os)
{
	return (os << get_break());
}

std::ostream& QA::write_divider(std::ostream& os)
{
	return (os << get_divider());
}


std::ostream& QA::write_break()
{
	return write_break(out);
}
std::ostream& QA::write_divider()
{
	return write_divider(out);
}

const char* QA::get_break()
{
	return "\n\n\n";
}

const char* QA::get_divider()
{
	return "--------";
}

std::ostream& QA::split_sections(std::ostream& os)
{
	return (write_break(os) << get_divider() << get_break() << "\n");
}

std::ostream& QA::split_sections()
{
	return split_sections(out);
}

std::ostream& QA::split_sections(std::ostream& os, const char* name)
{
	return (write_break(os) << get_divider() << "\n" << name << "\n" << get_divider() << get_break() << "\n");
}

std::ostream& QA::split_sections(const char* name)
{
	return split_sections(out, name);
}