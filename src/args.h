#pragma once

#include <stddef.h>
#include <stdint.h>

typedef enum param_type {
  PARAM_HELP,
  PARAM_BOOL,
  PARAM_INT32,
  PARAM_UINT32,
  PARAM_DOUBLE,
  PARAM_STRING
} param_type;

typedef union param_value {
  bool const as_bool;
  int32_t const as_int32;
  uint32_t const as_uint32;
  double const as_double;
  const char* const as_string;
} param_value;

typedef union mutable_param_value {
  bool as_bool;
  int32_t as_int32;
  uint32_t as_uint32;
  double as_double;
  const char* as_string;
  param_value immutable;
} mutable_param_value;

typedef struct param {
  const char* const name;
  const char* const short_name;
  const char* const description;
  const param_value default_value;
  const param_type type;
} param;

typedef struct parsed_param {
  const param param;
  const param_value value;
} parsed_param;

typedef struct args args;

#define param_bool(v)

bool args_parse(int argc, const char* argv[], int paramc, const param* paramk,
                parsed_param* parsed_params);
void args_free(args* args);


