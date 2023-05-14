#include "common/sdef_parser.hh"
#include "common/memory_arena.hh"
#include "common/result.hh"

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

  size_t line_num;
  size_t col;
};

constexpr const char *get_token_type_string(token_type type) {
  switch (type) {
  case TOKEN_NONE:
    return "TOKEN_NONE";
  case TOKEN_LSQUARE:
    return "TOKEN_LSQUARE";
  case TOKEN_RSQUARE:
    return "TOKEN_RSQUARE";
  case TOKEN_COMMA:
    return "TOKEN_COMMA";
  case TOKEN_STRING:
    return "TOKEN_STRING";
  case TOKEN_IDENTIFIER:
    return "TOKEN_IDENTIFIER";
  case TOKEN_INTEGER:
    return "TOKEN_INTEGER";
  case TOKEN_EOF:
    return "TOKEN_EOF";
    break;
  }
}

static inline result<token> eat(token_type type, token *tokens, int &i) {
  if (tokens[i].type == type)
    return result_ok(tokens[i++]);
  else
    return result_err<token>("Expected token %s but got type %s.",
                             get_token_type_string(type),
                             get_token_type_string(tokens[i].type));
}

static inline bool peek(token_type type, token *tokens, int i) {
  return tokens[i].type == type;
}

static inline token_type peek_type(token *tokens, int i) {
  return tokens[i].type;
}

result<token *> get_tokens(mem_arena &temp, const char *str, size_t len, size_t &out_len) {
  token *base = arena_push_struct(temp, token);
  base[out_len].type = TOKEN_NONE;
  base[out_len].len = 0;
  base[out_len].start = str;
  base[out_len].line_num = 1;
  base[out_len].col = 1;

  out_len = 0;

  size_t line = 1;
  size_t col = 1;

  for (int i = 0; i < len; i++, col++) {

    base[out_len].col = col;

    while (i < len && (str[i] >= '0' && str[i] <= '9')) {
      base[out_len].type = TOKEN_INTEGER;
      base[out_len].len++;
      i++;
      col++;
    }

    while (i < len && ((str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z') ||
                       (str[i] >= '0' && str[i] <= '9') || (str[i] == '_'))) {
      base[out_len].type = TOKEN_IDENTIFIER;
      base[out_len].len++;
      i++;
      col++;
    }

    if (base[out_len].type != TOKEN_INTEGER && base[out_len].type != TOKEN_IDENTIFIER) {
      switch (str[i]) {
      case '\n':
        line++;
        col = 0;
        base[out_len].line_num = line;
      case ' ':
      case '\t':
      case '\r':
        base[out_len].start = str + i + 1;
        continue;
      case '[':
        base[out_len].type = TOKEN_LSQUARE;
        base[out_len].len = 1;
        break;
      case ']':
        base[out_len].type = TOKEN_RSQUARE;
        base[out_len].len = 1;
        break;
      case ',':
        base[out_len].type = TOKEN_COMMA;
        base[out_len].len = 1;
        break;
      case '"':
        base[out_len].type = TOKEN_STRING;
        base[out_len].len = 0;
        i++;
        col++;
        base[out_len].start = str + i;
        while (i < len && str[i] != '"') {
          base[out_len].len++;
          i++;
          col++;
        }
        break;
      default:
        return result_err<token *>("Lexer: unknown character %c at %d:%d", str[i], line, col);
        break;
      }
    }

    if (base[out_len].type == TOKEN_INTEGER || base[out_len].type == TOKEN_IDENTIFIER) {
      i--;
      col--;
    }

    arena_push_struct(temp, token);
    out_len++;
    base[out_len].type = TOKEN_NONE;
    base[out_len].len = 0;
    base[out_len].start = str + i + 1;
    base[out_len].line_num = line;
    base[out_len].col = col;
  }

  base[out_len].type = TOKEN_EOF;
  out_len++;

  return result_ok(base);
}

void print_tokens(token *tokens, size_t n) {
  for (int i = 0; i < n; i++) {
    switch (tokens[i].type) {
    case TOKEN_NONE:
      printf("TOKEN_NONE %d:%d %.*s\n",
             (int)tokens[i].line_num,
             (int)tokens[i].col,
             (int)tokens[i].len,
             tokens[i].start);
      break;
    case TOKEN_COMMA:
      printf("TOKEN_COMMA %d:%d %.*s\n",
             (int)tokens[i].line_num,
             (int)tokens[i].col,
             (int)tokens[i].len,
             tokens[i].start);
      break;
    case TOKEN_LSQUARE:
      printf("TOKEN_LSQUARE %d:%d %.*s\n",
             (int)tokens[i].line_num,
             (int)tokens[i].col,
             (int)tokens[i].len,
             tokens[i].start);
      break;
    case TOKEN_RSQUARE:
      printf("TOKEN_RSQUARE %d:%d %.*s\n",
             (int)tokens[i].line_num,
             (int)tokens[i].col,
             (int)tokens[i].len,
             tokens[i].start);
      break;
    case TOKEN_STRING:
      printf("TOKEN_STRING %d:%d %.*s\n",
             (int)tokens[i].line_num,
             (int)tokens[i].col,
             (int)tokens[i].len,
             tokens[i].start);
      break;
    case TOKEN_IDENTIFIER:
      printf("TOKEN_IDENTIFIER %d:%d %.*s\n",
             (int)tokens[i].line_num,
             (int)tokens[i].col,
             (int)tokens[i].len,
             tokens[i].start);
      break;
    case TOKEN_INTEGER:
      printf("TOKEN_INTEGER %d:%d %.*s\n",
             (int)tokens[i].line_num,
             (int)tokens[i].col,
             (int)tokens[i].len,
             tokens[i].start);
      break;
    case TOKEN_EOF:
      printf("TOKEN_EOF %d:%d %.*s\n",
             (int)tokens[i].line_num,
             (int)tokens[i].col,
             (int)tokens[i].len,
             tokens[i].start);
      break;
    }
  }
}

result<> parse_property(mem_arena &arena, sdef_property *p, token *tokens, int &i) {
  result_forward_err(id, eat(TOKEN_IDENTIFIER, tokens, i));

  p->name = arena_push_array(arena, char, id.len + 1);
  memcpy(p->name, id.start, id.len);
  p->name[id.len] = 0;

  switch (peek_type(tokens, i)) {
  case TOKEN_INTEGER: {
    result_forward_err(integer, eat(TOKEN_INTEGER, tokens, i));

    p->type = SDEF_TYPE_INTEGER;
    p->integer = atoi(integer.start);

    return result_ok(true);
  }
  case TOKEN_STRING: {
    result_forward_err(string, eat(TOKEN_STRING, tokens, i));

    p->type = SDEF_TYPE_STRING;
    p->string = arena_push_array(arena, char, string.len + 1);

    memcpy(p->string, string.start, string.len);

    p->string[string.len] = 0;

    return result_ok(true);
  }

  case TOKEN_LSQUARE: {
    result_forward_err(_, eat(TOKEN_LSQUARE, tokens, i));
    p->type = SDEF_TYPE_STRING_ARRAY;

    p->string_array = (char *)arena_push(arena, 0);
    p->array_count = 0;

    while (1) {
      if (peek(TOKEN_COMMA, tokens, i)) {
        result_forward_err(_, eat(TOKEN_COMMA, tokens, i));
      }

      if (peek(TOKEN_RSQUARE, tokens, i)) {
        result_forward_err(_, eat(TOKEN_RSQUARE, tokens, i));
        break;
      }

      result_forward_err(string, eat(TOKEN_STRING, tokens, i));
      char *arr = arena_push_array(arena, char, string.len + 1);
      memcpy(arr, string.start, string.len);
      arr[string.len] = 0;
      p->array_count++;
    }

    return result_ok(true);
  }

  default:
    return result_err("Unexpected token %s! Expected TOKEN_INTEGER, TOKEN_STRING or TOKEN_LSQUARE!",
                      get_token_type_string(peek_type(tokens, i)));
    break;
  }
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

result<> parse_block(mem_arena &arena, sdef_block *block, token *tokens, int &i) {
  result_forward_err(_l, eat(TOKEN_LSQUARE, tokens, i));
  result_forward_err(type, eat(TOKEN_IDENTIFIER, tokens, i));
  result_forward_err(_r, eat(TOKEN_RSQUARE, tokens, i));
  result_forward_err(name, eat(TOKEN_IDENTIFIER, tokens, i));

  block->type = arena_push_array(arena, char, type.len + 1);
  memcpy(block->type, type.start, type.len);
  block->name = arena_push_array(arena, char, name.len + 1);
  memcpy(block->name, name.start, name.len);

  block->type[type.len] = 0;
  block->name[name.len] = 0;

  block->property_count = count_properties(tokens, i);
  block->properties = arena_push_array(arena, sdef_property, block->property_count);

  for (int j = 0; j < block->property_count; j++) {
    result_forward_err(_e, parse_property(arena, &block->properties[j], tokens, i));
  }

  return result_ok(true);
}

result<sdef_dom *> parse_file(mem_arena &arena, token *tokens, size_t token_count) {
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
    result_forward_err(_err, parse_block(arena, &n->blocks[i], tokens, token_index));
  }

  return result_ok(n);
}

result<sdef_dom *> sdef_parse(mem_arena &arena, mem_arena &temp_arena, const char *str, size_t len) {
  size_t token_count = 0;
  token *tokens = get_tokens(temp_arena, str, len, token_count);

  return parse_file(arena, tokens, token_count);
}
