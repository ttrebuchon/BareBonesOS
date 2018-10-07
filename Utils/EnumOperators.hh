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
constexpr inline E operator&(const E r, const T n) \
{ return (E)((T)r & n); } \
constexpr inline E operator|(const E r, const T n) \
{ return (E)((T)r | n); } \
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
{ return n - (T)r; } \
\
\
\
\
constexpr inline bool operator<(const E n, const E r) \
{ return (T)n < (T)r; } \
constexpr inline bool operator<=(const E n, const E r) \
{ return (T)n <= (T)r; } \
constexpr inline bool operator>(const E n, const E r) \
{ return (T)n > (T)r; } \
constexpr inline bool operator>=(const E n, const E r) \
{ return (T)n >= (T)r; } \
constexpr inline E operator&(const E n, const E r) \
{ return (E)((T)n & (T)r); } \
constexpr inline E operator|(const E n, const E r) \
{ return (E)((T)n | (T)r); } \
constexpr inline E& operator&=(E& n, const E r) \
{ return n = (E)((T)n & (T)r); } \
constexpr inline E& operator|=(E& n, const E r) \
{ return n = (E)((T)n | (T)r); } \
\
\
\
constexpr inline E operator~(const E e) noexcept \
{ return (E)~((T)e); }



#endif

#ifndef HAS_FLAG

#define HAS_FLAG(V, F) (!(((V) & (F)) != (F)))

#endif