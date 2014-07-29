#include "snes_header.h"

SNESHeader::SNESHeader()
{
  //  Do nothing
}

SNESHeader::SNESHeader(std::vector<uint8_t>& rom)
{
  this->initialize(rom);
}

SNESHeader::~SNESHeader()
{
  //  Do nothing
}

void SNESHeader::initialize(std::vector<uint8_t>& rom)
{
  // If the rom size divided by 0x2000 has a remainder of 0x200 (512)
  // Then the rom has a 512 byte smc header.
  this->has_smc = rom.size() % 0x2000 == 0x200;

  static std::vector<uint32_t> offsets{ 0x007fc0, 0x00ffc0, 0x40ffc0 };
  std::vector <uint32_t> scores;

  for (auto& addr : offsets)
  {
    scores.push_back(SNESHeader::score_header(rom, addr + (this->has_smc ? 0x200 : 0)));
  }

  auto max = std::max_element(scores.begin(), scores.end());

  if (*max == 0)
  {
    this->header_offset = 0;
  }
  else
  {
    //  Determine the index of the max score and grab the address related to the score.
    this->header_offset = offsets[std::distance(scores.begin(), max)];

    //  If the rom has an SMC header then the offset increases by 0x200
    if (this->has_smc)
    {
      this->header_offset += 0x200;
    }

    std::cout << "Offset of the SNES header is " << std::hex << header_offset << "." << std::dec << std::endl;

    //  Reserve enough space to copy the header
    this->header.resize(SNESHeader::Header_Size);

    //  Copy the header information into the class
    std::copy(rom.begin() + header_offset, rom.begin() + header_offset + SNESHeader::Header_Size, this->header.begin());
  }
}

bool SNESHeader::smc_header()
{
  return this->has_smc;
}

std::string SNESHeader::name()
{
  std::string name = std::string(header.begin(), header.begin() + 21);
  return name;
}

uint8_t SNESHeader::layout()
{
  return header[SNESOffset::Layout];
}

uint8_t SNESHeader::type()
{
  return header[SNESOffset::Type];
}

uint32_t SNESHeader::rom_size()
{
  return 0x400 << header[SNESOffset::ROM_Size];
}

uint32_t SNESHeader::ram_size()
{
  return header[SNESOffset::RAM_Size] == 0 ? 0 : 0x400 << header[SNESOffset::RAM_Size];
}

uint8_t SNESHeader::country()
{
  return header[SNESOffset::Country];
}

uint8_t SNESHeader::license_code()
{
  return header[SNESOffset::License];
}

uint8_t SNESHeader::version()
{
  return header[SNESOffset::Version];
}

uint32_t SNESHeader::offset()
{
  return this->header_offset;
}

/*
  Determines whether a given address is likely to be the SNES header location.

  Scoring system was adapted from the BSNES emulator ROM loader which can be found below.
  Scoring can be found in the file bsnes/nall/emulation/super-famicom.hpp
  http://bsnes.googlecode.com/files/bsnes_v091-source.tar.xz

  @param rom - The full rom data
  @param address - The address to score

  @return A score of how likely the given address is to be the SNES header. Higher values mean it's more likely.
*/
uint32_t SNESHeader::score_header(std::vector<uint8_t> &rom, uint32_t address)
{
  //  If the given address + header size exceeds the rom size then it can't possibly be a header.
  if (address + SNESHeader::Header_Size > rom.size())
  {
    return 0;
  }

  //  The map relates scores of certain opcodes found in the header.
  //  std::map<int Score, vector Opcodes>
  static std::map<int32_t, std::vector<uint8_t>> op_scores{
    {  8, std::vector<uint8_t>{ 0x18, 0x38, 0x4c, 0x5c, 0x78, 0x9c } }, // Most likely
    {  4, std::vector<uint8_t>{ 0x20, 0x22, 0xa0, 0xa2, 0xa9, 0xac, 0xad, 0xae, 0xaf, 0xc2, 0xe2 } }, // Likely
    { -4, std::vector<uint8_t>{ 0x40, 0x60, 0x6b, 0xcc, 0xcd, 0xec } }, // Less likely
    { -8, std::vector<uint8_t>{ 0x00, 0x02, 0x42, 0xdb, 0xff } } // Least likely
  };

  int32_t score = 0;

  //  Get reset location value
  uint16_t reset = SNESHeader::create_uint16(rom, address + SNESOffset::Emulated_RESET);

  //  Get the checksum and its complement.
  uint16_t checksum = SNESHeader::create_uint16(rom, address + SNESOffset::Checksum);
  uint16_t complement = SNESHeader::create_uint16(rom, address + SNESOffset::Complement);

  //  Get first opcode executed on reset
  uint16_t reset_op = rom[(address & ~0x7fff) | (reset & 0x7fff)];

  //  Mask off the FastROM bit if it's there.
  uint16_t layout = SNESHeader::create_uint16(rom, address + SNESOffset::Layout) & ~0x10;

  //  Anything under $8000 is invalid, score is 0
  if (reset < 0x8000)
  {
    return 0;
  }

  for (auto& pair : op_scores)
  {
    auto opcodes = pair.second;

    //  Add the score value of the reset opcode if it's found in the vector
    if (std::find(opcodes.begin(), opcodes.end(), reset_op) != opcodes.end())
    {
      score += pair.first;
    }
  }

  //  If the header has a valid checksum then add to the score
  if (checksum + complement == 0xffff && checksum != 0 && complement != 0)
  {
    score += 4;
  }

  switch (layout)
  {
  case 0x20: // LoROM
  case 0x22: // ExLoROM
    if (address == 0x007fc0)
    {
      score += 2;
    }
  case 0x21: // HiROM
    if (address == 0x00ffc0)
    {
      score += 2;
    }
  case 0x25: // ExHiROM
    if (address == 0x40ffc0)
    {
      score += 2;
    }
  }

  //  Check for extended header
  score += (rom[address + SNESOffset::License] == 0x33) ? 2 : 0;

  //  If values fall into accepted ranges then add to score
  score += (rom[address + SNESOffset::Type    ] < 0x08) ? 1 : 0;
  score += (rom[address + SNESOffset::RAM_Size] < 0x08) ? 1 : 0;
  score += (rom[address + SNESOffset::ROM_Size] < 0x0e) ? 1 : 0;
  score += (rom[address + SNESOffset::Country ] < 0x10) ? 1 : 0;

  //  Return the score or 0 if the score is less than 0
  return std::max(score, 0);
}

uint32_t SNESHeader::begin()
{
  //  If the rom has an SMC header then add 0x200
  uint32_t offset = this->has_smc ? 0x200u : 0u;

  uint8_t type = this->type();

  //  If the rom uses the SuperFX chip then skip the first rom bank
  //  to help avoid consistent crashing.
  if (type == 0x13 || type == 0x14 ||
      type == 0x15 || type == 0x1a)
  {
    offset += 0x10000;
  }

  return offset;
}

inline uint16_t SNESHeader::create_uint16(std::vector<uint8_t> &rom, uint32_t address)
{
  uint16_t value = rom[address] | (rom[address + 1] << 8);
  return value;
}
