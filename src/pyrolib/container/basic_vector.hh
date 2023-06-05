template <typename T>
class basic_vector {
public:
  typedef T *iterator;

  T &operator[](size_t index) {
    return m_base[index];
  }

  iterator begin() {
    return m_base;
  }

  iterator end() {
    return &m_base[m_size];
  }

  size_t size() {
    return m_size;
  }

  void clear() {
    m_size = 0;
  }

protected:
  T *m_base = nullptr;
  size_t m_size = 0;
};