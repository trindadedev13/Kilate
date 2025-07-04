#ifndef __LEXER_H__
#define __LEXER_H__

#include <stdarg.h>

#include "kilate/string.h"
#include "kilate/vector.h"

typedef enum {
  TOKEN_KEYWORD,     // work, return
  TOKEN_IDENTIFIER,  // hello, main
  TOKEN_STRING,      // "Hello world!"
  TOKEN_LPAREN,      // (
  TOKEN_RPAREN,      // )
  TOKEN_LBRACE,      // {
  TOKEN_RBRACE,      // }
  TOKEN_RARROW,      // ->
  TOKEN_LARROW,      // <-
  TOKEN_COLON,       // :
  TOKEN_TYPE,        // bool
  TOKEN_BOOL,        // true, false
  TOKEN_INT,         // 123
  TOKEN_FLOAT,       // 1.23
  TOKEN_LONG,        // 123l
  TOKEN_COMMA,       // ,,
  TOKEN_ASSIGN,      // =
  TOKEN_VAR,         // var
  TOKEN_LET,         // let
  TOKEN_EOF          // end of file.
} klt_token_type;

typedef struct {
  klt_token_type type;
  klt_str text;

  size_t column;
  size_t line;
} klt_token;

typedef klt_vector klt_token_vector;

klt_token* klt_token_make(klt_token_type, klt_str, size_t, size_t);

klt_str klt_tokentype_tostr(klt_token_type);

typedef struct {
  klt_str __input__;
  klt_token_vector* tokens;

  size_t __pos__;
  size_t __column__;
  size_t __line__;
} klt_lexer;

klt_lexer* klt_lexer_make(klt_str);

void klt_lexer_delete(klt_lexer*);

void klt_lexer_advance(klt_lexer*);

void klt_lexer_tokenize(klt_lexer*);

void klt_lexer_error(klt_lexer*, klt_str, ...);

#endif