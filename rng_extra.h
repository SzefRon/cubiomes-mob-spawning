#ifndef RNG_EXTRA_H_
#define RNG_EXTRA_H_

#include "md5.h"
#include "rng.h"

STRUCT(XoroHash)
{
    uint64_t hi_hash, lo_hash;
};

static inline uint64_t xMixStafford13(uint64_t seed)
{
    seed = (seed ^ (uint64_t)seed >> 30) * -4658895280553007687LL;
    seed = (seed ^ (uint64_t)seed >> 27) * -7723592293110705685LL;
    return seed ^ (uint64_t)seed >> 31;
}

static inline void xSetSeedHashed(Xoroshiro *xr, XoroHash *hash, uint64_t value)
{
    long long l2 = value ^ 0x6A09E667F3BCC909LL;
    long long l3 = l2 + -7046029254386353131LL;

    xr->lo = xMixStafford13(l2 ^ hash->lo_hash);
    xr->hi = xMixStafford13(l3 ^ hash->hi_hash);

    if ((xr->lo | xr->hi) == 0L) {
        xr->lo = -7046029254386353131LL;
        xr->hi = 7640891576956012809LL;
    }
}

static inline uint64_t fromBytes(unsigned char b1, unsigned char b2, unsigned char b3, unsigned char b4, 
                                 unsigned char b5, unsigned char b6, unsigned char b7, unsigned char b8)
{
    return ((uint64_t)b1) << 56 | 
           ((uint64_t)b2) << 48 | 
           ((uint64_t)b3) << 40 | 
           ((uint64_t)b4) << 32 | 
           ((uint64_t)b5) << 24 | 
           ((uint64_t)b6) << 16 | 
           ((uint64_t)b7) << 8  | 
           ((uint64_t)b8);
}

// construct hash from a resource
static inline XoroHash constructHash(char *str)
{
    XoroHash hash;

    uint8_t r[16];
    md5String(str, r);

    hash.lo_hash = fromBytes(r[0], r[1], r[2], r[3], r[4], r[5], r[6], r[7]);
    hash.hi_hash = fromBytes(r[8], r[9], r[10], r[11], r[12], r[13], r[14], r[15]);

    return hash;
}

static inline Xoroshiro constructFromWSeed(uint64_t world_seed)
{
    Xoroshiro rand;
    uint64_t l2 = world_seed ^ 7640891576956012809ULL;
    uint64_t l3 = l2 + 11400714819323198485ULL;

    rand.lo = xMixStafford13(l2);
    rand.hi = xMixStafford13(l3);

    if ((rand.lo | rand.hi) == 0L) {
        rand.lo = 11400714819323198485ULL;
        rand.hi = 7640891576956012809ULL;
    }

    return rand;
}

static inline Xoroshiro constructFromHash(char *str, Xoroshiro *fac)
{
    XoroHash hash = constructHash(str);

    Xoroshiro rand;
    rand.lo = fac->lo ^ hash.lo_hash;
    rand.hi = fac->hi ^ hash.hi_hash;
    if ((rand.lo | rand.hi) == 0L) {
        rand.lo = 11400714819323198485ULL;
        rand.hi = 7640891576956012809ULL;
    }

    return rand;
}

static inline Xoroshiro constructAt(Xoroshiro *fac, int32_t block_x, int32_t block_y, int32_t block_z)
{
    uint64_t i = (uint64_t)(block_x * 3129871) ^ (uint64_t)(block_z) * 116129781ULL ^ (uint64_t)(block_y);
    i = i * i * 42317861ULL + i * 11ULL;
    i = (uint64_t)(((int64_t)(i)) >> 16);
    uint64_t j = i ^ fac->lo;
    
    Xoroshiro rand;
    rand.lo = j;
    rand.hi = fac->hi;
    if ((rand.lo | rand.hi) == 0L) {
        rand.lo = 11400714819323198485ULL;
        rand.hi = 7640891576956012809ULL;
    }

    return rand;
}

// for the mob spawning algorithm
static inline void setDecoratorSeed(uint64_t *state, uint64_t world_seed, int32_t chunk_x, int32_t chunk_z)
{
    setSeed(state, world_seed);
    *state = ((nextLong(state) * (chunk_x << 4)) + (nextLong(state) * (chunk_z << 4))) ^ world_seed;
    setSeed(state, *state);
}

// for deciding what mob to spawn
static inline uint64_t sampleRandomWeighted(const uint64_t *weights, uint64_t size,
                                            uint64_t weights_sum, uint64_t *rand)
{
    int val = nextInt(rand, weights_sum);
    for (uint64_t i = 0ULL; i < size; i++) {
        val -= weights[i];
        if (val < 0) return i;
    }
    return size;
}

#endif