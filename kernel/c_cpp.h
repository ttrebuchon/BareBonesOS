#ifndef INCLUDED_C_CPP_H
#define INCLUDED_C_CPP_H


#ifdef __cplusplus

#define CPP_STATIC static
#define EXTERN_C extern "C"
#define START_NS(X) namespace X {
#define END_NS }

#else


#define CPP_STATIC
#define EXTERN_C 
#define START_NS(X) 
#define END_NS

#endif





#endif