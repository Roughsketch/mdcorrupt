#include "snes.h"

SNESCorruption::SNESCorruption()
{

}

SNESCorruption::SNESCorruption(std::string filename, std::vector<std::string>& args)
{
  this->initialize(filename, args);
}

SNESCorruption::~SNESCorruption()
{

}

void SNESCorruption::initialize(std::string filename, std::vector<std::string>& args)
{
  // Read the rom file
  rom = util::read_file(filename);
  info = std::make_unique<CorruptionInfo>(args);
  header = std::make_unique<SNESHeader>(rom);

  // If the rom is not valid then throw an exception
  if (!valid())
  {
    throw InvalidSNESRomException();
  }

  // Rom is valid, set up variables.
}

/*
  Determines whether or not a given byte is valid to include into the rom.

  @param byte - The byte that needs to be checked
  @param location - The offset that the byte will be placed at in the rom

  @return true if the byte can be added at @location in the rom.
*/
bool SNESCorruption::valid_byte(uint8_t byte, uint32_t location)
{
  static std::vector<uint8_t> invalid_write{ 
  // BRK,  COP,  STP,  WAI,  REP,  SEP
    0x00, 0x02, 0xdb, 0xcb, 0xc2, 0xe2              // Miscellaneous invalid bytes
  };

  static std::vector<uint8_t> invalid_stack{
    0x08, 0x0B, 0x28, 0x2b, 0x48, 0x4b, 0x5a, 0x62, // Stack opcodes
    0x68, 0x7a, 0x8b, 0xab, 0xd4, 0xda, 0xf4,       // Stack opcodes
  };

  //  Map of jump/branch opcodes and their parameter size excluding the opcode byte
  //  std::map<opcode, param_size>
  static std::map<uint8_t, uint8_t> invalid_branch{
    { 0x80, 2 }, { 0x82, 3 }, { 0x30, 2 }, { 0x50, 2 }, { 0x70, 2 }, { 0x90, 2 }, { 0xb0, 2 },
    { 0xd0, 2 }, { 0xf0, 2 }, { 0x10, 2 }, { 0x4c, 3 }, { 0x6c, 3 }, { 0xdc, 3 }, { 0x7c, 3 },
    { 0x5c, 4 }, { 0x20, 3 }, { 0xfc, 3 }, { 0x22, 4 }, { 0x40, 1 }, { 0x60, 1 }, { 0x6b, 1 } 
  };

  //  STA $xxxx (, *) opcodes
  static std::vector<uint8_t> sta_ops{ 0x8D, 0x99, 0x9D };

  //  If the location points to the SNES header then it is invalid
  if (location >= this->header->offset() && location <= this->header->offset() + SNESHeader::Header_Size)
  {
    return false;
  }

  if (std::find(invalid_write.begin(), invalid_write.end(), byte) != invalid_write.end())
  {
    //  Given byte value is invalid for any space
    return false;
  }

  //  Check special case of REP (0xc2) and SEP (0xe2) opcodes which cannot
  //  have their opcode or parameter modified safely.
  if (this->rom[location - 1] == 0xc2 || this->rom[location - 1] == 0xe2)
  {
    return false;
  }

  if (std::find(invalid_stack.begin(), invalid_stack.end(), this->rom[location]) != invalid_stack.end())
  {
    //  Byte at overwrite location contains an important value.
    return false;
  }

  //  Iterate over each jump/branch instruction pair
  for (auto& pair : invalid_branch)
  {
    //  For readabiity, assign branch pair values to locals
    uint8_t opcode = pair.first;  //  Branch opcode
    uint8_t param = pair.second;  //  Branch parameter size

    for (int i = 0; i < param; i++)
    {
      //  If this part of rom contains a branch opcode return false
      if (this->rom[location - i] == opcode)
      {
        return false;
      }
    }
  }

  //  Check for STA commands
  for (auto& sta : sta_ops)
  {
    //  All 3 are 3 bytes long, so check back 2 bytes max
    for (int i = 0; i < 3; i++)
    {
      //  Check if given location has a STA opcode
      if (this->rom[location - i] == sta)
      {
        //  If opcode is found, determine whether or not it contains an important register value
        if (SNESCorruption::is_register(util::read<uint16_t>(ref(rom), location - i)))
        {
          //  If it contains an important register then it isn't safe to overwrite.
          return false;
        }
      }
    }
  }

  return true;
}

/*
  Prints out basic header information for debugging purposes.
*/
void SNESCorruption::print_header()
{
  std::cout << std::hex;

  std::cout << "Name:     " << this->header->name() << std::endl;
  std::cout << "Layout:   " << static_cast<int>(this->header->layout())       << std::endl;
  std::cout << "Type:     " << static_cast<int>(this->header->type())         << std::endl;
  std::cout << "Rom Size: " << static_cast<int>(this->header->rom_size())     << std::endl;
  std::cout << "Ram Size: " << static_cast<int>(this->header->ram_size())     << std::endl;
  std::cout << "Country:  " << static_cast<int>(this->header->country())      << std::endl;
  std::cout << "License:  " << static_cast<int>(this->header->license_code()) << std::endl;
  std::cout << "Version:  " << static_cast<int>(this->header->version())      << std::endl;

  std::cout << std::dec;
}

/*
  Determines whether or not the currently loaded rom
  is a valid SNES rom by checking if the header exists.

  @return true for a valid header or false otherwise.
*/
bool SNESCorruption::valid()
{
  return this->header->offset() != 0;
}

/*
  Saves the current ROM to a file, automatically taking into account
  the extension.

  @param filename - The file to save to. Does not require extension.
*/
void SNESCorruption::save(std::string filename)
{
  std::string format = ".sfc";

  //  If the filename doesn't include the format extension then add it.
  if (boost::filesystem::extension(filename) != ".sfc")
  {
    filename += format;
  }


  //  Patch the COP and BRK vectors
  uint32_t offset = header->offset();
  rom[offset + SNESOffset::Unused1] = 0x40;   //  Set  unused portion to a single RTI command

  //  Set the COP interrupt to point to the RTI
  rom[offset + SNESOffset::Native_COP] = (offset + SNESOffset::Unused1) & 0xFF;
  rom[offset + SNESOffset::Native_COP + 1] = ((offset + SNESOffset::Unused1) & 0xFF00) >> 8;

  //  Set the BRK interrupt to point to the RTI
  rom[offset + SNESOffset::Native_BRK] = (offset + SNESOffset::Unused1) & 0xFF;
  rom[offset + SNESOffset::Native_BRK + 1] = ((offset + SNESOffset::Unused1) & 0xFF00) >> 8;

  try
  {
    if (info->save_file() != "")
    {
      util::write_file(info->save_file(), rom);
      this->save_name = info->save_file();
    }
    else
    {
      util::write_file(filename, rom);
      this->save_name = filename;
    }
  }
  catch (InvalidFileNameException e)
  {
    std::cout << e.what() << std::endl;
    exit(EXIT_FAILURE);
  }
}
