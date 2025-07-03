/*
xoshiro256++ written in 2019 and xoshiro256+ written in 2018
by David Blackman and Sebastiano Vigna (vigna@acm.org)

To the extent possible under law, the author has dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide.

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include <stdint.h>

#include "rand.h"

Random rand_init(uint64_t seed) {
  Random rand;
  rand.seed[0] = seed;
  for (int i = 1; i < 4; i++) {
    // Using a split mix to initialize the 256 bit seed from a 64 bit integer
    seed += 0x9e3779b97f4a7c15;
    uint64_t z = seed;
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
    z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
    rand.seed[i] = z ^ (z >> 31);
  }
  return rand;
}

static inline uint64_t rotl(const uint64_t x, int k) {
  return (x << k) | (x >> (64 - k));
}

static inline void xor_seed(uint64_t* s) {
  const uint64_t t = s[1] << 17;

  s[2] ^= s[0];
  s[3] ^= s[1];
  s[1] ^= s[2];
  s[0] ^= s[3];

  s[2] ^= t;

  s[3] = rotl(s[3], 45);
}

uint64_t rand_next(Random* rand) {
  uint64_t* s = rand->seed;
  const uint64_t result = rotl(s[0] + s[3], 23) + s[0];
  xor_seed(s);
  return result;
}

double rand_next_double(Random* rand) {
  uint64_t* s = rand->seed;
  const uint64_t result = s[0] + s[3];
  xor_seed(s);
  return (result >> 11) * 0x1.0p-53;
}

static inline void rand_jump_with_table(Random* rand, const uint64_t* jump_table) {
  uint64_t* s = rand->seed;

	uint64_t s0 = 0;
	uint64_t s1 = 0;
	uint64_t s2 = 0;
	uint64_t s3 = 0;
	for(int i = 0; i < 4; i++)
		for(int b = 0; b < 64; b++) {
			if (jump_table[i] & UINT64_C(1) << b) {
				s0 ^= s[0];
				s1 ^= s[1];
				s2 ^= s[2];
				s3 ^= s[3];
			}
			rand_next(rand);
		}

	s[0] = s0;
	s[1] = s1;
	s[2] = s2;
	s[3] = s3;
}

void rand_jump(Random* rand) {
	static const uint64_t JUMP[] = { 0x180ec6d33cfd0aba, 0xd5a61266f0c9392c, 0xa9582618e03fc9aa, 0x39abdc4529b1661c };
  rand_jump_with_table(rand, JUMP);
}

void rand_long_jump(Random* rand) {
	static const uint64_t LONG_JUMP[] = { 0x76e15d3efefdcbbf, 0xc5004e441c522fb3, 0x77710069854ee241, 0x39109bb02acbe635 };
  rand_jump_with_table(rand, LONG_JUMP);
}
