#include "log.hh"

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
    std::filesystem::copy("log.txt", "log.old.txt", std::filesystem::copy_options::overwrite_existing);
  }

  fopen_s(&fp, "log.txt", "w");
}

void done() {
  buf_flush();
  memory::heap heap;
  g_log_buf.lt_done(heap);
  fclose(fp);
}

void write(const container::array<char> &str) {
  if (!g_log_buf.write(str)) {
    buf_flush();
    g_log_buf.write(str);
  }
}

} // namespace log
} // namespace pyro
