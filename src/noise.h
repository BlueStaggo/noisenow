#pragma once

#include <stddef.h>
#include <stdint.h>

#include "rand.h"

typedef enum NoiseType {
  WHITE_NOISE,
  PINK_NOISE,
  BROWN_NOISE,
} NoiseType;

typedef struct noise_settings {
  double volume;
  NoiseType type;
  int channels;
} NoiseSettings;

NoiseType get_noise_type_from_string(const char* string);
void generate_noise(NoiseSettings settings, uint8_t* buffer, size_t size, Random* rand);
