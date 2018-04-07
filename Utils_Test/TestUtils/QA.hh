#pragma once
#include <iostream>


class QA
{
	private:
	static void Out_Init();
	
	protected:
	
	class _ostream;
	
	
	
	public:
	static std::ostream out;
	
	class Memory;
	
	static void Init()
	{
		Out_Init();
	}
};