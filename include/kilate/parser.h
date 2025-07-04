#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdarg.h>

#include "kilate/lexer.h"
#include "kilate/native.h"
#include "kilate/node.h"
#include "kilate/string.h"
#include "kilate/vector.h"

typedef struct {
  klt_token_vector* tokens;
  klt_node_vector* functions;
  size_t __pos__;
} klt_parser;

klt_parser* klt_parser_make(klt_token_vector*);

void klt_parser_delete(klt_parser*);

void klt_parser_delete_params(klt_str_vector*);

klt_token* klt_parser_consume(klt_parser*, klt_token_type);

klt_node* klt_parser_find_function(klt_parser*, klt_str);

klt_str klt_parser_tokentype_to_str(klt_token_type);

klt_str klt_parser_nodevaluetype_to_str(klt_node_valuetype);

klt_node_valuetype klt_parser_tokentype_to_nodevaluetype(klt_parser*,
                                                         klt_token*);

klt_node_valuetype klt_parser_str_to_nodevaluetype(klt_str);


klt_node* klt_parser_parse_statement(klt_parser*);

klt_node_fnparam_vector* klt_parser_parse_fnparams(klt_parser* parser);

void klt_parser_fn_validate_params(klt_node*, klt_node_fnparam_vector*, klt_token*);

klt_node* klt_parser_parse_call_node(klt_parser*, klt_token*);

void klt_parser_parse_program(klt_parser*);

klt_node* klt_parser_parse_function(klt_parser*);

void klt_parser_error(klt_token*, klt_str, ...);

#endif