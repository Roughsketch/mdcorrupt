#include "gbc.h"

GBCCorruption::GBCCorruption() { }

GBCCorruption::GBCCorruption(std::string filename, std::vector<std::string>& args)
{
  this->initialize(filename, args);
}

GBCCorruption::~GBCCorruption() { }

void GBCCorruption::initialize(std::string filename, std::vector<std::string>& args)
{
  // Read the rom file
  rom = util::read_file(filename);

  header = std::make_unique<GBCHeader>(rom);
  info = std::make_unique<CorruptionInfo>(args);

  // If the rom is not valid then throw an exception
  if (!valid())
  {
    throw InvalidGBCRomException();
  }

  // Rom is valid, set up variables.
}

void GBCCorruption::print_header()
{
  std::cout << std::hex;
  std::cout << "Entry point:  " << static_cast<int>(this->header->entry()) << std::endl;
  std::cout << "Valid:        " << static_cast<bool>(this->header->valid_logo()) << std::endl;
  std::cout << "Name:         " << this->header->name() << std::endl;
  std::cout << "Manufacturer: " << this->header->manufacturer() << std::endl;
  std::cout << "CGB Enabled:  " << static_cast<int>(this->header->cgb()) << std::endl;
  std::cout << "Company:      " << this->header->company() << std::endl;
  std::cout << "SGB Enabled:  " << static_cast<int>(this->header->sgb()) << std::endl;
  std::cout << "Cart Type:    " << static_cast<int>(this->header->type()) << std::endl;
  std::cout << "Rom Size:     " << static_cast<int>(this->header->rom_size()) << std::endl;
  std::cout << "Ram Size:     " << static_cast<int>(this->header->ram_size()) << std::endl;
  std::cout << "Japanese:     " << static_cast<int>(this->header->destination()) << std::endl;
  std::cout << "Old License:  " << static_cast<int>(this->header->old_license()) << std::endl;
  std::cout << std::dec;
}

bool GBCCorruption::valid()
{
  return this->header->valid_logo();
}

bool GBCCorruption::valid_byte(uint8_t byte, uint32_t location)
{
  //  If location is in the header then do nothing
  if (location < GBCHeader::Start)
  {
    return false;
  }

  //  Map of branch opcodes and their size
  //  std::map<opcode, size>
  static std::map<uint8_t, uint8_t> invalid_branch{
    { 0xC2, 3 }, { 0xD2, 3 }, { 0xC3, 3 }, { 0xC4, 3 },
    { 0xD4, 3 }, { 0xCA, 3 }, { 0xDA, 3 }, { 0XCC, 3 },
    { 0xCD, 3 }, { 0xDC, 3 }, { 0x20, 2 }, { 0x30, 2 },
    { 0x18, 2 }, { 0x28, 2 }, { 0x38, 2 }, { 0xE9, 1 }
  };

  //  List of stack operations (push/pop)
  static std::vector<uint8_t> invalid_stack{
    0xC1, 0xD1, 0xE1, 0xF1, 0xC5, 0xD5, 0xE5, 0xF5
  };

  //  List of 1 byte return opcodes
  static std::vector<uint8_t> invalid_ret{
    0xC0, 0xD0, 0xC7, 0xD7, 0xE7, 0xF7, 0xC8,
    0xD8, 0xC9, 0xD9, 0xCF, 0xDF, 0xEF, 0xFF
  };

  //  If you are within the header size then don't corrupt
  if (location < GBAHeader::Size)
  {
    return false;
  }

  if (std::find(invalid_ret.begin(), invalid_ret.end(), this->rom[location]) != invalid_ret.end() ||
      std::find(invalid_ret.begin(), invalid_ret.end(), byte) != invalid_ret.end())
  {
    //  If replaced byte or current byte is a 1 byte ret opcode then it isn't safe to replace
    return false;
  }

  //  Check if we are a branch opcode or the parameter of a branch opcode
  for (auto& branch : invalid_branch)
  {
    auto opcode = branch.first;
    auto size = branch.second;

    //  Check up to [size] bytes behind us to make sure this location or byte value isn't a branch parameter
    for (int i = 0; i < size; i++)
    {
      //  If it is then it is not valid
      if (this->rom[location - i] == opcode || byte == opcode)
      {
        return false;
      }
    }
  }

  return true;
}

void GBCCorruption::save(std::string filename)
{
  std::string format = ".gbc";

  //  If the filename doesn't include the format extension then add it.
  if (boost::filesystem::extension(filename) != format)
  {
    filename += format;
  }

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