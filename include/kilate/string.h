#ifndef __STRING_H__
#define __STRING_H__

#include <stdarg.h>
#include <stdlib.h>

#include "kilate/bool.h"
#include "kilate/vector.h"

typedef char* klt_str;
typedef klt_vector klt_str_vector;

size_t klt_str_length(const klt_str);

klt_bool klt_str_starts_with(const klt_str, const klt_str, size_t);

size_t klt_str_index_of(const klt_str, char, size_t);

klt_str klt_str_substring(const klt_str, size_t, size_t);

klt_bool klt_str_equals(const klt_str, const klt_str);

void klt_str_concat(klt_str, klt_str);

int klt_str_to_int(klt_str);

float klt_str_to_float(klt_str);

long klt_str_to_long(klt_str);

klt_str klt_str_format(const klt_str, ...);

klt_str klt_str_interpret_escapes(klt_str);

#endif