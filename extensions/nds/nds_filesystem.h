#ifndef _NINTENDO_DS_FILESYSTEM_H
#define _NINTENDO_DS_FILESYSTEM_H

#include <vector>
#include <map>

#include "nds_entry.h"
#include "nds_except.h"
#include "nds_header.h"

#include "util.h"

class NDSFileSystem
{
public:
  NDSFileSystem();
  NDSFileSystem(std::vector<uint8_t>& rom, std::unique_ptr<NDSHeader>& header);
  ~NDSFileSystem();

  void initialize(std::vector<uint8_t>& rom, std::unique_ptr<NDSHeader>& header);

  NDSEntry get(std::string file);
  std::string to_json();
private:
  std::map<std::string, NDSEntry> m_entries;
};

inline NDSEntry NDSFileSystem::get(std::string file)
{
  if (this->m_entries.count(file))
  {
    return this->m_entries.at(file);
  }

  throw NDSFileNotFoundException("Could not find " + file);
}

class NDSMainTableEntry
{
public:
  NDSMainTableEntry(std::vector<uint8_t>& data, uint32_t offset, int32_t prev_start_id = 0)
  {
    m_offset = util::read<uint32_t>(data, offset);
    m_start_id = util::read<uint16_t>(data, offset + 4);
    m_parent_id = util::read<uint16_t>(data, offset + 6) &  0x0FFF;

    std::cout << "MTE: " << std::hex << m_offset << "\t" << m_start_id << "\t" << m_parent_id << std::dec << std::endl;
  }

  uint32_t offset();
  uint16_t start_id();
  uint16_t parent_id();

private:
  uint32_t m_offset;
  uint16_t m_start_id;
  uint16_t m_parent_id;
};

inline uint32_t NDSMainTableEntry::offset()
{
  return m_offset;
}

inline uint16_t NDSMainTableEntry::start_id()
{
  return m_start_id;
}

inline uint16_t NDSMainTableEntry::parent_id()
{
  return m_parent_id;
}

#endif
