#include "genesis_header.h"

GenesisHeader::GenesisHeader()
{

}

GenesisHeader::GenesisHeader(std::vector<uint8_t>& rom)
{
  initialize(rom);
}

GenesisHeader::~GenesisHeader()
{

}

void GenesisHeader::initialize(std::vector<uint8_t>& rom)
{
  header.resize(0x200);
  std::copy(rom.begin(), rom.begin() + 0x200, header.begin());
}