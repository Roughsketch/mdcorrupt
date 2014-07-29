#ifndef _NINTENDO_DS_DIRECTORY_H
#define _NINTENDO_DS_DIRECTORY_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>

class NDSEntry
{
public:
  NDSEntry();
  NDSEntry(uint32_t offset, uint32_t size) : m_offset(offset), m_size(size) {};
  ~NDSEntry();

  std::vector<uint8_t> contents(std::vector<uint8_t>& rom);
  void write(std::vector<uint8_t>& rom, std::vector<uint8_t>& data);

  std::string name();
  uint32_t offset();
  uint32_t size();
private:
  uint32_t m_offset;
  uint32_t m_size;
};

inline uint32_t NDSEntry::offset()
{
  return m_offset;
}

inline uint32_t NDSEntry::size()
{
  return m_size;
}

#endif
