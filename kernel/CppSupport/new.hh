#ifdef __cplusplus
#ifndef INCLUDED_NEW_HH
#define INCLUDED_NEW_HH


#ifndef __USE_STD_NEW__
inline void *operator new(size_t, void *p) noexcept
{ return p; }

inline void *operator new[](size_t, void *p) noexcept
{ return p; }
#else
#include <new>
#endif


#endif
#endif