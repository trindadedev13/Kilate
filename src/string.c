#include "kilate/string.h"

#include <stdlib.h>
#include <string.h>

#include "kilate/bool.h"
#include "kilate/error.h"

size_t klt_str_length(klt_str str) {
  size_t len = 0;
  while (str[len] != '\0') {
    len++;
  }
  return len;
}

klt_bool klt_str_starts_with(klt_str str, klt_str startWith, size_t offset) {
  if (strncmp(str + offset, startWith, klt_str_length(startWith)) == 0) {
    return true;
  }
  return false;
}

size_t klt_str_index_of(const klt_str str, char ch, size_t offset) {
  klt_str ptr = strchr(str + offset, ch);
  if (ptr == NULL) {
    printf("Failed to get index of string %s\n", str);
    return SIZE_MAX;
  }
  return ptr - str;
}

klt_str klt_str_substring(const klt_str str, size_t start, size_t end) {
  if (!str || start > end || end > klt_str_length(str)) {
    return NULL;
  }

  size_t len = end - start;
  klt_str result = malloc(len + 1);
  if (!result)
    return NULL;

  memcpy(result, str + start, len);
  result[len] = '\0';
  return result;
}

klt_bool klt_str_equals(const klt_str str, const klt_str other) {
  if (strcmp(str, other) == 0) {
    return true;
  }
  return false;
}

void klt_str_concat(klt_str dest, const klt_str toConcat) {
  strcat(dest, toConcat);
}

int klt_str_to_int(const klt_str src) {
  int num = 0;
  size_t i = 0;
  int sign = 1;

  if (src[0] == '-') {
    sign = -1;
    i = 1;
  }

  for (; i < klt_str_length(src); ++i) {
    if (src[i] >= '0' && src[i] <= '9') {
      num = num * 10 + (src[i] - '0');
    } else {
      break;
    }
  }

  return sign * num;
}

float klt_str_to_float(klt_str s) {
  if (s == NULL)
    return 0.0f;
  return strtof(s, NULL);
}

long klt_str_to_long(klt_str s) {
  if (s == NULL)
    return 0;
  return strtol(s, NULL, 10);
}

klt_str klt_str_format(const klt_str fmt, ...) {
  va_list args;
  va_start(args, fmt);

  size_t len = vsnprintf(NULL, 0, fmt, args);
  va_end(args);

  klt_str buffer = malloc(len + 1);
  if (!buffer)
    return NULL;

  va_start(args, fmt);
  vsnprintf(buffer, len + 1, fmt, args);
  va_end(args);

  return buffer;
}

klt_str klt_str_interpret_escapes(klt_str input) {
  size_t len = klt_str_length(input);
  klt_str output = malloc(len + 1);
  size_t j = 0;

  for (size_t i = 0; i < len; ++i) {
    if (input[i] == '\\' && i + 1 < len) {
      i++;
      switch (input[i]) {
        case 'n':
          output[j++] = '\n';
          break;
        case 't':
          output[j++] = '\t';
          break;
        case '\\':
          output[j++] = '\\';
          break;
        default:
          output[j++] = input[i];
          break;
      }
    } else {
      output[j++] = input[i];
    }
  }

  output[j] = '\0';
  return output;
}