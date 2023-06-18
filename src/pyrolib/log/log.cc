#include "log.hh"
#include "pyrolib/memory/scratch_arena.hh"
#include "pyrolib/container/string_buffer.hh"

#include <filesystem>

namespace pyro {
namespace log {
static container::string_buffer g_log_buf;
FILE *fp;

void buf_flush() {
  memory::scratch_arena sa = memory::scratch_get();
  const char *a = g_log_buf.read_all(sa);
  fputs(a, fp);
  fflush(fp);
  memory::scratch_free(sa);
}

void init() {
  memory::heap heap;
  g_log_buf.lt_init(heap);

  if (std::filesystem::exists("log.txt")) {
    std::filesystem::copy("log.txt",
                          "log.old.txt",
                          std::filesystem::copy_options::overwrite_existing);
  }

  fopen_s(&fp, "log.txt", "w");
}

void done() {
  buf_flush();
  memory::heap heap;
  g_log_buf.lt_done(heap);
  fclose(fp);
}

void write(const char *str) {
  if (!g_log_buf.write(str)) {
    buf_flush();
    g_log_buf.write(str);
  }
}

PYROAPI char *temp_alloc(size_t len) {
  memory::scratch_arena a = memory::scratch_get();

  container::array<char> str;
  str.lt_init(a, len + 1);

  memory::scratch_free(a);
  return &str[0];
}

} // namespace log
} // namespace pyro
