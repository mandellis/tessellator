#ifndef MACRO_H
#define MACRO_H

typedef double real;

#include <functional>

//! -----------------------
//! function: hash_combine
//! -----------------------
template <class T>
inline void hash_combine(std::size_t & seed, const T & v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

#endif // MACRO_H
