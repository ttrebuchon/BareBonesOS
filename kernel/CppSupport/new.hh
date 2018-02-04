#ifndef INCLUDED_NEW_HH
#define INCLUDED_NEW_HH

#ifdef __cplusplus

inline void *operator new(size_t, void *p)     throw() { return p; }



#endif
#endif