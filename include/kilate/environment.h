#ifndef __ENVIRONMENT_H__
#define __ENVIRONMENT_H__

#include "kilate/bool.h"
#include "kilate/node.h"
#include "kilate/string.h"

typedef struct klt_env_entry {
  klt_str name;
  klt_node* value;
  struct klt_env_entry* next;
} klt_env_entry;

typedef struct klt_environment {
  klt_env_entry* entries;
  struct klt_environment* parent;
} klt_environment;

klt_environment* klt_environment_make(klt_environment* parent);

void klt_environment_destroy(klt_environment* env);

klt_bool klt_environment_define(klt_environment* env,
                            const klt_str name,
                            void* value);

klt_node* klt_environment_get(klt_environment* env, const klt_str name);

klt_bool klt_environment_set(klt_environment* env, const klt_str name, void* value);

#endif