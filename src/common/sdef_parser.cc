#include "common/sdef_parser.hh"
#include "common/result.hh"
#include "memory/memory_scratch_arena.hh"

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
    return result_err<token>("Expected token %s but got type %s with value %.*s line %d col %d.",
                             get_token_type_string(type),
                             get_token_type_string(tokens[i].type),
                             tokens[i].len,
                             tokens[i].start,
                             tokens[i].line_num,
                             tokens[i].col);
}

static inline bool peek(token_type type, token *tokens, int i) {
  return tokens[i].type == type;
}

static inline token_type peek_type(token *tokens, int i) {
  return tokens[i].type;
}

static inline result<token *> get_tokens(pyro::memory::arena &temp,
                                         const char *str,
                                         size_t len,
                                         size_t &out_len) {
  token *base = temp.push<token>();
  base[out_len].type = TOKEN_NONE;
  base[out_len].len = 0;
  base[out_len].start = str;
  base[out_len].line_num = 1;
  base[out_len].col = 1;

  out_len = 0;

  size_t line = 1;
  size_t col = 1;

  for (int i = 0; i < len; i++, col++) {
    // Set the col in the event it changed in the last itteration.
    base[out_len].col = col;

    // Parse out integers
    while (i < len && (str[i] >= '0' && str[i] <= '9')) {
      base[out_len].type = TOKEN_INTEGER;
      base[out_len].len++;
      i++;
      col++;
    }

    // We need to exclude already identified tokens here to avoid overwriting existing data
    if (base[out_len].type != TOKEN_INTEGER) {

      // This needs to happen after the integer as we dont want it identifing an integer as an
      // identifier
      while (i < len && ((str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z') ||
                         (str[i] >= '0' && str[i] <= '9') || (str[i] == '_'))) {
        base[out_len].type = TOKEN_IDENTIFIER;
        base[out_len].len++;
        i++;
        col++;
      }

      // We need to exclude already identified tokens here to avoid overwriting exisitng data
      if (base[out_len].type != TOKEN_IDENTIFIER) {
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
    }

    // Avoid overshooting integers and identifiers
    if (base[out_len].type == TOKEN_INTEGER || base[out_len].type == TOKEN_IDENTIFIER) {
      i--;
      col--;
    }

    temp.push<token>();
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

static inline result<> parse_property(pyro::memory::arena &arena,
                                      sdef_property *p,
                                      token *tokens,
                                      int &i) {
  result_forward_err(id, eat(TOKEN_IDENTIFIER, tokens, i));

  p->name = arena.push_cstring(id.start, id.len);

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
    p->string = arena.push_cstring(string.start, string.len);

    return result_ok(true);
  }
  case TOKEN_LSQUARE: {
    result_forward_err(_, eat(TOKEN_LSQUARE, tokens, i));
    p->type = SDEF_TYPE_STRING_ARRAY;

    p->string_array = (char *)arena.push(0);
    p->array_count = 0;
    p->total_len = 0;

    // Read all the elements in the array
    // Both [ "a" "b" "c" ] and ["a", "b", "c"] are valid
    while (1) {
      if (peek(TOKEN_COMMA, tokens, i)) {
        result_forward_err(_, eat(TOKEN_COMMA, tokens, i));
      }

      if (peek(TOKEN_RSQUARE, tokens, i)) {
        result_forward_err(_, eat(TOKEN_RSQUARE, tokens, i));
        break;
      }

      result_forward_err(string, eat(TOKEN_STRING, tokens, i));
      char *arr = arena.push_cstring(string.start, string.len);

      p->total_len += string.len + 1;
      p->array_count++;
    }

    return result_ok(true);
  }

  default:
    return result_err(
        "Unexpected token %s at %d:%d! Expected TOKEN_INTEGER, TOKEN_STRING or TOKEN_LSQUARE!",
        get_token_type_string(peek_type(tokens, i)),
        tokens[i].line_num,
        tokens[i].col);
    break;
  }
}

static inline size_t count_properties(token *tokens, int i) {
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

static inline result<> parse_block(pyro::memory::arena &arena,
                                   sdef_block *block,
                                   token *tokens,
                                   int &i) {
  result_forward_err(_l, eat(TOKEN_LSQUARE, tokens, i));
  result_forward_err(type, eat(TOKEN_IDENTIFIER, tokens, i));
  result_forward_err(_r, eat(TOKEN_RSQUARE, tokens, i));
  result_forward_err(name, eat(TOKEN_IDENTIFIER, tokens, i));

  block->type = arena.push_cstring(type.start, type.len);
  block->name = arena.push_cstring(name.start, name.len);

  // Not the best design in the world, but we count the number of props to allocate them in a
  // contiguous block
  size_t props = count_properties(tokens, i);
  block->properties.lt_init(arena, props);

  for (int j = 0; j < props; j++) {
    result_forward_err(_e, parse_property(arena, &block->properties[j], tokens, i));
  }

  return result_ok(true);
}

static inline result<sdef_dom *> parse_file(pyro::memory::arena &arena,
                                            token *tokens,
                                            size_t token_count) {
  sdef_dom *n = arena.push<sdef_dom>();

  size_t block_count = 0;

  for (int i = 0; i < token_count; i++) {
    // Not the best design in the world, but we count the number of blocks to allocate them in a
    // contiguous block
    if (token_count - i > 3 &&                          //
        peek_type(tokens, i) == TOKEN_LSQUARE &&        //
        peek_type(tokens, i + 1) == TOKEN_IDENTIFIER && //
        peek_type(tokens, i + 2) == TOKEN_RSQUARE &&    //
        peek_type(tokens, i + 3) == TOKEN_IDENTIFIER) {
      block_count++;
    }
  }

  int token_index = 0;

  n->blocks.lt_init(arena, block_count);

  for (int i = 0; i < block_count; i++) {
    result_forward_err(_err, parse_block(arena, &n->blocks[i], tokens, token_index));
  }

  return result_ok(n);
}

result<sdef_dom *> sdef_parse(pyro::memory::arena &arena, const char *str, size_t len) {
  size_t token_count = 0;
  mem_scratch_arena temp_arena = arena_scratch_get();

  token *tokens = get_tokens(temp_arena, str, len, token_count);

  result<sdef_dom *> dom = parse_file(arena, tokens, token_count);

  arena_scratch_free(temp_arena);

  return dom;
}
