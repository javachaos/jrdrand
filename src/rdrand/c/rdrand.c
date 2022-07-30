#include <stdint.h>
#include <jni.h>
#ifdef __GNUC__
#include <x86intrin.h>
#elif __MSC_VER__
#include <immintrin.h>
#endif

#if defined(__x86_64__) || defined (_M_X64)
#ifdef __INTEL_COMPILER
typedef uint64_t intrin_u64;
#else
typedef unsigned long long intrin_u64;
#endif
uint64_t rdrand64()
{
    intrin_u64 r;
    do { _mm_pause(); } while( !_rdrand64_step(&r) );
    return r;
}

uint64_t rdseed64()
{
    intrin_u64 r;
    do { _mm_pause(); } while( !_rdseed64_step(&r) );
    return r;
}
#endif

void JNI_OnLoad_rdrand(JavaVM *vm, void *reserved)
{
    return JNI_VERSION_1_8;
}

JNIEXPORT jlong JNICALL
Java_com_github_javachaos_jrdrand_RdRand_rdrand(JNIEnv *env, jobject obj)
{
    return rdrand64();
}

JNIEXPORT jlong JNICALL
Java_com_github_javachaos_jrdrand_RdRand_rdseed(JNIEnv *env, jobject obj)
{
    return rdseed64();
}