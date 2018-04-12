#ifdef __cplusplus
#ifndef INCLUDED_NEW_HH
#define INCLUDED_NEW_HH



inline void *operator new(size_t, void *p) noexcept
{ return p; }

inline void *operator new[](size_t, void *p) noexcept
{ return p; }


#endif
#endif