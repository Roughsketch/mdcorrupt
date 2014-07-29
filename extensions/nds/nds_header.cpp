#include "nds_header.h"

NDSHeader::NDSHeader()
{

}

NDSHeader::NDSHeader(std::vector<uint8_t>& rom)
{
  this->initialize(rom);
}

NDSHeader::~NDSHeader() 
{

}

void NDSHeader::initialize(std::vector<uint8_t>& rom)
{
  //  Move the raw header into the structure
  this->header.resize(NDSHeader::Header_Size);
  std::copy(rom.begin(), rom.begin() + NDSHeader::Header_Size, this->header.begin());
}
