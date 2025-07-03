#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "args.h"

static void args_print_help(FILE* stream, const char* cmd, int paramc, const Parameter* paramk) {
  fprintf(stream, "Help for %s:\n", cmd);

  for (int i = 0; i < paramc; i++) {
    Parameter param = paramk[i];

    if (param.short_name) {
      fprintf(stream, "-%-5s", param.short_name);
    } else {
      fprintf(stream, "      ");
    }

    if (param.name) {
      fprintf(stream, "--%-18s", param.name);
    } else {
      fprintf(stream, "                    ");
    }

    if (param.description) {
      fprintf(stream, ": %s", param.description);
    }

    fprintf(stream, "\n");
  }
}

bool args_parse(int argc, const char* argv[], int paramc, const Parameter* paramk, ParsedParameter* parsed_params) {
  if (argc < 1) {
    return false;
  }

  for (int p = 0; p < paramc; p++) {
    Parameter param = paramk[p];
    MutableParameterValue param_value = (MutableParameterValue) param.default_value;

    for (int a = 1; a < argc; a++) {
      const char* arg = argv[a];
      if (arg[0] != '-') continue;

      if (param.name != nullptr && arg[1] == '-' && strcmp(arg + 2, param.name) == 0) {
      } else if (param.short_name != nullptr && strcmp(arg + 1, param.short_name) == 0) {
      } else {
        continue;
      }

      if (param.type == PARAM_HELP) {
        args_print_help(stdout, argv[0], paramc, paramk);
        return false;
      }

      if (param.type == PARAM_BOOL) {
        param_value.as_bool = !param_value.as_bool;
        break;
      }

      a++;
      if (a >= argc) {
        break;
      }
      arg = argv[a];

      switch (param.type) {
        case PARAM_HELP:
        case PARAM_BOOL:
          break;

        case PARAM_UINT32:
          param_value.as_uint32 = atoi(arg);
          break;

        case PARAM_INT32:
          param_value.as_int32 = atoi(arg);
          break;

        case PARAM_DOUBLE:
          param_value.as_double = atof(arg);
          break;

        case PARAM_STRING:
          param_value.as_string = arg;
          break;
      }

      break;
    }

    ParsedParameter parsed_param = {
      .param = param,
      .value = param_value.immutable
    };
    memcpy(parsed_params + p, &parsed_param, sizeof(parsed_param));
  }

  return true;
}
