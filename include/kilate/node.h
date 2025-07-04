#ifndef __NODE_H__
#define __NODE_H__

#include "kilate/lexer.h"
#include "kilate/string.h"
#include "kilate/vector.h"

typedef enum {
  NODE_FUNCTION,
  NODE_PRINT,
  NODE_CALL,
  NODE_RETURN,
  NODE_VARDEC
} klt_nodetype;

typedef enum {
  NODE_VALUE_TYPE_INT,
  NODE_VALUE_TYPE_FLOAT,
  NODE_VALUE_TYPE_LONG,
  NODE_VALUE_TYPE_STRING,
  NODE_VALUE_TYPE_BOOL,
  NODE_VALUE_TYPE_VAR,
  NODE_VALUE_TYPE_FUNC,
  NODE_VALUE_TYPE_ANY
} klt_node_valuetype;

typedef struct klt_node klt_node;
typedef klt_vector klt_node_vector;

typedef struct {
  klt_str value;
  // klt_str typeStr;
  klt_node_valuetype type;
} klt_node_fnparam;

typedef klt_vector klt_node_fnparam_vector;

struct klt_node {
  klt_nodetype type;

  struct {
    klt_str fn_name;
    klt_str fn_return_type;
    klt_node_vector* fn_body;
    klt_node_fnparam_vector* fn_params;
  } function_n;

  struct {
    klt_str fn_call_name;
    klt_node_fnparam_vector* fn_call_params;
  } call_n;

  struct {
    klt_node_valuetype return_type;
    void* return_value;
  } return_n;

  struct {
    klt_str var_name;
    klt_str var_type;
    klt_node_valuetype var_value_type;
    void* var_value;
  } vardec_n;
};

klt_node* klt_function_node_make(klt_str,
                                 klt_str,
                                 klt_node_vector*,
                                 klt_node_fnparam_vector*);

klt_node* klt_call_node_make(klt_str, klt_node_fnparam_vector*);

klt_node* klt_return_node_make(klt_node_valuetype, void*);

klt_node* klt_var_dec_node_make(klt_str, klt_str, klt_node_valuetype, void*);

#endif