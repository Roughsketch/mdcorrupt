#ifndef _IMG_PATH_H
#define _IMG_PATH_H

#include <iterator>
#include <vector>
#include <fstream>
#include <cstdint>

#include "img_entry.h"
#include "img_helper.h"

class Path
{
public:
  static const uint8_t SectionHeaderSize = 0x18;

  Path();
  Path(std::vector<uint8_t>& bytes, uint32_t offset = 0);
  Path(Path& obj);
  ~Path();

  void populate(std::fstream& img, uint32_t r_blocksize, uint32_t l_blocksize);

  uint32_t location();
  uint32_t location(uint32_t blocksize);
  uint16_t parent_index();
  std::string identifier();
  std::vector<Entry> entries();

  uint16_t size();
private:
  std::vector<uint8_t> m_raw;
  uint32_t m_location;
  uint16_t m_parent_index;
  std::string m_identifier;

  std::vector<Entry> m_entries;
};

#endif
