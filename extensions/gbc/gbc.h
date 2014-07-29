#ifndef _GAMEBOY_COLOR_CORRUPTION_H
#define _GAMEBOY_COLOR_CORRUPTION_H

/*
  Basic information:

  This file is for corruptions of Gameboy Color games which
  use a limited subset of the Z80 instruction set on the
  LR35902 CPU.

  Opcode table can be found here:
  http://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html

  Header information and general device information can be found here:
  http://nocash.emubase.de/pandocs.htm

=============================================================
  Changelog:

  1.0
    Basic corruptions working, but a good percentage of them crash often.

    Implemented invalidations for the following families of opcodes:
      Branching opcodes (jumps, calls, rets)
      Push and Pop stack opcodes

  TODO:
    Add invalidations for the LD family of opcodes to potentially
    help prevent crashing.
=============================================================
*/

#include "corrupt.h"

#include "gbc_except.h"
#include "gbc_header.h"

class GBCCorruption : public Corruption
{
public:
  GBCCorruption();
  GBCCorruption(std::string filename, std::vector<std::string>& args);
  ~GBCCorruption();

  virtual void initialize(std::string filename, std::vector<std::string>& args);
  virtual void print_header();
  virtual bool valid();
  virtual void save(std::string filename);

private:
  std::unique_ptr<GBCHeader> header;

  bool valid_byte(uint8_t byte, uint32_t location);
};

/*
  GBCorruption is defined in this plugin directory because it
  is entirely identical to GBCCorruption in all aspects except
  file extension. If there are any differences later on this
  will be moved to its own directory, but for now this acts more
  like a typedef type thing.
*/
class GBCorruption : public GBCCorruption
{

};

#endif
