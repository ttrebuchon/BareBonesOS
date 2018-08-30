#ifndef INCLUDED_SUPPORT_PP_H
#define INCLUDED_SUPPORT_PP_H

#define PP_ARG_3(X, Y, Z, ...) Z
#define PP_ARG_2(X, Y, Z, ...) Y
#define PP_ARG_1(X, Y, Z, ...) X

#define VA_OPT_SUPPORTED_HELPER(...) PP_ARG_3(__VA_OPT__(,),1,0,)
#define VA_OPT_SUPPORTED VA_OPT_SUPPORTED_HELPER(?)

#define P00_ISEMPTY(_0, _1, _2, _3) \
P99_HAS_COMMA(P99_PASTE5(P00_IS_EMPTY_CASE_, _0, _1, _2, _3))



#define _ARG16(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, ...) _15

#define HAS_COMMA(...) _ARG16(__VA_ARGS__, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0)

#define _TRIGGER_PARENTHESIS_(...) ,



#define PP_IS_EMPTY(...) \
_ISEMPTY( \
HAS_COMMA(__VA_ARGS__), \
HAS_COMMA(_TRIGGER_PARENTHESIS_ __VA_ARGS__), \
HAS_COMMA(__VA_ARGS__ (/*empty*/)), \
HAS_COMMA(_TRIGGER_PARENTHESIS_ __VA_ARGS__ (/*empty*/)) \
)



#define PASTE5(_0, _1, _2, _3, _4) _0 ## _1 ## _2 ## _3 ## _4

#define _ISEMPTY(_0, _1, _2, _3) HAS_COMMA(PASTE5(_IS_EMPTY_CASE_, _0, _1, _2, _3))

#define _IS_EMPTY_CASE_0001 ,

#define __PP_INTERNAL_ID(...) __VA_ARGS__
#define __PP_INTERNAL_EAT(...) 

#define __PP_INTERNAL_IF_1(...) __VA_ARGS__ __PP_INTERNAL_EAT
#define __PP_INTERNAL_IF_0(...) __PP_INTERNAL_ID


#define PP_IF_EQ_0(N) __PP_INTERNAL_IF_##N


#define PP_IF_ELSE(...) PP_IF_EQ_0(__VA_ARGS__)

#define PP_IF_ARGS(T, ...) PP_IF_ELSE(PP_IS_EMPTY(__VA_ARGS__))()(T)









#endif // INCLUDED_SUPPORT_PP_H