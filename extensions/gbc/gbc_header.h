#ifndef _GAMEBOY_COLOR_HEADER_H
#define _GAMEBOY_COLOR_HEADER_H

/*
  Header info obtained from here:

  http://nocash.emubase.de/pandocs.htm#thecartridgeheader
*/

#include <vector>
#include <cstdint>

#include "gbc_except.h"
#include "util.h"

namespace GBCOffset
{
  //  Info for the comments and most of the implementation gathered from reading
  //  the following reference: http://nocash.emubase.de/pandocs.htm#thecartridgeheader
  enum GBC
  {
    Start           = 0x100,  //  Start of the header. Generally NOP | JP $xxxx
    Entry           = 0x102,  //  Offset to start of ROM code
    Logo            = 0x104,  //  Nintendo Logo
    Title           = 0x134,  //  Title of ROM
    Manufacturer    = 0x13F,  //  If CGB is set then this holds 4 characters of the manufacturer code
    CGB             = 0x143,  //  Enabled CGB functions
    Company         = 0x144,  //  Two bytes indicating the company or publisher
    SGB             = 0x146,  //  Enabled SGB functions
    Type            = 0x147,  //  Cartridge type - specifications commented in the accessor
    RomSize         = 0x148,  //  Rom size - specifications commented in the accessor
    RamSize         = 0x149,  //  Ram size - specifications commented in the accessor
    Destination     = 0x14A,  //  0 is sold in Japan or 1 otherwise.
    OldLicense      = 0x14B,  //  Company or publisher code. If 0x33 then use Company offset instead.
    Version         = 0x14C,  //  Verison of the game
    HeaderChecksum  = 0x14D,  //  8 bit checksum of header from 0x134 to 0x14C [x=0:FOR i=0134h TO 014Ch:x=x-MEM[i]-1:NEXT]
    GloablChecksum  = 0x14E,  //  Unverified global checksum of the ROM
    End             = 0x14F   //  End of the header
  };
}

namespace GBCLocal
{
  //  Info for the comments and most of the implementation gathered from reading
  //  the following reference: http://nocash.emubase.de/pandocs.htm#thecartridgeheader
  enum GBC
  {
    Start           = 0x00,  //  Start of the header. Generally NOP | JP $xxxx
    Entry           = 0x02,  //  Offset to start of ROM code
    Logo            = 0x04,  //  Nintendo Logo
    Title           = 0x34,  //  Title of ROM
    Manufacturer    = 0x3F,  //  If CGB is set then this holds 4 characters of the manufacturer code
    CGB             = 0x43,  //  Enabled CGB functions
    Company         = 0x44,  //  Two bytes indicating the company or publisher
    SGB             = 0x46,  //  Enabled SGB functions
    Type            = 0x47,  //  Cartridge type - specifications commented in the accessor
    RomSize         = 0x48,  //  Rom size - specifications commented in the accessor
    RamSize         = 0x49,  //  Ram size - specifications commented in the accessor
    Destination     = 0x4A,  //  0 is sold in Japan or 1 otherwise.
    OldLicense      = 0x4B,  //  Company or publisher code. If 0x33 then use Company offset instead.
    Version         = 0x4C,  //  Verison of the game
    HeaderChecksum  = 0x4D,  //  8 bit checksum of header from 0x34 to 0x4C [x=0:FOR i=0134h TO 014Ch:x=x-MEM[i]-1:NEXT]
    GloablChecksum  = 0x4E,  //  Unverified global checksum of the ROM
    End             = 0x4F   //  End if the header.
  };
}

class GBCHeader
{
public:
  //  0x0000 - 0x0100 - Boot code
  //  0x0100 - 0x014F - Header
  //  0x0150 - 0xXXXX - ROM
  static const uint32_t Start = 0x150;
  static const uint32_t LogoSize = 0x30;

  GBCHeader();
  GBCHeader(std::vector<uint8_t>& rom);

  void initialize(std::vector<uint8_t>& rom);
  
  uint16_t entry();
  bool valid_logo();
  std::string name();
  uint32_t manufacturer();
  uint8_t cgb();
  uint16_t company();
  uint8_t sgb();
  uint8_t type();
  uint8_t rom_size();
  uint8_t ram_size();
  uint8_t destination();
  uint8_t old_license();

private:
  std::vector<uint8_t> header;
};

#endif
