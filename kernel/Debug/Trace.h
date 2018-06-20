#ifndef INCLUDED_KERNEL_DEBUG_H
#define INCLUDED_KERNEL_DEBUG_H

#include <kernel/c_cpp.h>
#include <drivers/VGA.hh>

#ifdef __cplusplus

	#ifndef INCLUDED_OSTREAM_HH_IN_PROGRESS
	#include <Utils/iostream>
	#endif
	#include <kernel/Initialize.hh>


	#define __do_trace(F, L, FN, M) \
	do \
	{ \
		if (!::stdlib_initialized()) \
		{ \
			::Drivers::VGA::Write(F); \
			::Drivers::VGA::Write("::"); \
			::Drivers::VGA::Write(L); \
			::Drivers::VGA::Write("::"); \
			::Drivers::VGA::Write(FN); \
			::Drivers::VGA::Write("::"); \
			::Drivers::VGA::Write(M); \
			::Drivers::VGA::Write("\n"); \
		} \
		else \
		{ \
			std::cout << F << "::" << L << "::" << FN << "::" << M << std::endl; \
		} \
	} while (false)


	// template <class T>
	// void __do_trace(const char* file, int32_t line, const char* function, T msg)
	// {
	// 	if (!::stdlib_initialized())
	// 	{
	// 		::Drivers::VGA::Write(file);
	// 		::Drivers::VGA::Write("::");
	// 		::Drivers::VGA::Write(line);
	// 		::Drivers::VGA::Write("::");
	// 		::Drivers::VGA::Write(function);
	// 		::Drivers::VGA::Write("::");
	// 		::Drivers::VGA::Write(msg);
	// 		::Drivers::VGA::Write("\n");
	// 	}
	// 	else
	// 	{
	// 		std::cout << file << "::" << line << "::" << function << "::" << msg << std::endl;
	// 	}
	// }

#else

	inline void __do_trace(const char* file, int32_t line, const char* function, const char* msg)
	{
		c_vga_write(file);
		c_vga_write("::");
		c_vga_write_int(line);
		c_vga_write("::");
		c_vga_write(function);
		c_vga_write("::");
		c_vga_write(msg);
		c_vga_write("\n");
	}

#endif

#endif