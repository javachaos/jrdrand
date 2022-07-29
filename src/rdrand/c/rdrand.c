/* Copyright � 2012, Intel Corporation.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

-       Redistributions of source code must retain the above copyright notice,
		this list of conditions and the following disclaimer.
-       Redistributions in binary form must reproduce the above copyright
		notice, this list of conditions and the following disclaimer in the
		documentation and/or other materials provided with the distribution.
-       Neither the name of Intel Corporation nor the names of its contributors
		may be used to endorse or promote products derived from this software
		without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY INTEL CORPORATION "AS IS" AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
EVENT SHALL INTEL CORPORATION BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

Modified: 2022 Alfred Laderoute
- added some support for linux
*/

#include "rdrand.h"
#include <jni.h>
#include <string.h>

#ifdef __linux__

#include <stdio.h>
#include <stdint.h>
#define DRNG_NO_SUPPORT	0x0	/* For clarity */
#define DRNG_HAS_RDRAND	0x1
#define DRNG_HAS_RDSEED	0x2
#define RDRAND_RETRIES 33

/**
 * Majority of code is from:
 *   https://www.intel.com/content/www/us/en/developer/articles/guide/intel-digital-random-number-generator-drng-software-implementation-guide.html
 */

typedef struct cpuid_struct {
	unsigned int eax;
	unsigned int ebx;
	unsigned int ecx;
	unsigned int edx;
} cpuid_t;

void cpuid (cpuid_t *info, unsigned int leaf, unsigned int subleaf)
{
	asm volatile("cpuid"
	: "=a" (info->eax), "=b" (info->ebx), "=c" (info->ecx), "=d" (info->edx)
	: "a" (leaf), "c" (subleaf)
	);
}

int _is_intel_cpu ()
{
	static int intel_cpu= -1;
	cpuid_t info;

	if ( intel_cpu == -1 ) {
		cpuid(&info, 0, 0);

		if (
			memcmp((char *) &info.ebx, "Genu", 4) ||
			memcmp((char *) &info.edx, "ineI", 4) ||
			memcmp((char *) &info.ecx, "ntel", 4)
		) {
			intel_cpu= 0;
		} else {
			intel_cpu= 1;
		}
	}

	return intel_cpu;
}



int get_drng_support ()
{
	static int drng_features= -1;

	/* So we don't call cpuid multiple times for
	 * the same information */

	if ( drng_features == -1 ) {
		drng_features= DRNG_NO_SUPPORT;

		if ( _is_intel_cpu() ) {
			cpuid_t info;

			cpuid(&info, 1, 0);

			if ( (info.ecx & 0x40000000) == 0x40000000 ) {
				drng_features|= DRNG_HAS_RDRAND;
			}

			cpuid(&info, 7, 0);

			if ( (info.ebx & 0x40000) == 0x40000 ) {
				drng_features|= DRNG_HAS_RDSEED;
			}
		}
	}

	return drng_features;
}

int rdrand16_step (uint16_t *rand)
{
	unsigned char ok;

	asm volatile ("rdrand %0; setc %1"
		: "=r" (*rand), "=qm" (ok));

	return (int) ok;
}

int rdrand32_step (uint32_t *rand)
{
	unsigned char ok;

	asm volatile ("rdrand %0; setc %1"
		: "=r" (*rand), "=qm" (ok));

	return (int) ok;
}

int rdrand64_step (uint64_t *rand)
{
	unsigned char ok;

	asm volatile ("rdrand %0; setc %1"
		: "=r" (*rand), "=qm" (ok));

	return (int) ok;
}

int rdrand16_retry (unsigned int retries, uint16_t *rand)
{
	unsigned int count= 0;

	while ( count <= retries ) {
		if ( rdrand16_step(rand) ) {
			return 1;
		}

		++count;
	}

	return 0;
}

int rdrand32_retry (unsigned int retries, uint32_t *rand)
{
	unsigned int count= 0;

	while ( count <= retries ) {
		if ( rdrand32_step(rand) ) {
			return 1;
		}

		++count;
	}

	return 0;
}

int rdrand64_retry (unsigned int retries, uint64_t *rand)
{
	unsigned int count= 0;

	while ( count <= retries ) {
		if ( rdrand64_step(rand) ) {
			return 1;
		}

		++count;
	}

	return 0;
}

unsigned int rdrand_get_n_uints (unsigned int n, unsigned int *dest)
{
	unsigned int i;
	uint32_t *lptr= (uint32_t *) dest;

	for (i= 0; i< n; ++i, ++dest) {
		if ( ! rdrand32_step(dest) ) {
			return i;
		}
	}

	return n;
}

unsigned int rdrand_get_bytes (unsigned int n, unsigned char *dest)
{
	unsigned char *headstart, *tailstart;
	uint64_t *blockstart;
	unsigned int count, ltail, lhead, lblock;
	uint64_t i, temprand;

	/* Get the address of the first 64-bit aligned block in the
	 * destination buffer. */

	headstart= dest;
	if ( ( (uint64_t)headstart % (uint64_t)8 ) == 0 ) {

		blockstart= (uint64_t *)headstart;
		lblock= n;
		lhead= 0;
	} else {
		blockstart= (uint64_t *)
			( ((uint64_t)headstart & ~(uint64_t)7) + (uint64_t)8 );

		lblock= n - (8 - (unsigned int) ( (uint64_t)headstart & (uint64_t)8 ));

		lhead= (unsigned int) ( (uint64_t)blockstart - (uint64_t)headstart );
	}

	/* Compute the number of 64-bit blocks and the remaining number
	 * of bytes (the tail) */

	ltail= n-lblock-lhead;
	count= lblock/8;	/* The number 64-bit rands needed */

	if ( ltail ) {
		tailstart= (unsigned char *)( (uint64_t) blockstart + (uint64_t) lblock );
	}

	/* Populate the starting, mis-aligned section (the head) */

	if ( lhead ) {
		if ( ! rdrand64_retry(RDRAND_RETRIES, &temprand) ) {
			return 0;
		}

		memcpy(headstart, &temprand, lhead);
	}

	/* Populate the central, aligned block */

	for (i= 0; i< count; ++i, ++blockstart) {
		if ( ! rdrand64_retry(RDRAND_RETRIES, blockstart) ) {
			return i*8+lhead;
		}
	}

	/* Populate the tail */

	if ( ltail ) {
		if ( ! rdrand64_retry(RDRAND_RETRIES, &temprand) ) {
			return count*8+lhead;
		}

		memcpy(tailstart, &temprand, ltail);
	}

	return n;
}

int rdseed16_step (uint16_t *seed)
{
	unsigned char ok;

	asm volatile ("rdseed %0; setc %1"
		: "=r" (*seed), "=qm" (ok));

	return (int) ok;
}

int rdseed32_step (uint32_t *seed)
{
	unsigned char ok;

	asm volatile ("rdseed %0; setc %1"
		: "=r" (*seed), "=qm" (ok));

	return (int) ok;
}

int rdseed64_step (uint64_t *seed)
{
	unsigned char ok;

	asm volatile ("rdseed %0; setc %1"
		: "=r" (*seed), "=qm" (ok));

	return (int) ok;
}

int rdseed64_slow(uint64_t *seed) {
    unsigned char ok;
    while (! (ok = rdseed64_step(seed))) {
        asm volatile("pause");
    }
    return (int) ok;
}

JNIEXPORT jlong JNICALL
Java_jRdRand_rdrand(JNIEnv *env, jobject obj, jlong seed)// *** Caller must check for 0 ***
{
    if (_is_intel_cpu() && get_drng_support()) {
        uint64_t s = seed;
        unsigned char ok = rdrand64_retry (RDRAND_RETRIES, &s);
        if (ok) {
            return s;
        } else {
            return (long) ok;
        }
    } else {
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_jRdRand_rdseed(JNIEnv *env, jobject obj) // *** Caller must check for zero ***
{
    if (_is_intel_cpu() && get_drng_support()) {
        uint64_t seed;
        rdseed64_slow(&seed);
        return (long) seed;
    } else {
        return 0;
    }
}

JNI_OnLoad_rdrand(JavaVM *vm, void *reserved)
{
    return JNI_VERSION_1_8;
}

#elif (_WIN64 || _WIN32)
#include <immintrin.h>

/*! \def RDRAND_MASK
*    The bit mask used to examine the ecx register returned by cpuid. The
 *   30th bit is set.
 */
#define RDRAND_MASK	0x40000000

#define RETRY_LIMIT 10

#ifdef _WIN64
typedef uint64_t _wordlen_t;
#else
typedef uint32_t _wordlen_t;
#endif

/*! \brief Queries cpuid to see if rdrand is supported
 *
 * rdrand support in a CPU is determined by examining the 30th bit of the ecx
 * register after calling cpuid.
 *
 * \return bool of whether or not rdrand is supported
 */
int RdRand_cpuid()
{
	int info[4] = {-1, -1, -1, -1};

	/* Are we on an Intel processor? */

	__cpuid(info, /*feature bits*/0);

	if ( memcmp((void *) &info[1], (void *) "Genu", 4) != 0 ||
		memcmp((void *) &info[3], (void *) "ineI", 4) != 0 ||
		memcmp((void *) &info[2], (void *) "ntel", 4) != 0 ) {

		return 0;
	}

	/* Do we have RDRAND? */

	 __cpuid(info, /*feature bits*/1);

	 int ecx = info[2];
	 if ((ecx & RDRAND_MASK) == RDRAND_MASK)
		 return 1;
	 else
		 return 0;
}

/*! \brief Determines whether or not rdrand is supported by the CPU
 *
 * This function simply serves as a cache of the result provided by cpuid,
 * since calling cpuid is so expensive. The result is stored in a static
 * variable to save from calling cpuid on each invocation of rdrand.
 *
 * \return bool/int of whether or not rdrand is supported
 */
int RdRand_isSupported()
{
	static int supported = RDRAND_SUPPORT_UNKNOWN;

	if (supported == RDRAND_SUPPORT_UNKNOWN)
	{
		if (RdRand_cpuid())
			supported = RDRAND_SUPPORTED;
		else
			supported = RDRAND_UNSUPPORTED;
	}

	return (supported == RDRAND_SUPPORTED) ? 1 : 0;
}

int rdrand_16(uint16_t* x, int retry)
{
	if (RdRand_isSupported())
	{
		if (retry)
		{
			for (int i = 0; i < RETRY_LIMIT; i++)
			{
				if (_rdrand16_step(x))
					return RDRAND_SUCCESS;
			}

			return RDRAND_NOT_READY;
		}
		else
		{
				if (_rdrand16_step(x))
					return RDRAND_SUCCESS;
				else
					return RDRAND_NOT_READY;
		}
	}
	else
	{
		return RDRAND_UNSUPPORTED;
	}
}

int rdrand_32(uint32_t* x, int retry)
{
	if (RdRand_isSupported())
	{
		if (retry)
		{
			for (int i = 0; i < RETRY_LIMIT; i++)
			{
				if (_rdrand32_step(x))
					return RDRAND_SUCCESS;
			}

			return RDRAND_NOT_READY;
		}
		else
		{
				if (_rdrand32_step(x))
					return RDRAND_SUCCESS;
				else
					return RDRAND_NOT_READY;
		}
	}
	else
	{
		return RDRAND_UNSUPPORTED;
	}
}

int rdrand_64(uint64_t* x, int retry)
{
	if (RdRand_isSupported())
	{
		if (retry)
		{
			for (int i = 0; i < RETRY_LIMIT; i++)
			{
				if (_rdrand64_step(x))
					return RDRAND_SUCCESS;
			}

			return RDRAND_NOT_READY;
		}
		else
		{
				if (_rdrand64_step(x))
					return RDRAND_SUCCESS;
				else
					return RDRAND_NOT_READY;
		}
	}
	else
	{
		return RDRAND_UNSUPPORTED;
	}
}

int rdrand_get_n_64(unsigned int n, uint64_t *dest)
{
	int success;
	int count;
	unsigned int i;

	for (i=0; i<n; i++)
	{
		count = 0;
		do
		{
        		success= rdrand_64(dest, 1);
		} while((success == 0) && (count++ < RETRY_LIMIT));
		if (success != RDRAND_SUCCESS) return success;
		dest= &(dest[1]);
	}
	return RDRAND_SUCCESS;
}

int rdrand_get_n_32(unsigned int n, uint32_t *dest)
{
	int success;
	int count;
	unsigned int i;

	for (i=0; i<n; i++)
	{
		count = 0;
		do
		{
        		success= rdrand_32(dest, 1);
		} while((success == 0) && (count++ < RETRY_LIMIT));
		if (success != RDRAND_SUCCESS) return success;
		dest= &(dest[1]);
	}
	return RDRAND_SUCCESS;
}

int rdrand_get_bytes(unsigned int n, unsigned char *dest)
{
	unsigned char *start;
	unsigned char *residualstart;
	_wordlen_t *blockstart;
	_wordlen_t i, temprand;
	unsigned int count;
	unsigned int residual;
	unsigned int startlen;
	unsigned int length;
	int success;

	/* Compute the address of the first 32- or 64- bit aligned block in the destination buffer, depending on whether we are in 32- or 64-bit mode */
	start = dest;
	if (((uint32_t)start % (uint32_t) sizeof(_wordlen_t)) == 0)
	{
		blockstart = (_wordlen_t *)start;
		count = n;
		startlen = 0;
	}
	else
	{
		blockstart = (_wordlen_t *)(((_wordlen_t)start & ~(_wordlen_t) (sizeof(_wordlen_t)-1) )+(_wordlen_t)sizeof(_wordlen_t));
		count = n - (sizeof(_wordlen_t) - (unsigned int)((_wordlen_t)start % sizeof(_wordlen_t)));
		startlen = (unsigned int)((_wordlen_t)blockstart - (_wordlen_t)start);
	}

	/* Compute the number of 32- or 64- bit blocks and the remaining number of bytes */
	residual = count % sizeof(_wordlen_t);
	length = count/sizeof(_wordlen_t);
	if (residual != 0)
	{
		residualstart = (unsigned char *)(blockstart + length);
	}

	/* Get a temporary random number for use in the residuals. Failout if retry fails */
	if (startlen > 0)
	{
#ifdef _WIN64
		if ( (success= rdrand_64((uint64_t *) &temprand, 1)) != RDRAND_SUCCESS) return success;
#else
		if ( (success= rdrand_32((uint32_t *) &temprand, 1)) != RDRAND_SUCCESS) return success;
#endif
	}

	/* populate the starting misaligned block */
	for (i = 0; i<startlen; i++)
	{
		start[i] = (unsigned char)(temprand & 0xff);
		temprand = temprand >> 8;
	}

	/* populate the central aligned block. Fail out if retry fails */

#ifdef _WIN64
	if ( (success= rdrand_get_n_64(length, (uint64_t *)(blockstart))) != RDRAND_SUCCESS) return success;
#else
	if ( (success= rdrand_get_n_32(length, (uint32_t *)(blockstart))) != RDRAND_SUCCESS) return success;
#endif
	/* populate the final misaligned block */
	if (residual > 0)
	{
#ifdef _WIN64
		if ((success= rdrand_64((uint64_t *)&temprand, 1)) != RDRAND_SUCCESS) return success;
#else
		if ((success= rdrand_32((uint32_t *)&temprand, 1)) != RDRAND_SUCCESS) return success;
#endif

		for (i = 0; i<residual; i++)
		{
			residualstart[i] = (unsigned char)(temprand & 0xff);
			temprand = temprand >> 8;
		}
	}

    return RDRAND_SUCCESS;
}

int rdseed_64(uint64_t* x, int retry)
{
	if (RdRand_isSupported())
	{
		if (retry)
		{
			for (int i = 0; i < RETRY_LIMIT; i++)
			{
				if (_rdseed64_step(x))
					return RDRAND_SUCCESS;
			}

			return RDRAND_NOT_READY;
		}
		else
		{
				if (_rdseed64_step(x))
					return RDRAND_SUCCESS;
				else
					return RDRAND_NOT_READY;
		}
	}
	else
	{
		return RDRAND_UNSUPPORTED;
	}
}

JNI_OnLoad_rdrand(JavaVM *vm, void *reserved)
{
    return JNI_VERSION_1_8;
}

JNIEXPORT jlong JNICALL
Java_jRdRand_rdrand(JNIEnv *env, jobject obj, jlong seed)// *** Caller must check for 0 ***
{
        uint64_t s = seed;
        unsigned char ok = rdrand_64(&s, 33);
        if (ok) {
            return (uint64_t) s;
        } else {
            return ok;
        }
}

JNIEXPORT jlong JNICALL
Java_jRdRand_rdseed(JNIEnv *env, jobject obj) // *** Caller must check for zero ***
{
    uint64_t seed;
    rdseed_64(&seed, 33);
    return (uint64_t) seed;
}
#else
#endif