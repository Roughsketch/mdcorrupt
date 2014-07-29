#ifndef _SNES_CORRUPTION_H
#define _SNES_CORRUPTION_H

/*
  Basic Information:
    This file is for Super Nintendo Entertainment System corruptions.
    The CPU used inside the SNES is a 16-bit Ricoh 5A22 utilizing
    the 65c816 instruction set.

    A list of opcodes and their forms can be found here:
    http://pastebin.com/CjSsr31z

    A list of important register values can be found here:
    http://wiki.superfamicom.org/snes/show/Registers

    A lot of information used in determining how to code layout and header
    stuff was from these places:
    http://romhack.wikia.com/wiki/SNES_ROM_layout
    http://romhack.wikia.com/wiki/SNES_header
    http://romhack.wikia.com/wiki/SMC_header

=============================================================
  Changelog:

  1.0
    Corruptions are working very rarely. Yoshi's Island 100% crash.

  1.1
    Added invalidations for branch and stack operations.

    Corruptions still crash often, Yoshi's Island 100% crash.

  1.2
    Added register checks and invalidations for the STA family of opcodes.

    Corruptions crash significantly less often, Yoshi's Island works sometimes.


  TODO:
    Figure out how to prevent switching to Mode 7 from causing a 100% crash.
=============================================================

*/
#include "corrupt.h"
#include "snes_except.h"
#include "snes_header.h"

#include <string>
#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "helpers.h"

class SNESCorruption : public Corruption
{
public:
  SNESCorruption();
  SNESCorruption(std::string filename, std::vector<std::string>& args);
  ~SNESCorruption();

  virtual void initialize(std::string filename, std::vector<std::string>& args);
  virtual bool valid();
  virtual bool valid_byte(uint8_t byte, uint32_t location);
  virtual void print_header();
  virtual void save(std::string filename);

private:
  std::unique_ptr<SNESHeader> header;

  static bool is_register(uint16_t value);
};


/*
Determines whether a given 16 bit address corresponds to an SNES register.
The list of registers corresponds to any register that is marked for
write operations in this list: http://wiki.superfamicom.org/snes/show/Registers

@param addr - The address to check

@return true if value is an SNES register.
*/
inline bool SNESCorruption::is_register(uint16_t addr)
{
  //  There are over 200 different registers in the SNES but it is possible to represent all of them
  //  using generalized bitwise compares instead of writing out every one of them. On top of not
  //  having to waste time or memory making a vector it is possible to inline the code in one expression.

  //  The following bitwise is true for anything in the following ranges:
  //  $21(0-2)x || $213(0-3) || $2137 || $214(0-4 & !2) || $218(0-3) || $4016 || $420(0-D) || $43x(0-A)

  return   (addr >= 0x2100 && addr <= 0x212F)
    || (addr >= 0x2130 && addr <= 0x2133)
    || (addr == 0x2137)
    || (addr >= 0x2140 && addr <= 0x2144 && addr != 0x2142)
    || (addr >= 0x2180 && addr <= 0x2183)
    || (addr == 0x4016)
    || (addr >= 0x4200 && addr <= 0x420D)
    || (addr >= 0x4300 && addr <= 0x4400 && (addr & 0xF) <= 0xA);
}

#endif
