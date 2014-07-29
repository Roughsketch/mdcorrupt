#ifndef _GENESIS_HEADER_H
#define _GENESIS_HEADER_H

#include <cstdint>
#include <vector>

#include "util.h"

namespace GenesisOffset
{
  enum Genesis
  {
    Name = 0x00,
    Layout = 0x15,
    Type = 0x16,
    ROM_Size = 0x17,
    RAM_Size = 0x18,
    Country = 0x19,
    License = 0x1a,
    Version = 0x1b,
    Complement = 0x1c,
    Checksum = 0x1e,
    Native_Int = 0x23,
    Emulation_Int = 0x34,
    Reset = 0x3c
  };
}

struct GenesisHeader
{
  static const int Header_Size = 64;

  GenesisHeader();
  GenesisHeader(std::vector<uint8_t> &rom);
  ~GenesisHeader();

  void initialize(std::vector<uint8_t>&);
  uint16_t checksum();
  uint32_t begin();
  uint32_t end();

  bool valid();

  //std::string console_name();
  //std::string copyright();
  //std::string domestic_name();
  //std::string overseas_name();
  //std::string game_type();
  //std::string version();
  //std::string io_support();
  //std::string countries();

private:
  std::vector<uint8_t> header;
};

inline bool GenesisHeader::valid()
{
  return util::read(header, 0x100, 4) == "SEGA";
}

inline uint16_t GenesisHeader::checksum()
{
  return util::read<uint16_t>(header, 0x18E);
}

inline uint32_t GenesisHeader::begin()
{
  return util::read<uint32_t>(header, 0x1A0) + header.size();
}

inline uint32_t GenesisHeader::end()
{
  return util::read<uint32_t>(header, 0x1A4);
}
#endif
