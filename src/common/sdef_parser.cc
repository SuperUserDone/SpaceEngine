#include "common/sdef_parser.hh"
#include "common/memory_arena.hh"

#include <ctype.h>
#include <stdio.h>
#include <vcruntime_string.h>

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

      if (base[out_len].type == TOKEN_NONE) {
        base[out_len].type = TOKEN_INTEGER;
      }

      if (base[out_len].type == TOKEN_INTEGER || base[out_len].type == TOKEN_IDENTIFIER) {
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

bool parse_property(mem_arena &arena, sdef_property *p, token *tokens, int &i) {
  if (tokens[i].type == TOKEN_IDENTIFIER) {
    p->name = arena_push_array(arena, char, tokens[i].len + 1);
    memcpy(p->name, tokens[i].start, tokens[i].len);
    p->name[tokens[i].len] = 0;

    i++;

    if (tokens[i].type == TOKEN_INTEGER) {
      p->type = SDEF_TYPE_INTEGER;
      p->integer = atoi(tokens[i].start);

      i++;
      return true;
    } else if (tokens[i].type == TOKEN_STRING) {
      p->type = SDEF_TYPE_STRING;
      p->string = arena_push_array(arena, char, tokens[i].len + 1);

      memcpy(p->string, tokens[i].start, tokens[i].len);

      p->string[tokens[i].len] = 0;

      i++;
      return true;
    } else if (tokens[i].type == TOKEN_LSQUARE) {
      i++;
      p->type = SDEF_TYPE_STRING_ARRAY;


      p->string_array = (char *)arena_push(arena, 0);
      p->array_count = 0;

      while (1) {
        if (tokens[i].type == TOKEN_COMMA)
          i++;
        if (tokens[i].type == TOKEN_RSQUARE) {
          i++;
          break;
        }

        if (tokens[i].type == TOKEN_STRING) {
          char *arr = arena_push_array(arena, char, tokens[i].len + 1);
          memcpy(arr, tokens[i].start, tokens[i].len);
          arr[tokens[i].len] = 0;
          p->array_count++;
        }

        i++;
      }

      return true;
    }
  }
  return false;
}

size_t count_properties(token *tokens, int i) {
  size_t count = 0;

  while (1) {
    if (tokens[i].type == TOKEN_IDENTIFIER) {
      i++;
      if (tokens[i].type == TOKEN_INTEGER || tokens[i].type == TOKEN_STRING) {
        count++;
        i++;
        continue;
      } else if (tokens[i].type == TOKEN_LSQUARE) {

        while (tokens[i++].type != TOKEN_RSQUARE)
          ;

        count++;
        continue;
      }
    }
    break;
  }

  return count;
}

bool parse_block(mem_arena &arena, sdef_block *block, token *tokens, int &i) {
  if (tokens[i].type == TOKEN_LSQUARE &&        //
      tokens[i + 1].type == TOKEN_IDENTIFIER && //
      tokens[i + 2].type == TOKEN_RSQUARE &&    //
      tokens[i + 3].type == TOKEN_IDENTIFIER) {

    block->type = arena_push_array(arena, char, tokens[i + 1].len + 1);
    memcpy(block->type, tokens[i + 1].start, tokens[i + 1].len);
    block->name = arena_push_array(arena, char, tokens[i + 3].len + 1);
    memcpy(block->name, tokens[i + 3].start, tokens[i + 3].len);

    block->type[tokens[i + 1].len] = 0;
    block->name[tokens[i + 3].len] = 0;

    i += 4;

    block->property_count = count_properties(tokens, i);
    block->properties = arena_push_array(arena, sdef_property, block->property_count);

    for (int j = 0; j < block->property_count; j++) {
      if (!parse_property(arena, &block->properties[j], tokens, i))
        return false;
    }

    return true;
  }
  return false;
}

sdef_dom *parse_file(mem_arena &arena, token *tokens, size_t token_count) {
  sdef_dom *n = arena_push_struct(arena, sdef_dom);
  n->block_count = 0;

  for (int i = 0; i < token_count; i++) {
    if (token_count - i > 3 &&                    //
        tokens[i].type == TOKEN_LSQUARE &&        //
        tokens[i + 1].type == TOKEN_IDENTIFIER && //
        tokens[i + 2].type == TOKEN_RSQUARE) {
      n->block_count++;
    }
  }

  int token_index = 0;

  n->blocks = arena_push_array(arena, sdef_block, n->block_count);

  for (int i = 0; i < n->block_count; i++) {
    if (!parse_block(arena, &n->blocks[i], tokens, token_index))
      return nullptr;
  }

  return n;
}

sdef_dom *sdef_parse(mem_arena &arena, mem_arena &temp_arena, const char *str, size_t len) {
  size_t token_count = 0;
  token *tokens = get_tokens(temp_arena, str, len, token_count);

  print_tokens(tokens, token_count);

  return parse_file(arena, tokens, token_count);
}
