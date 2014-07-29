#include "genesis.h"

GenesisCorruption::GenesisCorruption()
{

}

GenesisCorruption::GenesisCorruption(std::string filename, std::vector<std::string>& args)
{
  this->initialize(filename, args);
}

GenesisCorruption::~GenesisCorruption()
{

}

void GenesisCorruption::initialize(std::string filename, std::vector<std::string>& args)
{
  // Read the rom file
  this->read(filename);
  this->info = std::make_unique<CorruptionInfo>(args);
  this->header = std::make_unique<GenesisHeader>(ref(this->rom));

  // If the rom is not valid then throw an exception
  if (!this->valid())
  {
    throw InvalidGenesisRomException();
  }

  // Rom is valid, set up variables.
}

/*
Determines whether or not a given byte is valid to include into the rom.

@param byte - The byte that needs to be checked
@param location - The offset that the byte will be placed at in the rom

@return true if the byte can be added at @location in the rom.
*/
bool GenesisCorruption::valid_byte(uint8_t byte, uint32_t location)
{
  if (is_branch(util::read_big<uint16_t>(rom, location)) ||
    is_branch(util::read_big<uint16_t>(rom, location - 1)))
  {
    return true;
  }

  return true;
}

bool GenesisCorruption::is_branch(uint16_t instruction)
{
  uint8_t hi_byte = (instruction & 0xFF00) >> 8;
  uint8_t hi_nybble = (instruction & 0xF000) >> 12;

  return (hi_nybble == 0x06 ||  //  Bcc
          hi_byte == 0x61 || // BSR
          hi_byte == 0x60 || // BRA
          hi_byte == 0x4E75 ||  //  RTS
          hi_byte == 0x4E77 ||  //  RTR
          hi_byte == 0x4E74 ||  //  RTD
          (instruction & 0xFFC0) == 0x4E80 || //  JSR
          (instruction & 0xFFC0) == 0x4EC0 || //  JMP
          (instruction & 0xF0F8) == 0x50C8);  //DBcc
}

/*
Prints out basic header information for debugging purposes.
*/
void GenesisCorruption::print_header()
{
}

/*
Determines whether or not the currently loaded rom
is a valid Genesis rom by checking if the header exists.

@return true for a valid header or false otherwise.
*/
bool GenesisCorruption::valid()
{
  return this->header->valid();
}

/*
Saves the current ROM to a file, automatically taking into account
the extension.

@param filename - The file to save to. Does not require extension.
*/
void GenesisCorruption::save(std::string filename)
{
  std::string format = ".md";

  //  If the filename doesn't include the format extension then add it.
  if (filename.length() <= 4 || filename.compare(filename.length() - format.length(), format.length(), format) != 0)
  {
    filename += format;
  }

  //  Fix the checksum
  uint16_t cs = checksum();
  rom[0x18E] = (cs & 0xFF00) >> 8;
  rom[0x18F] = (cs & 0xFF);

  //  Patch the vector table
  for (int i = 8; i < 0x100; i += 4)
  {
    //  Ignore 0x00, 0x04, 0x70 and 0x78

    if (i == 0x40)
    {
      //  Actual patch goes in 0x40-0x48
      rom[i] = 0x54;
      rom[i + 1] = 0x8F;
      rom[i + 2] = 0x54;
      rom[i + 3] = 0x97;
      rom[i + 4] = 0x55;
      rom[i + 5] = 0x8F;
      rom[i + 6] = 0x4E;
      rom[i + 7] = 0x73;
      i += 4;
    }
    else if (i != 0x70 && i != 0x78)
    {
      //  Any other section besides 0x70 and 0x78 points to the patch at 0x40
      rom[i] = 0;
      rom[i + 1] = 0;
      rom[i + 2] = 0;
      rom[i + 3] = 0x40;
    }
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

    std::copy(rom.begin(), rom.end(), std::ostream_iterator<uint8_t>(this->rom_file));
    this->rom_file.close();

    this->save_name = filename;
  }
  catch (InvalidFileNameException e)
  {
    std::cout << e.what() << std::endl;
    exit(EXIT_FAILURE);
  }
}

uint16_t GenesisCorruption::checksum()
{
  uint16_t sum = 0;

  for (uint32_t i = 0x200; i < rom.size();)
  {
    sum += rom[i++] * 0x100;
    sum += rom[i++];
  }

  //debug::cout << "Checksum: " << std::hex << sum << std::dec << std::endl;

  return sum;
}