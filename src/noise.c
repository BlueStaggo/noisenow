#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "noise.h"
#include "rand.h"

NoiseType get_noise_type_from_string(const char* string) {
  static const char* ids[] = {
    "white",
    "pink",
    "brown",
  };
  int id_count = sizeof(ids) / sizeof(ids[0]);

  for (int i = 0; i < id_count; i++) {
    if (strcmp(string, ids[i]) == 0) {
      return i;
    }
  }

  return 0;
}

void generate_noise(noise_settings settings, uint8_t* buffer, size_t size, Random* rand) {
  for (int channel = 0; channel < settings.channels; channel++) {
    double b[7];

    for (size_t i = 0; i < size; i++) {
      double amplitude = rand_next_double(rand) - 0.5;
      double white = amplitude;

      switch (settings.type) {
        case PINK_NOISE:
          // https://www.firstpr.com.au/dsp/pink-noise/
          b[0] = 0.99886 * b[0] + white * 0.0555179;
          b[1] = 0.99332 * b[1] + white * 0.0750759;
          b[2] = 0.96900 * b[2] + white * 0.1538520;
          b[3] = 0.86650 * b[3] + white * 0.3104856;
          b[4] = 0.55000 * b[4] + white * 0.5329522;
          b[5] = -0.7616 * b[5] - white * 0.0168980;
          amplitude = b[0] + b[1] + b[2] + b[3] + b[4] + b[5] + b[6] + white * 0.5362;
          amplitude *= 0.129;
          b[6] = white * 0.115926;
          break;

        case BROWN_NOISE:
          // https://stackoverflow.com/questions/26963342/generating-colors-of-noise-in-java
          amplitude = (b[0] + (0.02 * white)) / 1.02;
          b[0] = amplitude;
          amplitude *= 2.5;
          break;

        default:
          break;
      }

      amplitude = amplitude * settings.volume + 0.5;

      // Clamp amplitude to prevent wrapping
      if (amplitude < 0.0) {
        amplitude = 0.0;
      } else if (amplitude > 1.0) {
        amplitude = 1.0;
      }

      buffer[i] = (uint8_t)(amplitude * 255.0);
    }
  }
}
