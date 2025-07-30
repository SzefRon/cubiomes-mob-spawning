// this code checks for a pack of 4 foxes spawning in the 0,0 chunk
// all of them having an item in their mainhand (3 wheat and 1 egg)

#include <stdio.h>
#include <stdlib.h>

#include "generator.h"
#include "finders.h"
#include "biomes.h"
#include "rng.h"
#include "rng_extra.h"

#define MOB_POOL_SIZE 7
#define CREATURE_PROBABILITY 0.1f

int main()
{
    // how likely a certain mob is to be spawned
    const uint64_t mob_weights[MOB_POOL_SIZE] = {12, 10, 10, 8, 8, 4, 8};
    
    uint64_t weights_sum = 0;
    for (int i = 0; i < MOB_POOL_SIZE; i++)
        weights_sum += mob_weights[i];

    uint64_t rand;
    Generator g;
    setupGenerator(&g, MC_1_21_1, 0);

    // chunk pos to test for mob spawning
    int32_t chunk_x = 0, chunk_z = 0;
    uint64_t start_seed = 786545808ULL;

    for (uint64_t w_seed = start_seed;; w_seed++) {
        setDecoratorSeed(&rand, w_seed, chunk_x, chunk_z);

        // creature spawns
        if (nextFloat(&rand) >= CREATURE_PROBABILITY) continue;

        // creature is fox
        int mob_idx = sampleRandomWeighted(mob_weights, MOB_POOL_SIZE, weights_sum, &rand);
        if (mob_idx != 6) continue;
        // mob index for taiga biomes:
        // 0 - sheep, 1 - pig, 2 - chicken, 3 - cow, 4 - wolf, 5 - rabbit, 6 - fox

        // fox pack size is 4
        int pack_size = nextInt(&rand, 3) + 2;
        if (pack_size != 4) continue;

        // get random source for finalizing mob spawn
        Xoroshiro rand1 = constructFromWSeed(w_seed);
        Xoroshiro fac1;
        fac1.lo = xNextLong(&rand1);
        fac1.hi = xNextLong(&rand1);
        
        Xoroshiro rand2 = constructFromHash("minecraft:worldgen_region_random", &fac1);
        Xoroshiro fac2;
        fac2.lo = xNextLong(&rand2);
        fac2.hi = xNextLong(&rand2);

        Xoroshiro rand3 = constructAt(&fac2, chunk_x << 4, 0, chunk_z << 4);

        // fox item requirements
        int egg_count = 0, wheat_count = 0;
        int pass = 1;
        for (int i = 0; i < 4; i++) {
            if (xNextFloat(&rand3) >= 0.2f) {   // doesn't have an item
                pass = 0;
                break;
            }
            float f = xNextFloat(&rand3);
            if (f >= 0.05f && f < 0.2f) {
                egg_count++;
            }
            else if (f >= 0.4f && f < 0.6f) {
                wheat_count++;
            }
            xSkipN(&rand3, 3); // 1 x left-handedness + 2 x for one rand.triangle() call
        }

        if (pass == 0) continue;
        if (egg_count != 1 || wheat_count != 3) continue;

        // biome is taiga
        applySeed(&g, DIM_OVERWORLD, w_seed);
        int biome = getBiomeAt(&g, 1, 0, 320, 0);
        int biome2 = getBiomeAt(&g, 1, 16, 320, 16);
        if (biome != taiga && biome != old_growth_spruce_taiga && biome != old_growth_pine_taiga)
            continue;
        if (biome2 != biome) continue;
        
        // mob spawn isn't guaranteed because of additional terrain checks
        // which we can't simulate (yet)
        printf("seed: %lld\n", (int64_t)w_seed);
    }

    return 0;
}