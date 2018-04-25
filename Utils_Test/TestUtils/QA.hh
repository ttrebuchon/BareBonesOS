#pragma once
#include <iostream>
#include <Common.h>

class QA
{
	private:
	static void Out_Init();
	static void MultiBoot_Init();
	
	static void* phys_start;
	static void* phys_end;
	
	protected:
	
	class _ostream;
	
	
	
	public:
	static std::ostream out;
	
	class Memory;
	
	static void Init()
	{
		Out_Init();
		MultiBoot_Init();
	}
	
	static const void* physical_start() noexcept
	{
		return phys_start;
	}
	
	static const void* physical_end() noexcept
	{
		return phys_end;
	}
};