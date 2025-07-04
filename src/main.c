#include <assert.h>

#include <stdio.h>

#include "kilate/error.h"
#include "kilate/file.h"
#include "kilate/interpreter.h"
#include "kilate/lexer.h"
#include "kilate/native.h"
#include "kilate/parser.h"
#include "kilate/string.h"

klt_bool interpret(klt_str src) {
  klt_lexer* lexer = klt_lexer_make(src);
  assert(lexer);
  klt_lexer_tokenize(lexer);

  klt_native_init();

  klt_parser* parser = klt_parser_make(lexer->tokens);
  assert(parser);

  klt_parser_parse_program(parser);

  klt_interpreter* interpreter =
      klt_interpreter_make(parser->functions, native_functions);
  assert(interpreter);

  klt_parser_delete(parser);
  klt_interpreter_delete(interpreter);
  klt_lexer_delete(lexer);
  klt_native_end();
  return true;
}

klt_bool run(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Usage: %s <action> <file>\n", argv[0]);
    printf("Use %s help for more info.\n", argv[0]);
    return false;
  }
  klt_str action = argv[1];
  if (klt_str_equals(action, "run")) {
    // TODO: SUPPORT MULTIPLES FILES
    if (argc < 3) {
      klt_error_fatal("Please provide at least 1 file!");
      return false;
    }
    klt_file* file = klt_file_open(argv[2], FILE_MODE_READ);
    if (file == NULL) {
      klt_error_fatal("Failed to open %s", argv[2]);
      return false;
    }
    klt_str src = klt_file_read_text(file);
    if (src == NULL) {
      klt_error_fatal("Failed to read %s", argv[2]);
      return false;
    }
    klt_bool interRes = interpret(src);
    free(src);
    klt_file_close(file);
    return interRes;
  } else if (klt_str_equals(action, "help")) {
    printf("%s help        : prints help\n", argv[0]);
    printf("%s run <files> : executes an kilate file.\n", argv[0]);
    return true;
  }
  return false;
}

int main(int argc, char* argv[]) {
  klt_bool runRes = run(argc, argv);
  return runRes ? 0 : 1;
}