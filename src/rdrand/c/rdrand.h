#include <stdint.h>
#include <jni.h>
#ifdef __GNUC__
#include <x86intrin.h>
#elif __MSC_VER__
inline intrin_u64 rdrand64();
inline intrin_u64 rdseed64();
#include <immintrin.h>
#else
#include <x86intrin.h>
#endif
#if defined(__x86_64__) || defined (_M_X64)
#ifdef __INTEL_COMPILER
typedef uint64_t intrin_u64;
#else
typedef unsigned long long intrin_u64;
#endif
#endif

