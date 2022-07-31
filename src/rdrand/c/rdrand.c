#include "rdrand.h"

inline uint64_t rdrand64()
{
    intrin_u64 r;
    do { _mm_pause(); } while( !_rdrand64_step(&r) );
    return r;
}

inline uint64_t rdseed64()
{
    intrin_u64 r;
    do { _mm_pause(); } while( !_rdseed64_step(&r) );
    return r;
}

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