#include "arena.hh"

#ifdef _WIN32
#include <windows.h>
#endif

namespace pyro {
namespace memory {

#ifdef _WIN32

static size_t page_size = 0x0;

void arena::lt_init(size_t max_size) {
  m_base = nullptr;
  m_size = 0;
  m_grow_semaphore = false;

  // We need to keep track of the system page size so lazy-initialize it if it is not already
  // initialized
  if (page_size == 0) {
    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);
    if (sys_info.dwPageSize > 0)
      page_size = sys_info.dwPageSize;
    else // Should never happen, but just incase use the default on x86
      page_size = 4096;
  }

  // Round the max size to a multiple of the page size.
  m_max_size = ((max_size + page_size - 1) / page_size) * page_size;

  m_allocated_size = page_size;
  // We reserve the page, but dont allocate it yet. Dont allow execution
  m_base = VirtualAlloc(nullptr, m_max_size, MEM_RESERVE, PAGE_NOACCESS);

  SPACE_ASSERT(m_base, "Could not allocate space for arena!");

  // Allocate the first page as a optimization. Will grow the arena if more space is requested
  grow(page_size);
}

void arena::grow(size_t min_size) {
  // Round the size to the next page size
  size_t alloc = ((min_size + page_size - 1) / page_size) * (page_size);

  // Check if we really need to grow the arena
  if (alloc < m_allocated_size)
    return;

  // Spin until all growth opperations are completed in other threads
  while (m_grow_semaphore.load())
    _mm_pause();

  // Might be a bug here, should be a compare and exchange? Maybe we can force ordering with a flag?
  m_grow_semaphore.store(true);

  SPACE_ASSERT(alloc <= m_max_size, "Tried to grow arena beyond its capacity!");

  // Now we can commit the already reserved memory MAKE SURE IT IS RW NOT RWX
  void *a = VirtualAlloc(m_base, alloc, MEM_COMMIT, PAGE_READWRITE);

  m_allocated_size = alloc;

  SPACE_ASSERT(a, "Failed to commit arena memory!")

  m_grow_semaphore.store(false);
}

void arena::lt_done() {
  SPACE_ASSERT(VirtualFree(m_base, 0, MEM_RELEASE), "Failed to free arena memory!");

  m_base = nullptr;
  m_size = 0;
  m_allocated_size = 0;
  m_max_size = 0;
  m_grow_semaphore = false;
}
}
} // namespace pyro

#endif