#include <immintrin.h>
#include <stdint.h>
#include <jni.h>

#if defined(__x86_64__) || defined (_M_X64)
#ifdef __INTEL_COMPILER
typedef uint64_t intrin_u64;
#else
typedef unsigned long long intrin_u64;
#endif
#if defined(__RDRND__) || defined(_MSC_VER)
uint64_t rdrand64()
{
    intrin_u64 r;
    do {} while( !_rdrand64_step(&r) );
    return r;
}
#endif

#if defined(__RDSEED__) || defined(_MSC_VER)
uint64_t rdseed64()
{
    intrin_u64 r;
    do {} while( !_rdseed64_step(&r) );
    return r;
}
#endif
#endif

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