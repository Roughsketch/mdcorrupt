#include "gba.h"

GBACorruption::GBACorruption()
{

}

GBACorruption::GBACorruption(std::string filename, std::vector<std::string>& args)
{
  initialize(filename, args);
}

GBACorruption::~GBACorruption()
{

}

void GBACorruption::initialize(std::string filename, std::vector<std::string>& args)
{
  this->read(filename);
  this->header = std::make_unique<GBAHeader>(ref(this->rom));
  this->info = std::make_unique<CorruptionInfo>(args);

  if (!valid())
  {
    throw InvalidRomException();
  }
}

bool GBACorruption::valid()
{
  return header->valid();
}

bool GBACorruption::valid_byte(uint8_t byte, uint32_t location)
{
  //  Entire instruction is at location of the nearest 4 byte boundary
  uint32_t instruction = Util::read<uint32_t>(ref(rom), location - (location % 4));

  /*
  Format for ARM7 instructions:

  +-----------------------------------------------------+
  |31  28|27      20|19               8|7        4|3   0|
  |-----------------------------------------------------|
  | COND | OPCODE 1 | PARAMETERS/OTHER | OPCODE 2 | RM  |
  +-----------------------------------------------------+
  */

  uint8_t condition = (instruction & 0xF0000000) >> 24;
  uint8_t opcode1 = (instruction & 0x0FF00000) >> 20;
  uint8_t opcode2 = (instruction & 0x000000F0) >> 4;

  //  When opcode1 is between 0x80 and 0xE0 it will be jumps or load/store
  if (opcode1 >= 0x80 && opcode1 < 0xE0)
  {
    return false;
  }

  //  Every even opcode1 between 0x60 and 0x80 is a load/store
  if (opcode1 >= 0x60 && opcode1 < 0x80 && opcode2 % 2 == 0)
  {
    return false;
  }

  //  Opcode1 between 0x40 and 0x60 are all load/store
  if (opcode1 >= 0x40 && opcode1 < 0x60)
  {
    return false;
  }

  //  If opcode1 is between 0x00 and 0x20
  if (opcode1 >= 0x00 && opcode1 < 0x20)
  {
    if ((opcode2 & 0x0B) == 0x0B)
    {
      //  All opcode2 that end with 0x0B are load/store
      return false;
    }
    else if (opcode1 % 2 == 1 && ((opcode2 & 0x0D) == 0x0D || (opcode2 & 0x0F) == 0x0F))
    {
      //  All opcode2 that end with 0x0D or 0x0F and have an odd opcode1 are load/store
      return false;
    }
  }

  return true;
}

void GBACorruption::print_header()
{

}

void GBACorruption::save(std::string filename)
{
  std::string format = ".gba";

  //  If the filename doesn't include the format extension then add it.
  if (filename.length() <= 4 || filename.compare(filename.length() - format.length(), format.length(), format) != 0)
  {
    filename += format;
  }

  try
  {
    if (info->save_file() != "")
    {
      this->rom_file.open(info->save_file(), std::ios::out | std::ios::binary);
    }
    else
    {
      this->rom_file.open(filename, std::ios::out | std::ios::binary);
    }

    rom_file.write(reinterpret_cast<char *>(&rom[0]), rom.size());
    //std::copy(rom.begin(), rom.end(), std::ostream_iterator<uint8_t>(this->rom_file));
    this->rom_file.close();

    this->save_name = filename;
  }
  catch (InvalidFileNameException e)
  {
    std::cout << e.what() << std::endl;
    exit(EXIT_FAILURE);
  }
}
