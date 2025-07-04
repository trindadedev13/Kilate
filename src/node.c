#include "kilate/node.h"


#include <stdlib.h>
#include <string.h>

#include "kilate/string.h"
#include "kilate/vector.h"

klt_node* klt_function_node_make(klt_str name,
                                 klt_str return_type,
                                 klt_node_vector* body,
                                 klt_node_fnparam_vector* params) {
  klt_node* node = malloc(sizeof(klt_node));
  node->type = NODE_FUNCTION;
  node->function_n.fn_name = strdup(name);
  if (return_type != NULL) {
    node->function_n.fn_return_type = strdup(return_type);
  } else {
    node->function_n.fn_return_type = NULL;
  }
  node->function_n.fn_body = body;
  node->function_n.fn_params = params;
  return node;
}

klt_node* klt_call_node_make(const klt_str functionName,
                             klt_node_fnparam_vector* functionParams) {
  klt_node* node = malloc(sizeof(klt_node));
  node->type = NODE_CALL;
  node->call_n.fn_call_name = strdup(functionName);
  node->call_n.fn_call_params = functionParams;
  return node;
}

klt_node* klt_return_node_make(klt_node_valuetype return_type,
                               void* return_value) {
  klt_node* node = malloc(sizeof(klt_node));
  node->type = NODE_RETURN;
  node->return_n.return_type = return_type;
  node->return_n.return_value = return_value;
  return node;
}

klt_node* klt_var_dec_node_make(klt_str name,
                                klt_str type,
                                klt_node_valuetype valueType,
                                void* value) {
  klt_node* node = malloc(sizeof(klt_node));
  node->type = NODE_VARDEC;
  node->vardec_n.var_name = strdup(name);
  node->vardec_n.var_type = strdup(type);
  node->vardec_n.var_value_type = valueType;
  node->vardec_n.var_value = value;
  return node;
}