#pragma once

#include <stddef.h>
#include <stdint.h>

typedef enum ParameterType {
  PARAM_HELP,
  PARAM_BOOL,
  PARAM_INT32,
  PARAM_UINT32,
  PARAM_DOUBLE,
  PARAM_STRING
} ParameterType;

typedef union ParameterValue {
  bool const as_bool;
  int32_t const as_int32;
  uint32_t const as_uint32;
  double const as_double;
  const char* const as_string;
} ParameterValue;

typedef union MutableParameterValue {
  bool as_bool;
  int32_t as_int32;
  uint32_t as_uint32;
  double as_double;
  const char* as_string;
  ParameterValue immutable;
} MutableParameterValue;

typedef struct Parameter {
  const char* const name;
  const char* const short_name;
  const char* const description;
  const ParameterValue default_value;
  const ParameterType type;
} Parameter;

typedef struct ParsedParameter {
  const Parameter param;
  const ParameterValue value;
} ParsedParameter;

#define param_bool(v)

bool args_parse(int argc, const char* argv[], int paramc, const Parameter* paramk,
                ParsedParameter* parsed_params);


