#ifndef ENUM_OPS

#define ENUM_OPS(E, T) \
constexpr inline bool operator<(const E r, const T n) \
{ return (T)r < n; } \
constexpr inline bool operator<=(const E r, const T n) \
{ return (T)r <= n; } \
constexpr inline bool operator>(const E r, const T n) \
{ return (T)r > n; } \
constexpr inline bool operator>=(const E r, const T n) \
{ return (T)r >= n; } \
constexpr inline bool operator==(const E r, const T n) \
{ return (T)r == n; } \
constexpr inline bool operator!=(const E r, const T n) \
{ return (T)r != n; } \
constexpr inline T operator&(const E r, const T n) \
{ return (T)r & n; } \
constexpr inline T operator|(const E r, const T n) \
{ return (T)r | n; } \
constexpr inline T operator+(const E r, const T n) \
{ return (T)r + n; } \
constexpr inline T operator-(const E r, const T n) \
{ return (T)r - n; } \
\
\
\
constexpr inline bool operator<(const T n, const E r) \
{ return n < (T)r; } \
constexpr inline bool operator<=(const T n, const E r) \
{ return n <= (T)r; } \
constexpr inline bool operator>(const T n, const E r) \
{ return n > (T)r; } \
constexpr inline bool operator>=(const T n, const E r) \
{ return n >= (T)r; } \
constexpr inline bool operator==(const T n, const E r) \
{ return n == (T)r; } \
constexpr inline bool operator!=(const T n, const E r) \
{ return n != (T)r; } \
constexpr inline T operator&(const T n, const E r) \
{ return n & (T)r; } \
constexpr inline T operator|(const T n, const E r) \
{ return n | (T)r; } \
constexpr inline T operator+(const T n, const E r) \
{ return n + (T)r; } \
constexpr inline T operator-(const T n, const E r) \
{ return n - (T)r; }



#endif