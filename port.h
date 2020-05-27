#pragma once

// Collection of platform-dependent routines

#include <cinttypes>

typedef uint8_t u1;
typedef uint16_t u2;
typedef uint32_t u4;
typedef int8_t i1;
typedef int16_t i2;
typedef int32_t i4;
typedef int64_t i8;

const int N=8;
const int NN=N*N;

typedef int16_t CValueCompact;
typedef int32_t CValue;

typedef uint64_t u64;
typedef int64_t i64;
typedef uint32_t u32;


#if __GNUC__ >= 4 && defined(__x86_64__)
#include <xmmintrin.h>
#include <smmintrin.h>
#elif defined(_WIN32)
#include <nmmintrin.h>
#endif


#ifdef _WIN32
#include <intrin.h>
#else
#include <unistd.h>
#endif

#include <string>

#include "port.h"

inline int bit(int index, u64 bits) {
    // using _bittest64 seems slightly slower (2% endgame, same midgame),
    // could be random timing variation but not going to use it
    return ((bits>>index)&1);
}

inline bool bitClear(int index, u64 bits) {
    return bit(index, bits)==0;
}

inline bool bitSet(int index, u64 bits) {
    return bit(index, bits)!=0;
}

/**
 * low order 32 bits
 */
inline u32 low32(u64 n) {
    return u32(n);
}

/**
 * hi order 32 bits
 */
inline u32 hi32(u64 n) {
    return u32(n>>32);
}

/**
* Count number of 1 bits
* 
* This is tuned for 64-bit implementations but needs to work under 32-bit for testing.
* @param bits bitboard
* @return number of '1' bits in the bitboard.
*/
inline u64 bitCount(u64 bits) {
#if __GNUC__ >= 4
    return __builtin_popcountll(bits);
#elif defined(_WIN32)
#ifdef _M_AMD64
    return __popcnt64(bits);
#else
    return __popcnt(u32(bits)) + __popcnt(u32(bits>>32));
#endif
#else 
#error "Unknown compiler"
#endif
}

inline void storeLowBitIndex(unsigned long& result, u64 bits) {
#if __GNUC__ >= 4
    result = bits? __builtin_ctzll(bits) : 0;
#elif defined(_WIN32)
#ifdef _M_AMD64
    _BitScanForward64(&result, bits);
#else 
    if (!_BitScanForward(&result, low32(bits))) {
    	_BitScanForward(&result, hi32(bits));
    	result+=32;
    }
#endif
#else
#error "Unknown compiler"
#endif
}

inline int bitCountInt(u64 bits) {
    return int(bitCount(bits));
}

inline unsigned long lowBitIndex(u64 bits) {
    unsigned long result;
    storeLowBitIndex(result, bits);
    return result;
}

inline unsigned long popLowBit(u64& bits) {
    unsigned long result;
    storeLowBitIndex(result, bits);
    bits&=bits-1;
    return result;
}

inline int square(int row, int col) {
    return (row<<3)+col;
}

inline int col(int square) {
    return square & 7;
}

inline int row(int square) {
    return square>>3;
}

inline u64 mask(int square) {
    return 1ULL<<square;
}

inline u64 mask(int row, int col) {
    return mask(square(row, col));
}

inline u64 flipVertical(u64 a) {
#if __GNUC__ >= 4
    return __builtin_bswap64(a);
#elif defined(_WIN32)
    return _byteswap_uint64(a);
#else
#error "Unknown compiler"
#endif
}
