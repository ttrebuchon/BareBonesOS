
#ifdef __cplusplus
extern "C" {
#endif

void __stack_info_func(const char* file, const int line, const char* func, const char* msg);

#ifdef __cplusplus
}
#endif

#ifdef STACK
#undef STACK
#endif

#ifndef STACK_INFO_V
#define STACK(X) 
#else
#define STACK(X) do { __stack_info_func(__FILE__, __LINE__, __func__, #X); } while (false)
#endif

#ifndef INCLUDED_METAINFO_H
#define INCLUDED_METAINFO_H

#endif