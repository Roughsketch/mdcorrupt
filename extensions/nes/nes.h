#ifndef _NES_CORRUPTION_H
#define _NES_CORRUPTION_H

/*
  Basic Information:
    This file is for Nintendo Entertainment System corruptions.
    The CPU used inside the NES is an 8-bit Ricoh 2A03 utilizing
    the 6502 instruction set.

    In-depth hardware and opcode information can be found here:
    http://nesdev.com/6502.txt

=============================================================
  Changelog:

  1.0
    CHR-ROM corruptions are working and provide great sprite corruptions.

  1.1
    PRG-ROM corruptions implemented. Crashes more likely, but better
      corruptions also more likely.

    Implemented invalidations for branch and stack opcodes.

    Added options for PRG-ROM and CHR-ROM specific corrupting.

  TODO:
    Add more corruption options.
=============================================================

*/
#include "corrupt.h"
#include "nes_except.h"
#include "nescorruptioninfo.h"

#include <cstring>

#include <iomanip>
#include <iostream>
#include <memory>
#include <limits>


class NESCorruption : public Corruption
{
public:
  NESCorruption();
  NESCorruption(std::string filename, std::vector<std::string>& args);
  ~NESCorruption();

  virtual void corrupt();
  virtual void initialize(std::string filename, std::vector<std::string>& args);
  virtual void print_header();
  virtual bool valid();
  virtual bool valid_byte(uint8_t byte, uint32_t location);
  virtual void save(std::string filename);

  void corrupt_prg();
  void corrupt_chr();
private:
  std::unique_ptr<NESCorruptionInfo> info;

  // How many PRG-ROM banks there are
  uint32_t prg_rom;

  // How many CHR-ROM banks there are
  uint32_t chr_rom;

  // Where PRG-ROM starts in the file
  uint32_t prg_start;

  // Where CHR-ROM starts in the file
  uint32_t chr_start;

  // True if rom uses CHR-ROM
  bool chr;

};

#endif
