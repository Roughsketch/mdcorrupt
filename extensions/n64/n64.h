#ifndef _N64_CORRUPTION_H
#define _N64_CORRUPTION_H

/*
  Although it seemed like when I started working on this class that the
  corruptions would follow the same pattern as the NES and SNES ones it
  turns out that it is vastly more complex. There are a couple problems
  which I do not know how to resolve at the moment but I will list them
  here in hopes I can come back to it and figure them out later.

  1) ROMs may or may not be compressed.
    I found out that there are special decompressors made for popular
    games like Super Mario 64, Ocarina of Time, Majora's Mask, etc. I
    don't know if this means the opcodes themselves are in a compressed
    form or not, so whether or not opcode checking works depends on if
    they are in raw form. There is not a decompressor for every game
    and I've been told that roms that are not decompressed crash very
    often when corrupted.

  2) Every game may have a different filesystem, file types, etc.
    N64 roms are structured a lot differently than the older consoles.
    Specifically, the programmers had a lot more access to higher
    level tools which allowed them to make their own systems of files
    rather than having to conform to the console's rules. This means
    that every game will be different and there is no way to make a
    universal corrupter for all games that will work good most of the
    time. However, this means that it will still be possible to work
    on a game-by-game basis which might be good enough if I decide to
    only work on popular games.

  3) There are no concrete references for MIPS R4300i instruction set.
    Although I have found many references relating directly to the N64
    technical documentation, I still cannot find a source labeling the
    instructions via opcode. Although there are technical chip layouts
    available I cannot figure out how that relates to what commands are
    created. 

    I also tried to disassemble the raw bytes in SM64's bootloader
    section to try to see what it would look like using a MIPS R4300
    disassembler, but many of the commands didn't make sense in terms
    of what bytes relate to what parameter. Stuff like 152000EB turned
    into something like | bnez a5, 0x000003D0 | and I have no idea why.
    From my current understanding $15 is the opcode (bnez), $20 is a
    register (a5), but I don't know how $03D0 comes from $EB.

  Basic Information:
    Listing some links I gathered information from in case I need to 
    come back to reference them.

    File information regarding endianness of z64, v64, and n64 files.
    http://jul.rustedlogic.net/thread.php?id=11769
*/

#include <memory>

#include "corrupt.h"

#include "n64_header.h"

class N64Corruption : public Corruption
{
public:
  static const uint32_t Start = 0x1000; // Location past the boot code

  N64Corruption();
  N64Corruption(std::string filename, std::vector<std::string>& args);
  ~N64Corruption();

  virtual void initialize(std::string filename, std::vector<std::string>& args);
  virtual bool valid();
  virtual bool valid_byte(uint8_t byte, uint32_t location);
  virtual void print_header();
  virtual void save(std::string filename);

  static void convert_z64(std::string filename);

private:
  std::unique_ptr<N64Header> header;

};

#endif
