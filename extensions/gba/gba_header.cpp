#include "gba_header.h"

GBAHeader::GBAHeader()
{

}

GBAHeader::GBAHeader(std::vector<uint8_t>& rom)
{
  initialize(rom);
}

void GBAHeader::initialize(std::vector<uint8_t>& rom)
{
  header.resize(GBAHeader::Size);
  std::copy(rom.begin(), rom.begin() + GBAHeader::Size, header.begin());
}

bool GBAHeader::valid()
{
  return header[GBAOffset::Fixed] == 0x96;
}
