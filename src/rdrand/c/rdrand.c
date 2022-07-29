#include <immintrin.h>
#include <stdint.h>
#include <jni.h>

#if defined(__x86_64__) || defined (_M_X64)
// Figure out which 64-bit type the output arg uses
#ifdef __INTEL_COMPILER       // Intel declares the output arg type differently from everyone(?) else
// ICC for Linux declares rdrand's output as unsigned long, but must be long long for a Windows ABI
typedef uint64_t intrin_u64;
#else
// GCC/clang headers declare it as unsigned long long even for Linux where long is 64-bit, but uint64_t is unsigned long and not compatible
typedef unsigned long long intrin_u64;
#endif

//#if defined(__RDRND__) || defined(_MSC_VER)  // conditional definition if you want
inline
uint64_t rdrand64() {
    intrin_u64 ret;
    do{}while( !_rdrand64_step(&ret) );  // retry until success.
    return ret;
}
//#endif

#if defined(__RDSEED__) || defined(_MSC_VER)
inline
uint64_t rdseed64() {
    intrin_u64 ret;
    do{}while( !_rdseed64_step(&ret) );   // retry until success.
    return ret;
}
#endif  // RDSEED
#endif  // x86-64

JNI_OnLoad_rdrand(JavaVM *vm, void *reserved)
{
    return JNI_VERSION_1_8;
}

JNIEXPORT jlong JNICALL
Java_RdRand_rdrand(JNIEnv *env, jobject obj)// *** Caller must check for 0 ***
{
    return rdrand64();
}

JNIEXPORT jlong JNICALL
Java_RdRand_rdseed(JNIEnv *env, jobject obj) // *** Caller must check for zero ***
{
    return rdseed64();
}