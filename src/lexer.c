#include "kilate/lexer.h"

#include <ctype.h>
#include <stdarg.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kilate/bool.h"
#include "kilate/error.h"
#include "kilate/string.h"

klt_lexer* klt_lexer_make(klt_str input) {
  klt_lexer* lexer = malloc(sizeof(klt_lexer));
  lexer->__pos__ = 0;
  lexer->__input__ = strdup(input);
  lexer->tokens = klt_vector_make(sizeof(klt_token*));
  lexer->__line__ = 1;
  lexer->__column__ = 1;
  return lexer;
}

void klt_lexer_delete(klt_lexer* lexer) {
  for (size_t i = 0; i < lexer->tokens->size; ++i) {
    klt_token* token = *(klt_token**)klt_vector_get(lexer->tokens, i);
    free(token->text);
    free(token);
  }
  klt_vector_delete(lexer->tokens);
  free(lexer->__input__);
  free(lexer);
}

klt_token* klt_token_make(klt_token_type type,
                          klt_str text,
                          size_t line,
                          size_t column) {
  klt_token* tk = malloc(sizeof(klt_token));
  tk->type = type;
  tk->text = strdup(text);
  tk->line = line;
  tk->column = column;
  return tk;
}

klt_str klt_tokentype_tostr(klt_token_type type) {
  switch (type) {
    case TOKEN_KEYWORD:
      return "keyword";
    case TOKEN_IDENTIFIER:
      return "identifier";
    case TOKEN_STRING:
      return "string";
    case TOKEN_LPAREN:
      return "left_parenthesis";
    case TOKEN_RPAREN:
      return "right_parenthesis";
    case TOKEN_LBRACE:
      return "left_brace";
    case TOKEN_RBRACE:
      return "right_brace";
    case TOKEN_RARROW:
      return "right_arrow";
    case TOKEN_LARROW:
      return "left_arrow";
    case TOKEN_COLON:
      return "colon";
    case TOKEN_TYPE:
      return "type";
    case TOKEN_BOOL:
      return "boolean";
    case TOKEN_INT:
      return "int";
    case TOKEN_FLOAT:
      return "float";
    case TOKEN_LONG:
      return "double";
    case TOKEN_COMMA:
      return "comma";
    case TOKEN_ASSIGN:
      return "assign";
    case TOKEN_LET:
      return "let";
    case TOKEN_VAR:
      return "var";
    case TOKEN_EOF:
      return "end_of_file";
    default:
      return "unknow_token";
  };
}

void klt_lexer_advance(klt_lexer* lexer) {
  if (lexer->__input__[lexer->__pos__] == '\n') {
    lexer->__line__++;
    lexer->__column__ = 1;
  } else {
    lexer->__column__++;
  }
  lexer->__pos__++;
}

void klt_lexer_tokenize(klt_lexer* lexer) {
  size_t inputLen = klt_str_length(lexer->__input__);
  while (lexer->__pos__ < inputLen) {
    char c = lexer->__input__[lexer->__pos__];
    if (isspace(c)) {
      klt_lexer_advance(lexer);
      continue;
    }

    switch (c) {
      case '(': {
        size_t tkl = lexer->__line__;
        size_t tkc = lexer->__column__;
        klt_token* token = klt_token_make(TOKEN_LPAREN, "(", tkl, tkc);
        klt_vector_push_back(lexer->tokens, &token);

        klt_lexer_advance(lexer);
        continue;
      }
      case ')': {
        size_t tkl = lexer->__line__;
        size_t tkc = lexer->__column__;
        klt_token* token = klt_token_make(TOKEN_RPAREN, ")", tkl, tkc);
        klt_vector_push_back(lexer->tokens, &token);

        klt_lexer_advance(lexer);
        continue;
      }
      case '{': {
        size_t tkl = lexer->__line__;
        size_t tkc = lexer->__column__;
        klt_token* token = klt_token_make(TOKEN_LBRACE, "{", tkl, tkc);
        klt_vector_push_back(lexer->tokens, &token);

        klt_lexer_advance(lexer);
        continue;
      }
      case '}': {
        size_t tkl = lexer->__line__;
        size_t tkc = lexer->__column__;
        klt_token* token = klt_token_make(TOKEN_RBRACE, "}", tkl, tkc);
        klt_vector_push_back(lexer->tokens, &token);

        klt_lexer_advance(lexer);
        continue;
      }
      case ':': {
        size_t tkl = lexer->__line__;
        size_t tkc = lexer->__column__;
        klt_token* token = klt_token_make(TOKEN_COLON, ":", tkl, tkc);
        klt_vector_push_back(lexer->tokens, &token);

        klt_lexer_advance(lexer);
        continue;
      }
      case ',': {
        size_t tkl = lexer->__line__;
        size_t tkc = lexer->__column__;
        klt_token* token = klt_token_make(TOKEN_COMMA, ",", tkl, tkc);
        klt_vector_push_back(lexer->tokens, &token);

        klt_lexer_advance(lexer);
        continue;
      }
      case '=': {
        size_t tkl = lexer->__line__;
        size_t tkc = lexer->__column__;
        klt_token* token = klt_token_make(TOKEN_ASSIGN, "=", tkl, tkc);
        klt_vector_push_back(lexer->tokens, &token);

        klt_lexer_advance(lexer);
        continue;
      }
      case '\n': {
        // lexer->__line__++;
        //   lexer->__column__ = 1;
        klt_lexer_advance(lexer);
        continue;
      }
      case ';': {
        klt_lexer_advance(lexer);
        continue;
      }
    };
    if (klt_str_starts_with(lexer->__input__, "//", lexer->__pos__)) {
      lexer->__pos__ += 2;
      while (lexer->__pos__ < inputLen &&
             lexer->__input__[lexer->__pos__] != '\n') {
        klt_lexer_advance(lexer);
      }
      continue;
    }
    if (klt_str_starts_with(lexer->__input__, "->", lexer->__pos__)) {
      size_t tkl = lexer->__line__;
      size_t tkc = lexer->__column__;
      klt_token* token = klt_token_make(TOKEN_RARROW, "->", tkl, tkc);
      klt_vector_push_back(lexer->tokens, &token);

      lexer->__pos__ += 2;
      continue;
    }
    if (klt_str_starts_with(lexer->__input__, "<-", lexer->__pos__)) {
      size_t tkl = lexer->__line__;
      size_t tkc = lexer->__column__;
      klt_token* token = klt_token_make(TOKEN_LARROW, "->", tkl, tkc);
      klt_vector_push_back(lexer->tokens, &token);

      lexer->__pos__ += 2;
      continue;
    }
    if (c == '"') {
      size_t end = klt_str_index_of(lexer->__input__, '"', lexer->__pos__ + 1);
      if (end == SIZE_MAX) {
        printf("Unclosed string\n");
        lexer->__pos__ = inputLen;
        break;
      }
      klt_str str =
          klt_str_substring(lexer->__input__, lexer->__pos__ + 1, end);
      if (str == NULL) {
        printf("Failed to get substr\n");
        break;
      }
      size_t tkl = lexer->__line__;
      size_t tkc = lexer->__column__;
      klt_token* token = klt_token_make(TOKEN_STRING, str, tkl, tkc);
      klt_vector_push_back(lexer->tokens, &token);
      free(str);

      lexer->__pos__ = end + 1;
      continue;
    }
    if (isdigit(c)) {
      size_t start = lexer->__pos__;
      klt_bool has_dot = false;
      while (lexer->__pos__ < inputLen) {
        char ch = lexer->__input__[lexer->__pos__];
        if (isdigit(ch)) {
          klt_lexer_advance(lexer);
        } else if (ch == '.' && !has_dot) {
          has_dot = true;
          klt_lexer_advance(lexer);
        } else {
          break;
        }
      }

      klt_bool is_long = false;
      if (lexer->__pos__ < inputLen &&
          (lexer->__input__[lexer->__pos__] == 'l' ||
           lexer->__input__[lexer->__pos__] == 'L')) {
        is_long = true;
        klt_lexer_advance(lexer);
      }

      klt_str number =
          klt_str_substring(lexer->__input__, start, lexer->__pos__);
      if (number == NULL) {
        klt_lexer_error(lexer, "Failed to extract number");
      }

      size_t tkl = lexer->__line__;
      size_t tkc = lexer->__column__;
      klt_token_type numType = TOKEN_INT;

      if (is_long) {
        numType = TOKEN_LONG;
      } else if (has_dot) {
        numType = TOKEN_FLOAT;
      }

      klt_token* token = klt_token_make(numType, number, tkl, tkc);
      klt_vector_push_back(lexer->tokens, &token);
      free(number);
      continue;
    }

    if (isalpha(c) || c == '_') {
      size_t start = lexer->__pos__;
      while (lexer->__pos__ < inputLen &&
                 (isalpha(lexer->__input__[lexer->__pos__]) ||
                  isdigit(lexer->__input__[lexer->__pos__]) ||
             lexer->__input__[lexer->__pos__] == '_')) {
        klt_lexer_advance(lexer);
      }
      klt_str word = klt_str_substring(lexer->__input__, start, lexer->__pos__);
      if (word == NULL) {
        klt_lexer_error(lexer, "Failed to get word");
        break;
      }
      size_t tkl = lexer->__line__;
      size_t tkc = lexer->__column__;
      if (klt_str_equals(word, "work") || klt_str_equals(word, "return")) {
        klt_token* token = klt_token_make(TOKEN_KEYWORD, word, tkl, tkc);
        klt_vector_push_back(lexer->tokens, &token);

      } else if (klt_str_equals(word, "true") ||
                 klt_str_equals(word, "false")) {
        klt_token* token = klt_token_make(TOKEN_BOOL, word, tkl, tkc);
        klt_vector_push_back(lexer->tokens, &token);

      } else if (klt_str_equals(word, "bool") || klt_str_equals(word, "int") ||
                 klt_str_equals(word, "float") ||
                 klt_str_equals(word, "long") ||
                 klt_str_equals(word, "string") ||
                 klt_str_equals(word, "any")) {
        klt_token* token = klt_token_make(TOKEN_TYPE, word, tkl, tkc);
        klt_vector_push_back(lexer->tokens, &token);
      } else if (klt_str_equals(word, "var")) {
        klt_token* token = klt_token_make(TOKEN_VAR, word, tkl, tkc);
        klt_vector_push_back(lexer->tokens, &token);
      } else if (klt_str_equals(word, "let")) {
        klt_token* token = klt_token_make(TOKEN_LET, word, tkl, tkc);
        klt_vector_push_back(lexer->tokens, &token);
      } else {
        klt_token* token = klt_token_make(TOKEN_IDENTIFIER, word, tkl, tkc);
        klt_vector_push_back(lexer->tokens, &token);
      }
      free(word);
      continue;
    }
    klt_lexer_error(lexer, "Unexpected character %c", c);
    break;
  }
  size_t tkl = lexer->__line__;
  size_t tkc = lexer->__column__;
  klt_token* token = klt_token_make(TOKEN_EOF, "", tkl, tkc);
  klt_vector_push_back(lexer->tokens, &token);
}

void klt_lexer_error(klt_lexer* lexer, klt_str fmt, ...) {
  va_list args;
  va_start(args, fmt);
  fprintf(stderr, "[Error at %zu:%zu] ", lexer->__line__, lexer->__column__);
  vprintf(fmt, args);
  printf("\n");
  va_end(args);
  exit(1);
}