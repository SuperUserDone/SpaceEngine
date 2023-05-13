#pragma once

#include "common/sdef_parser.hh"
#include "common/memory_arena.hh"

#include <ctype.h>
#include <stdio.h>

enum token_type {
  TOKEN_NONE,
  TOKEN_LSQUARE,
  TOKEN_RSQUARE,
  TOKEN_COMMA,
  TOKEN_STRING,
  TOKEN_IDENTIFIER,
  TOKEN_INTEGER,
  TOKEN_EOF,
};

struct token {
  token_type type;

  const char *start;
  size_t len;
};

token *get_tokens(mem_arena &temp, const char *str, size_t len, size_t &out_len) {
  token *base = arena_push_struct(temp, token);
  base[out_len].type = TOKEN_NONE;
  base[out_len].len = 0;
  base[out_len].start = str;
  out_len = 0;

  for (int i = 0; i < len; i++) {

    if ((str[i] >= '0' && str[i] <= '9')) {
      if (base[out_len].type == TOKEN_INTEGER || base[out_len].type == TOKEN_NONE) {
        base[out_len].type = TOKEN_INTEGER;
        base[out_len].len++;
        continue;
      }
    } else if (((str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z') ||
                str[i] == '_')) {
      if (base[out_len].type == TOKEN_IDENTIFIER || base[out_len].type == TOKEN_NONE) {
        base[out_len].type = TOKEN_IDENTIFIER;
        base[out_len].len++;
        continue;
      }
    } else if (base[out_len].type != TOKEN_INTEGER && base[out_len].type != TOKEN_IDENTIFIER) {
      if (str[i] == ' ' || str[i] == '\t' || str[i] == '\n' || str[i] == '\r') {
        base[out_len].start = str + i + 1;
        continue;
      } else {
        base[out_len].len = 1;
        if (str[i] == '[') {
          base[out_len].type = TOKEN_LSQUARE;
        } else if (str[i] == ']') {
          base[out_len].type = TOKEN_RSQUARE;
        } else if (str[i] == ',') {
          base[out_len].type = TOKEN_COMMA;
        } else if (str[i] == '"') {
          base[out_len].type = TOKEN_STRING;
          base[out_len].len = 0;
          i++;
          base[out_len].start = str + i;
          while (str[i] != '"' && i < len) {
            base[out_len].len++;
            i++;
          }

        }
      }
    } else if (base[out_len].type == TOKEN_INTEGER || base[out_len].type == TOKEN_IDENTIFIER) {
      i--;
    }

    arena_push_struct(temp, token);
    out_len++;
    base[out_len].type = TOKEN_NONE;
    base[out_len].len = 0;
    base[out_len].start = str + i + 1;
  }

  base[out_len].type = TOKEN_EOF;
  out_len++;

  return base;
}

void print_tokens(token *tokens, size_t n) {
  for (int i = 0; i < n; i++) {
    switch (tokens[i].type) {
    case TOKEN_NONE:
      printf("TOKEN_NONE %.*s\n", (int)tokens[i].len, tokens[i].start);
      break;
    case TOKEN_COMMA:
      printf("TOKEN_COMMA %.*s\n", (int)tokens[i].len, tokens[i].start);
      break;
    case TOKEN_LSQUARE:
      printf("TOKEN_LSQUARE %.*s\n", (int)tokens[i].len, tokens[i].start);
      break;
    case TOKEN_RSQUARE:
      printf("TOKEN_RSQUARE %.*s\n", (int)tokens[i].len, tokens[i].start);
      break;
    case TOKEN_STRING:
      printf("TOKEN_STRING %.*s\n", (int)tokens[i].len, tokens[i].start);
      break;
    case TOKEN_IDENTIFIER:
      printf("TOKEN_IDENTIFIER %.*s\n", (int)tokens[i].len, tokens[i].start);
      break;
    case TOKEN_INTEGER:
      printf("TOKEN_INTEGER %.*s\n", (int)tokens[i].len, tokens[i].start);
      break;
    case TOKEN_EOF:
      printf("TOKEN_EOF %.*s\n", (int)tokens[i].len, tokens[i].start);
      break;
    }
  }
}

ast_node *sdef_parse(mem_arena &arena, mem_arena &temp_arena, const char *str, size_t len) {
  size_t token_count = 0;
  token *tokens = get_tokens(temp_arena, str, len, token_count);

  print_tokens(tokens, token_count);

  ast_node *n = arena_push_struct(arena, ast_node);

  return n;
}
