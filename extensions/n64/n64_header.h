#ifndef _N64_HEADER_H
#define _N64_HEADER_H

#include <vector>
#include <cstdint>

#include "n64_except.h"

#include "util.h"

namespace N64Offset
{
  enum N64
  {
    Domain          = 0x00,
    ClockRate       = 0x04,
    ProgramCounter  = 0x08,
    Release         = 0x0C,
    CRC1            = 0x10,
    CRC2            = 0x14,
    Name            = 0x20,
    Format          = 0x38,
    ID              = 0x3C,
    Region          = 0x3E,
    Version         = 0x3F
  };
}

class N64Header
{
public:
  static const uint32_t Size = 0x1000;

  N64Header();
  N64Header(std::vector<uint8_t>& rom);

  void initialize(std::vector<uint8_t>& rom);

  uint32_t domain();
  uint32_t clock_rate();
  uint32_t program_counter();
  uint32_t release();
  uint32_t crc1();
  uint32_t crc2();
  std::string name();
  char format();
  uint16_t id();
  uint8_t region();
  double version();

  bool is_little_endian();
  bool is_big_endian();
private:
  std::vector<uint8_t> header;
};

#endif
