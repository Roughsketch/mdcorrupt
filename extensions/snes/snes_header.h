#ifndef _SNES_HEADER_H
#define _SNES_HEADER_H

#include <algorithm>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <memory>

#include <cstdint>

#include "snes_except.h"


namespace SNESOffset
{
  enum SNES
  {
    Name            = 0x00,
    Layout          = 0x15,
    Type            = 0x16,
    ROM_Size        = 0x17,
    RAM_Size        = 0x18,
    Country         = 0x19,
    License         = 0x1a,
    Version         = 0x1b,
    Complement      = 0x1c,
    Checksum        = 0x1e,
    Native_Int      = 0x23,
    Emulation_Int   = 0x34,
    Reset           = 0x3c
  };
}

struct SNESHeader
{
  static const int Header_Size = 64;

  SNESHeader();
  SNESHeader(std::vector<uint8_t> &rom);
  ~SNESHeader();

  void initialize(std::vector<uint8_t>&);

  bool smc_header();

  std::string name();
  uint8_t layout();
  uint8_t type();
  uint32_t rom_size();
  uint32_t ram_size();
  uint8_t country();
  uint8_t license_code();
  uint8_t version();

  uint32_t offset();
  uint32_t begin();

  std::vector<uint8_t> native_interrupts();
  std::vector<uint8_t> emulation_interrupts();
private:
  std::vector<uint8_t> header;
  bool has_smc;
  uint32_t header_offset;

  static uint32_t score_header(std::vector<uint8_t>& rom, uint32_t address);
  static inline uint16_t create_uint16(std::vector<uint8_t>& rom, uint32_t address);
};

#endif
