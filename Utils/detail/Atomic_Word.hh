#ifndef INCLUDED_ATOMIC_WORD_HH
#define INCLUDED_ATOMIC_WORD_HH



namespace Utils
{
	typedef int __Atomic_word;
	
	#define _GLIBCXX_READ_MEM_BARRIER __asm __volatile (""::::"memory")
	
	#define _GLIBCXX_WRITE_MEM_BARRIER __asm __volatile (""::::"memory")
}

#endif