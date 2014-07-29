#include "nds_entry.h"

NDSEntry::NDSEntry()
{
  m_offset = 0;
  m_size = 0;
}

NDSEntry::~NDSEntry()
{

}

std::vector<uint8_t> NDSEntry::contents(std::vector<uint8_t>& rom)
{
  return std::vector<uint8_t>(rom.begin() + this->offset(), rom.begin() + this->offset() + this->size());
}

void NDSEntry::write(std::vector<uint8_t>& rom, std::vector<uint8_t>& data)
{
  std::copy(rom.begin() + this->offset(), rom.begin() + this->offset() + this->size(), data.begin());
}