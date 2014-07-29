#ifndef _TIMER_H
#define _TIMER_H

#include <chrono>

class Timer
{
public:
  Timer()
  {
    m_start = std::chrono::high_resolution_clock::now();
  }

  inline uint64_t current()
  {
    m_end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(m_end - m_start).count();
  }

  inline void start()
  {
    m_start = std::chrono::high_resolution_clock::now();
  }
private:
  std::chrono::time_point<std::chrono::system_clock> m_start;
  std::chrono::time_point<std::chrono::system_clock> m_end;
};

#endif
