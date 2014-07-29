#include "nds.h"

NDSCorruption::NDSCorruption()
{

}

NDSCorruption::NDSCorruption(std::string filename, std::vector<std::string>& args)
{
  initialize(filename, args);
}

NDSCorruption::~NDSCorruption()
{

}

void NDSCorruption::initialize(std::string filename, std::vector<std::string>& args)
{
  this->read(filename);
  this->header = std::make_unique<NDSHeader>(ref(this->rom));
  this->info = std::make_unique<CorruptionInfo>(args);
  this->filesystem = std::make_unique<NDSFileSystem>(ref(this->rom), ref(this->header));

  this->m_save = false;

  if (info->list())
  {
    std::cout << "JSON File Listing: " << filesystem->to_json() << std::endl;
  }
}

void NDSCorruption::corrupt()
{
  //  Don't do anything if no important options were set
  if (info->step() <= 0 || info->files().size() == 0)
  {
    return;
  }

  std::cout << "Rom size: " << std::hex << rom.size() << std::dec << std::endl;


  //  For counting amount of corruptions
  uint32_t corruptions = 0;

  //  Random number distribution over the byte value range
  std::uniform_int_distribution<int> random(0x00, 0xFF);

  for (auto& file : info->files())
  {
    std::cout << "Reading file" << std::endl;

    //  Read the file into wad
    NDSEntry entry;
    
    try
    {
      entry = filesystem->get(file);
    }
    catch (...)
    {
      debug::cout << "Could not find file '" << file << "'" << std::endl;
      continue;
    }

    std::cout << "Corrupting " << file << " with size " << entry.size() << std::endl;


    std::cout << "Starting" << std::endl;

    for (uint32_t i = info->start() + entry.offset(); i + info->step() < entry.offset() + entry.size() && i - entry.offset() < info->end(); i += info->step())
    {
      if (info->type() == CorruptionType::Shift)
      {
        //  If it's okay to put the other byte in this position then change it
        if (i + info->value() < rom.size() && valid_byte(rom[i + info->value()], i))
        {
          corruptions++;
          rom[i] = rom[i + info->value()];
        }
      }
      else if (info->type() == CorruptionType::Swap)
      {

        if (i + info->value() < rom.size() &&
          valid_byte(rom[i + info->value()], i) &&  //  If it's okay to put the other byte in this position
          valid_byte(rom[i], i + info->value()))    //  And it's okay to put this byte in the other position
        {
          corruptions++;
          uint8_t temp = rom[i + info->value()];
          rom[i] = temp;
          rom[i + info->value()] = temp;
        }
      }
      else if (info->type() == CorruptionType::Add)
      {
        //  If the new byte value is valid then do the corruption
        if (valid_byte(rom[i] + info->value(), i))
        {
          corruptions++;
          rom[i] += info->value();
        }
      }
      else if (info->type() == CorruptionType::Set)
      {
        //  If the set value can be placed here then do it
        if (valid_byte(info->value(), i))
        {
          corruptions++;
          rom[i] = info->value();
        }
      }
      else if (info->type() == CorruptionType::Random)
      {
        bool corrupted = false;

        //  Try up to 100 times to corrupt
        for (uint32_t retry = 0; !corrupted && retry < 100; retry++)
        {
          uint8_t rand = random(this->random);
          if (valid_byte(rand, i))
          {
            corruptions++;
            rom[i] = rand;
            corrupted = true;
          }
        }
      }
      else if (info->type() == CorruptionType::RotateLeft)
      {
        uint8_t rotate = Util::rol<uint8_t>(rom[i], info->value());

        if (valid_byte(rotate, i))
        {
          rom[i] = rotate;
          corruptions++;
        }
      }
      else if (info->type() == CorruptionType::RotateRight)
      {
        uint8_t rotate = Util::ror<uint8_t>(rom[i], info->value());

        if (valid_byte(rotate, i))
        {
          rom[i] = rotate;
          corruptions++;
        }
      }
      else if (info->type() == CorruptionType::LogicalAnd)
      {
        if (valid_byte(rom[i] & info->value(), i))
        {
          rom[i] &= info->value();
          corruptions++;
        }
      }
      else if (info->type() == CorruptionType::LogicalOr)
      {
        if (valid_byte(rom[i] | info->value(), i))
        {
          rom[i] |= info->value();
          corruptions++;
        }
      }
      else if (info->type() == CorruptionType::LogicalXor)
      {
        if (valid_byte(rom[i] ^ info->value(), i))
        {
          rom[i] ^= info->value();
          corruptions++;
        }
      }
      else if (info->type() == CorruptionType::LogicalComplement)
      {
        if (valid_byte(~rom[i], i))
        {
          rom[i] = ~rom[i];
          corruptions++;
        }
      }
      else
      {
        break;  //  No corruption selected, might as well quit.
      }
    }

    //  Write the modified data back to the file
    //entry.write(rom, data);
  }

  //  Tell the user how many bytes were corrupted
  std::cout << corruptions << " bytes corrupted." << std::endl;

  this->m_save = corruptions > 0;
}

bool NDSCorruption::valid()
{
  return true;
}

bool NDSCorruption::valid_byte(uint8_t byte, uint32_t location)
{
  //  Entire instruction is at location of the nearest 4 byte boundary
  uint32_t instruction = Util::read<uint32_t>(ref(rom), location - (location % 4));

  /*
    Format for ARM9 instructions:

    +-----------------------------------------------------+
    |31  28|27      20|19               8|7        4|3   0|
    |-----------------------------------------------------|
    | COND | OPCODE 1 | PARAMETERS/OTHER | OPCODE 2 | RM  |
    +-----------------------------------------------------+
  */

  uint8_t condition = (instruction & 0xF0000000) >> 24;
  uint8_t opcode1   = (instruction & 0x0FF00000) >> 20;
  uint8_t opcode2   = (instruction & 0x000000F0) >> 4;

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

void NDSCorruption::print_header()
{

}

void NDSCorruption::save(std::string filename)
{
  //  If nothing was corrupted then no need to save
  if (this->m_save == false)
  {
    return;
  }

  std::string format = ".nds";

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
