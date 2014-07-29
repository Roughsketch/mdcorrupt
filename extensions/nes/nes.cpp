#include "nes.h"

NESCorruption::NESCorruption()
{

}

NESCorruption::NESCorruption(std::string filename, std::vector<std::string>& args)
{
  this->initialize(filename, args);
}

NESCorruption::~NESCorruption()
{

}

void NESCorruption::initialize(std::string filename, std::vector<std::string>& args)
{
  // Read the rom file
  this->read(filename);
  this->info = std::make_unique<NESCorruptionInfo>(args);

  // If the rom is not valid then throw an exception
  if (!this->valid())
  {
    throw InvalidNESRomException();
  }

  // Rom is valid, set up variables.

  // Amount of PRG-ROM banks
  this->prg_rom = this->rom[4];

  // Amount of CHR-ROM banks
  this->chr_rom = this->rom[5];

  // Determine whether this rom utilizes CHR-ROM
  this->chr = this->rom[5] != 0;

  // Header is always 16 bytes and PRG-ROM is directly after the header.
  this->prg_start = 0x10;

  // CHR-ROM is directly after PRG-ROM and each PRG-ROM bank is 0x4000 bytes long.
  this->chr_start = this->prg_rom * 0x4000 + this->prg_start;
}

void NESCorruption::corrupt()
{
  std::cout << "Rom size: " << std::hex << rom.size() << std::dec << std::endl;

  if (info->prg())
  {
    this->corrupt_prg();
  }
  
  if (info->chr())
  {
    this->corrupt_chr();
  }
}

void NESCorruption::corrupt_prg()
{
  debug::cout << "PRG Type: " << info->prg_type() << std::endl;

  uint32_t corruptions = 0;
  std::uniform_int_distribution<int> dist(0x00, 0xFF);

  for (uint32_t i = this->prg_start + info->prg_start(); i < this->chr_start - info->prg_step() && i < info->prg_end(); i += info->prg_step())
  {
    if (info->prg_type() == CorruptionType::Shift)
    {
      if (valid_byte(this->rom[i + info->prg_value()], i))
      {
        corruptions++;
        this->rom[i] = this->rom[i + info->prg_value()];
      }
    }
    else if (info->prg_type() == CorruptionType::Swap)
    {
      if (valid_byte(this->rom[i + info->prg_value()], i) &&
        valid_byte(this->rom[i], i + info->prg_value()))
      {
        corruptions++;
        uint8_t temp = this->rom[i + info->prg_value()];
        this->rom[i + info->prg_value()] = this->rom[i];
        this->rom[i] = temp;
      }
    }
    else if (info->prg_type() == CorruptionType::Add)
    {
      if (valid_byte(this->rom[i + info->prg_value()] + info->prg_value(), i))
      {
        corruptions++;
        this->rom[i] += info->prg_value();
      }
    }
    else if (info->prg_type() == CorruptionType::Set)
    {
      if (valid_byte(info->prg_value(), i))
      {
        corruptions++;
        this->rom[i] = info->prg_value();
      }
    }
    else if (info->prg_type() == CorruptionType::Random)
    {
      bool corrupted = false;

      //  Try up to 100 times to corrupt
      for (uint32_t retry = 0; !corrupted && retry < 100; retry++)
      {
        uint8_t rand = dist(this->random);

        if (valid_byte(rand, i))
        {
          corruptions++;
          this->rom[i] = rand;
          corrupted = true;
        }
      }
    }
    else if (info->prg_type() == CorruptionType::RotateLeft)
    {
      uint8_t rotate = Util::rol<uint8_t>(this->rom[i], info->prg_value());

      if (valid_byte(rotate, i))
      {
        this->rom[i] = rotate;
        corruptions++;
      }
    }
    else if (info->prg_type() == CorruptionType::RotateRight)
    {
      uint8_t rotate = Util::ror<uint8_t>(this->rom[i], info->prg_value());

      if (valid_byte(rotate, i))
      {
        this->rom[i] = rotate;
        corruptions++;
      }
    }
    else if (info->prg_type() == CorruptionType::LogicalAnd)
    {
      if (valid_byte(this->rom[i] & info->prg_value(), i))
      {
        this->rom[i] &= info->prg_value();
        corruptions++;
      }
    }
    else if (info->prg_type() == CorruptionType::LogicalOr)
    {
      if (valid_byte(this->rom[i] | info->prg_value(), i))
      {
        this->rom[i] |= info->prg_value();
        corruptions++;
      }
    }
    else if (info->prg_type() == CorruptionType::LogicalXor)
    {
      if (valid_byte(this->rom[i] ^ info->prg_value(), i))
      {
        this->rom[i] ^= info->prg_value();
        corruptions++;
      }
    }
    else if (info->prg_type() == CorruptionType::LogicalComplement)
    {
      if (valid_byte(~this->rom[i], i))
      {
        this->rom[i] = ~this->rom[i];
        corruptions++;
      }
    }
    else
    {
      break;  //  No corruption selected, might as well quit.
    }
  }
  std::cout << "Replaced a total of " << corruptions << " bytes in PRG-ROM." << std::endl;
}

void NESCorruption::corrupt_chr()
{
  //  No CHR-ROM section for this rom, return without changing anything.
  if (this->chr == false)
  {
    return;
  }

  debug::cout << "CHR Type: " << info->chr_type() << std::endl;

  uint32_t corruptions = 0;
  std::uniform_int_distribution<int> dist(0x00, 0xFF);

  uint32_t chr_end = info->chr_end();

  if (chr_end == std::numeric_limits<uint32_t>::max())
  {
    chr_end -= this->chr_start; // Prevent integer overflowing in the for loop
  }

  //  Corruptions in CHR-ROM will be in blocks of 8 bytes instead of single values.
  //  There are no checks for valid bytes in CHR-ROM.
  for (uint32_t i = this->chr_start + info->chr_start();
       i < rom.size() - info->chr_step() && i < this->chr_start + chr_end;
       i += info->chr_step())
  {
    if (info->chr_type() == CorruptionType::Shift)
    {
      corruptions++;
      std::copy(this->rom.begin() + i + info->chr_value(), this->rom.begin() + i + info->chr_value() + 8, this->rom.begin() + i);
    }
    else if (info->chr_type() == CorruptionType::Swap)
    {
      corruptions++;
      for (uint32_t index = i; index < i + 8; index++)
      {
        uint8_t temp = this->rom[index + info->chr_value()];
        this->rom[index + info->chr_value()] = this->rom[index];
        this->rom[index] = temp;
      }
    }
    else if (info->chr_type() == CorruptionType::Add)
    {
      corruptions++;
      for (uint32_t index = i; index < rom.size() && index < i + 8; index++)
      {
        this->rom[index] += info->chr_value();
      }
    }
    else if (info->chr_type() == CorruptionType::Set)
    {
      corruptions++;
      for (uint32_t index = i; index < rom.size() && index < i + 8; index++)
      {
        this->rom[index] = info->chr_value();
      }
    }
    else if (info->chr_type() == CorruptionType::Random)
    {
      corruptions++;
      for (uint32_t index = i; index < rom.size() && index < i + 8; index++)
      {
        this->rom[index] = dist(this->random);
      }
    }
    else if (info->chr_type() == CorruptionType::RotateLeft)
    {
      corruptions++;
      for (uint32_t index = i; index < rom.size() && index < i + 8; index++)
      {
        this->rom[index] = Util::rol(this->rom[index], info->chr_value());
      }
    }
    else if (info->chr_type() == CorruptionType::RotateRight)
    {
      corruptions++;
      for (uint32_t index = i; index < rom.size() && index < i + 8; index++)
      {
        this->rom[index] = Util::ror(this->rom[index], info->chr_value());
      }
    }
    else if (info->chr_type() == CorruptionType::LogicalAnd)
    {
      corruptions++;
      for (uint32_t index = i; index < rom.size() && index < i + 8; index++)
      {
        this->rom[index] &= info->chr_value();
      }
    }
    else if (info->chr_type() == CorruptionType::LogicalOr)
    {
      corruptions++;
      for (uint32_t index = i; index < rom.size() && index < i + 8; index++)
      {
        this->rom[index] |= info->chr_value();
      }
    }
    else if (info->chr_type() == CorruptionType::LogicalXor)
    {
      corruptions++;
      for (uint32_t index = i; index < rom.size() && index < i + 8; index++)
      {
        this->rom[index] ^= info->chr_value();
      }
    }
    else if (info->chr_type() == CorruptionType::LogicalComplement)
    {
      corruptions++;
      for (uint32_t index = i; index < rom.size() && index < i + 8; index++)
      {
        this->rom[index] = ~this->rom[index];
      }
    }
    else
    {
      break;
    }

    //std::swap_ranges(this->rom.begin() + i, this->rom.begin() + i + 8, this->rom.begin() + i + 8);
  }

  std::cout << "Replaced a total of " << corruptions << " bytes in CHR-ROM." << std::endl;
}

/*
  Determines whether or not a given byte is valid to include into PRG-ROM

  @param byte - The byte that needs to be checked
  @param location - The offset that the byte will be placed at in the rom

  @return true if the byte can be added at @location in PRG-ROM
*/
bool NESCorruption::valid_byte(uint8_t byte, uint32_t location)
{
  //  If the location refers to CHR-ROM then it doesn't matter what the byte is
  if (location >= this->chr_start)
  {
    return true;
  }

  //  Vector of invalid byte values to include in PRG-ROM
  static std::vector<uint8_t> invalid({ 
    0x00, 0x02, 0x08, 0x12, 0x22, 0x28, 0x32, 0x42, 0x48,
    0x52, 0x62, 0x68, 0x72, 0x78, 0x92, 0xB2, 0xD2, 0xF2 
  });

  //  Byte immediately falls within the invalid byte range
  if (std::find(invalid.begin(), invalid.end(), byte) != invalid.end())
  {
    return false;
  }

  //  Vector of invalid bytes to have before the given insertion
  static std::vector<uint8_t> protect({
    0x48, 0x08, 0x68, 0x28, 0x78, 0x40, 0x60, 0x00, // Invalid for rom[location    ]
    0x90, 0xB0, 0xF0, 0x30, 0xD0, 0x10, 0x50, 0x70, // Invalid for rom[location - 1]
    0x4C, 0x6C, 0x20                                // Invalid for rom[location - 2]
  });

  //  If any invalid prefix bytes are found then it is not valid
  if (std::find(protect.begin()     , protect.end(), this->rom[location    ]) != protect.end() ||
      std::find(protect.begin() +  8, protect.end(), this->rom[location - 1]) != protect.end() ||
      std::find(protect.begin() + 16, protect.end(), this->rom[location - 2]) != protect.end())
  {
    return false;
  }

  //  No false condition found
  return true;
}

/*
  Prints out basic header information for debugging purposes.
*/
void NESCorruption::print_header()
{
  std::cout << "Raw header values:" << std::endl;

  for(int i = 0; i < 10; i++)
  {
    std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(this->rom[i]) << " ";
  }

  std::cout << std::endl;

  std::cout << "There are " << static_cast<int>(this->rom[4]) << " pages of PRG-ROM." << std::endl;

  if (this->rom[5] == 0)
  {
    std::cout << "This game does not directly utilize CHR-ROM." << std::endl;
  }
  else
  {
    std::cout << "There are " << static_cast<int>(this->rom[5]) << " pages of CHR-ROM." << std::endl;
    std::cout << "CHR-ROM starts at $" << std::hex << (0x4000 * this->rom[4] + 0x10) << std::dec << std::endl;
  }
  
  std::cout << std::endl;
}

/*
  Determines whether or not the currently loaded rom
  if a valid NES rom by checking the header.

  @return true for a valid header or false otherwise.
*/
bool NESCorruption::valid()
{
  bool valid = true;
  const char header[5] = {'N', 'E', 'S', 0x1A, 0x00};

  if (this->rom.size() <= 0x10)
  {
    valid = false;
  }
  else
  {
    for (int i = 0; i < 4; i++)
    {
      if (valid && header[i] != this->rom[i])
      {
        valid = false;
      }
    }
  }

  return valid;
}

void NESCorruption::save(std::string filename)
{
  std::string format = ".nes";

  //  If the filename doesn't include the format extension then add it.
  if (filename.length() <= format.length() || filename.compare(filename.length() - format.length(), format.length(), format) != 0)
  {
    filename += format;
  }

  try
  {
    if (info->save_file() != "")
    {
      debug::cout << "Saving to " << info->save_file() << std::endl;
      this->rom_file.open(info->save_file(), std::ios::out | std::ios::binary);
      this->save_name = info->save_file();
    }
    else
    {
      this->rom_file.open(filename, std::ios::out | std::ios::binary);
      this->save_name = filename;
    }

    std::copy(rom.begin(), rom.end(), std::ostream_iterator<uint8_t>(this->rom_file));
    this->rom_file.close();
  }
  catch (InvalidFileNameException e)
  {
    std::cout << e.what() << std::endl;
    exit(EXIT_FAILURE);
  }
}