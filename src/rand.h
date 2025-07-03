#pragma once

#include <stdint.h>

typedef struct Random {
  uint64_t seed[4];
} Random;

Random rand_init(uint64_t seed);
uint64_t rand_next(Random* rand);
double rand_next_double(Random* rand);
void rand_jump(Random* rand);
void rand_long_jump(Random* rand);
