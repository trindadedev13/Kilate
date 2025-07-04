#include "kilate/native.h"

#include <dirent.h>
#include <dlfcn.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#define psleep(ms) Sleep(ms)
#else
#include <unistd.h>
#define psleep(ms) usleep(ms * 1000)
#endif

#include "kilate/lexer.h"
#include "kilate/node.h"
#include "kilate/string.h"
#include "kilate/vector.h"

klt_node_vector* native_functions = NULL;

void klt_native_init() {
  native_functions = klt_vector_make(sizeof(klt_native_fnentry*));
  klt_native_register_all_functions();
  klt_native_load_extern();
}

void klt_native_load_extern() {
  klt_str dir;
  if ((dir = getenv("KLT_SO_DIRS")) == NULL) {
    dir = ".";
  }

  DIR* d = opendir(dir);
  if (!d)
    return;

  struct dirent* entry;
  while ((entry = readdir(d))) {
    if (strstr(entry->d_name, ".so")) {
      char path[512];
      snprintf(path, sizeof(path), "%s/%s", dir, entry->d_name);

      void* handle = dlopen(path, RTLD_NOW);
      if (!handle) {
        fprintf(stderr, "Error loading %s: %s\n", path, dlerror());
        continue;
      }

      void (*extern_native_init)() = dlsym(handle, "KILATE_NATIVE_REGISTER");
      if (!extern_native_init) {
        fprintf(stderr, "Function KILATE_NATIVE_REGISTER not found in %s\n",
                path);
        continue;
      }
      extern_native_init();
    }
  }
  closedir(d);
}

void klt_native_end() {
  for (size_t i = 0; i < native_functions->size; ++i) {
    klt_native_fnentry* entry =
        *(klt_native_fnentry**)klt_vector_get(native_functions, i);
    free(entry->name);
    if (entry->requiredParams != NULL)
      klt_vector_delete(entry->requiredParams);
    free(entry);
  }
  klt_vector_delete(native_functions);
}

void klt_native_register_all_functions() {
  {
    // Register native print method
    klt_str_vector* requiredParams = klt_vector_make(sizeof(klt_str*));
    klt_str any = "any";
    klt_vector_push_back(requiredParams, &any);
    klt_native_register_fn("print", requiredParams, klt_native_print);
  }
  {
    // Register native system method
    klt_str_vector* requiredParams = klt_vector_make(sizeof(klt_str*));
    klt_str str = "string";
    klt_vector_push_back(requiredParams, &str);
    klt_native_register_fn("system", requiredParams, klt_native_system);
  }
  {
    // Register native system method
    klt_str_vector* requiredParams = klt_vector_make(sizeof(klt_str*));
    klt_str str = "long";
    klt_vector_push_back(requiredParams, &str);
    klt_native_register_fn("sleep", requiredParams, klt_native_sleep);
  }
}

void klt_native_register_fnentry(klt_native_fnentry* entry) {
  klt_vector_push_back(native_functions, &entry);
}

void klt_native_register_fn(klt_str name,
                            klt_str_vector* requiredParams,
                            klt_native_fn fn) {
  klt_native_fnentry* entry = malloc(sizeof(klt_native_fnentry));
  entry->name = strdup(name);
  entry->fn = fn;
  entry->requiredParams = requiredParams;
  klt_native_register_fnentry(entry);
}

klt_native_fnentry* klt_native_find_function(klt_str name) {
  for (size_t i = 0; i < native_functions->size; ++i) {
    klt_native_fnentry* entry =
        *(klt_native_fnentry**)klt_vector_get(native_functions, i);
    if (klt_str_equals(entry->name, name)) {
      return entry;
    }
  }
  return NULL;
}

klt_node* klt_native_print(klt_native_fndata* data) {
  for (size_t i = 0; i < data->params->size; ++i) {
    klt_node_fnparam* param =
        *(klt_node_fnparam**)klt_vector_get(data->params, i);
    if (param->type == NODE_VALUE_TYPE_VAR) {
      klt_node* var = klt_environment_get(data->env, param->value);
      void* value = var->vardec_n.var_value;
      switch (var->vardec_n.var_value_type) {
        case NODE_VALUE_TYPE_INT: {
          printf("%d", (int)(intptr_t)value);
          break;
        }
        case NODE_VALUE_TYPE_FLOAT: {
          printf("%f", *(float*)value);
          break;
        }
        case NODE_VALUE_TYPE_LONG: {
          printf("%ld", (long)(intptr_t)value);
          break;
        }
        case NODE_VALUE_TYPE_STRING:
          printf("%s", (klt_str)value);
          break;
        case NODE_VALUE_TYPE_BOOL:
          printf("%s", (klt_bool)(intptr_t)value ? "true" : "false");
          break;
        case NODE_VALUE_TYPE_FUNC:
          // Does nothing for now
          break;
        case NODE_VALUE_TYPE_VAR:
          // Does nothing for now
          break;
        default:
          // Does nothing for now
          break;
      }
      continue;
    }
    klt_str interpreted = klt_str_interpret_escapes(param->value);
    printf("%s", interpreted);
    free(interpreted);
  }
  free(data);
  return NULL;
}

klt_node* klt_native_system(klt_native_fndata* data) {
  for (size_t i = 0; i < data->params->size; ++i) {
    klt_node_fnparam* param =
        *(klt_node_fnparam**)klt_vector_get(data->params, i);
    if (param->type == NODE_VALUE_TYPE_VAR) {
      klt_node* var = klt_environment_get(data->env, param->value);
      void* value = var->vardec_n.var_value;
      switch (var->vardec_n.var_value_type) {
        case NODE_VALUE_TYPE_STRING:
          system((klt_str)value);
          break;
        default:
          break;
      }
      continue;
    }
    system(param->value);
  }
  free(data);
  return NULL;
}

klt_node* klt_native_sleep(klt_native_fndata* data) {
  klt_node_fnparam* param =
      *(klt_node_fnparam**)klt_vector_get(data->params, 0);
  if (param->type == NODE_VALUE_TYPE_VAR) {
    klt_node* var = klt_environment_get(data->env, param->value);
    if (var->vardec_n.var_value_type != NODE_VALUE_TYPE_INT) {
      psleep((int)(intptr_t)param->value);
    }
  }
  psleep(klt_str_to_int(param->value));
  return NULL;
}