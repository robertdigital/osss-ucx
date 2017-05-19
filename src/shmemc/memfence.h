#ifndef _MEMFENCE_H
#define _MEMFENCE_H 1

/*
 * How do various compilers handle memory barriers and intrinsics?
 *
 * Some guesswork here...
 *
 */

#if defined(__INTEL_COMPILER)
/*
 * icc and friends
 */

# define LOAD_STORE_FENCE __memory_barrier

#elif defined(__SUNPRO_C)
/*
 * Sun/Oracle Studio
 */

# include <mbarrier.h>

# define LOAD_STORE_FENCE __machine_rw_barrier

#elif defined(__PGI)
/*
 * Portland Group (PGI)
 */
# include <emmintrin.h>
# define LOAD_STORE_FENCE _mm_mfence

#elif defined(__GNUC__)
/*
 * GCC
 */

# define LOAD_STORE_FENCE __sync_synchronize

#elif defined(__xlc__)
/*
 * IBM XL
 */

# define LOAD_STORE_FENCE __lwsync

#else

# error "I don't know how to do memory fences here"

#endif /* choose appropriate memory fence */

#endif /* ! _MEMFENCE_H */
