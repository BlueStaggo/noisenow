#ifdef _WIN32
# include <Windows.h>
# define sleep_ms(ms) Sleep((ms))
#elif __has_include(<unistd.h>)
# include <unistd.h>
# define sleep_ms(ms) nanosleep(&(struct timespec) { ms / 1000, (long)(ms % 1000) * 1000000 }, nullptr)
#else
# define sleep_ms(ms)
#endif

#include <AL/al.h>
#include <AL/alc.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "args.h"
#include "noise.h"
#include "rand.h"

#define AL_ASSERT(message) do { if (check_al_error(message ": %u")) { exit(1); return 1; } } while (0)
#define AL_TRY(func, params) do { func params ; AL_ASSERT(#func); } while (0)

typedef struct Settings {
  NoiseSettings noise;
  int sample_rate;
  int time;
  bool loop;
} Settings;

static Settings init_settings(int argc, const char* argv[]);
static bool check_al_error(const char* message);

int main(int argc, const char* argv[]) {
  // Arguments
  Settings settings = init_settings(argc, argv);

  // Initialize device
  ALCdevice* device = alcOpenDevice(nullptr);
  if (!device) {
    fprintf(stderr, "Could not open an OpenAL device.\n");
    return 1;
  }
  alGetError();

  // Initialize context
  ALCcontext* context = alcCreateContext(device, nullptr);
  if (!alcMakeContextCurrent(context)) {
    fprintf(stderr, "Could not make OpenAL context current.\n");
    return 1;
  }

  ALuint audio_buffer;
  AL_TRY(alGenBuffers, (1, &audio_buffer));

  // Generate noise
  size_t buffer_size = settings.sample_rate * settings.time * settings.noise.channels;
  ALenum buffer_format = settings.noise.channels == 2 ? AL_FORMAT_STEREO8 : AL_FORMAT_MONO8;

  uint8_t buffer_data[buffer_size];
  Random rand = rand_init(time(nullptr));
  generate_noise(settings.noise, buffer_data, buffer_size, &rand);

  AL_TRY(alBufferData, (audio_buffer, buffer_format, buffer_data, buffer_size, settings.sample_rate));

  // Set up source
  ALuint audio_source;
  AL_TRY(alGenSources, (1, &audio_source));
  AL_TRY(alSourcei, (audio_source, AL_BUFFER, audio_buffer));
  AL_TRY(alSourcei, (audio_source, AL_LOOPING, settings.loop));
  AL_TRY(alSourcePlay, (audio_source));

  // Keep playing until user input or loop ends
  if (settings.loop) {
    getchar();
  } else {
    ALint source_state;
    do {
      sleep_ms(100);
      AL_TRY(alGetSourcei, (audio_source, AL_SOURCE_STATE, &source_state));
    } while (source_state == AL_PLAYING);
  }

  // Exit
  context = alcGetCurrentContext();
  device = alcGetContextsDevice(context);

  alDeleteSources(1, &audio_source);
  alDeleteBuffers(1, &audio_buffer);
  alcMakeContextCurrent(nullptr);
  alcDestroyContext(context);
  alcCloseDevice(device);

  return 0;
}

static Settings init_settings(int argc, const char* argv[]) {
  Parameter params[] = {
    (Parameter) {
      .name = "help",
      .short_name = "h",
      .description = "Print this help message.",
      .type = PARAM_HELP
    },
    (Parameter) {
      .name = "version",
      .description = "Print the version of Noise NOW!.",
      .type = PARAM_BOOL
    },
    (Parameter) {
      .name = "volume",
      .short_name = "v",
      .description = "The volume of the noise. Defaults to 1.",
      .default_value = { .as_double = 1.0 },
      .type = PARAM_DOUBLE
    },
    (Parameter) {
      .name = "type",
      .short_name = "n",
      .description = "The type of noise to use. Valid values are \"white\", \"pink\" and \"brown\". Defaults to \"white\".",
      .default_value = { .as_string = "white" },
      .type = PARAM_STRING
    },
    (Parameter) {
      .name = "mono",
      .short_name = "m",
      .description = "Use only one audio channel (mono) instead of two (stereo).",
      .default_value = { .as_bool = false },
      .type = PARAM_BOOL
    },
    (Parameter) {
      .name = "sample_rate",
      .short_name = "r",
      .description = "The amount of samples per seconds. Defaults to 44.1 kHz",
      .default_value = { .as_uint32 = 44100 },
      .type = PARAM_UINT32
    },
    (Parameter) {
      .name = "time",
      .short_name = "t",
      .description = "The amount of time in seconds to generate noise for. Note that regardless of the value given, the noise will loop. Defaults to 10 seconds.",
      .default_value = { .as_uint32 = 10 },
      .type = PARAM_UINT32
    },
    (Parameter) {
      .name = "no-loop",
      .description = "Disable looping.",
      .default_value = { .as_bool = true },
      .type = PARAM_BOOL
    },
  };
  int paramc = sizeof(params) / sizeof(params[0]);
  ParsedParameter parsed_params[paramc];

  if (!args_parse(argc, argv, paramc, params, parsed_params)) {
    exit(1);
  }

  if (parsed_params[1].value.as_bool) {
    printf("Noise NOW! Version 1.0.0 by BlueStaggo.\n");
    exit(1);
  }

  return (Settings) {
    .noise.volume = parsed_params[2].value.as_double,
    .noise.type = get_noise_type_from_string(parsed_params[3].value.as_string),
    .noise.channels = parsed_params[4].value.as_bool ? 1 : 2,
    .sample_rate = parsed_params[5].value.as_uint32,
    .time = parsed_params[6].value.as_int32,
    .loop = parsed_params[7].value.as_bool,
  };
}

static bool check_al_error(const char* message) {
  ALenum error = alGetError();
  if (error == AL_NO_ERROR) {
    return false;
  }

  fprintf(stderr, message, error);
  return true;
}
