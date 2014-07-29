#include "corrupt.h"

Corruption::Corruption()
{
  this->random.seed(this->rd());
}

Corruption::Corruption(std::string filename, std::vector<std::string>& args)
{
  this->initialize(filename, args);
}

Corruption::~Corruption()
{
}

void Corruption::initialize(std::string filename, std::vector<std::string>& args)
{
  rom = util::read_file(filename);
  this->info = std::make_unique<CorruptionInfo>(args);

  if (info->save_file() == "")
  {
    this->info->set_save_file(boost::filesystem::basename(filename));
  }
}

/*
  Writes the current rom into a given file.

  @param filename - The file to write to.
*/
void Corruption::save(std::string filename)
{
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

void Corruption::corrupt()
{
  uint64_t corruptions = 0;

  std::uniform_int_distribution<int> dist(0x00, 0xFF);

  for (uint32_t i = info->start();            // Start at header + start value
    i < this->rom.size() && i < info->end();  // While in a valid range
    i += info->step())                        // Increase by step size
  {
    if (info->type() == CorruptionType::Shift)
    {
      //  If it's okay to put the other byte in this position then change it
      if (i + info->value() < rom.size() && valid_byte(this->rom[i + info->value()], i))
      {
        corruptions++;
        this->rom[i] = this->rom[i + info->value()];
      }
    }
    else if (info->type() == CorruptionType::Swap)
    {

      if (i + info->value() < rom.size() &&
        valid_byte(this->rom[i + info->value()], i) &&  //  If it's okay to put the other byte in this position
        valid_byte(this->rom[i], i))    //  And it's okay to put this byte in the other position
      {
        corruptions++;
        uint8_t temp = this->rom[i + info->value()];
        this->rom[i] = temp;
        this->rom[i + info->value()] = temp;
      }
    }
    else if (info->type() == CorruptionType::Add)
    {
      //  If the new byte value is valid then do the corruption
      if (valid_byte(this->rom[i] + info->value(), i))
      {
        corruptions++;
        this->rom[i] += info->value();
      }
    }
    else if (info->type() == CorruptionType::Set)
    {
      //  If the set value can be placed here then do it
      if (valid_byte(info->value(), i))
      {
        corruptions++;
        this->rom[i] = info->value();
      }
    }
    else if (info->type() == CorruptionType::Random)
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
    else if (info->type() == CorruptionType::RotateLeft)
    {
      uint8_t rotate = util::rol<uint8_t>(this->rom[i], info->value());

      if (valid_byte(rotate, i))
      {
        this->rom[i] = rotate;
        corruptions++;
      }
    }
    else if (info->type() == CorruptionType::RotateRight)
    {
      uint8_t rotate = util::ror<uint8_t>(this->rom[i], info->value());

      if (valid_byte(rotate, i))
      {
        this->rom[i] = rotate;
        corruptions++;
      }
    }
    else if (info->type() == CorruptionType::LogicalAnd)
    {
      if (valid_byte(this->rom[i] & info->value(), i))
      {
        this->rom[i] &= info->value();
        corruptions++;
      }
    }
    else if (info->type() == CorruptionType::LogicalOr)
    {
      if (valid_byte(this->rom[i] | info->value(), i))
      {
        this->rom[i] |= info->value();
        corruptions++;
      }
    }
    else if (info->type() == CorruptionType::LogicalXor)
    {
      if (valid_byte(this->rom[i] ^ info->value(), i))
      {
        this->rom[i] ^= info->value();
        corruptions++;
      }
    }
    else if (info->type() == CorruptionType::LogicalComplement)
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

  std::cout << "Replaced a total of " << corruptions << " bytes." << std::endl;
}

bool Corruption::valid()
{
  return true;
}

bool Corruption::valid_byte(uint8_t byte, uint32_t location)
{
  return true;
}

void Corruption::print_header()
{
  //debug::cout << "No header." << std::endl;
}

int Corruption::size()
{
  return rom.size();
}

void Corruption::run()
{
  //  TODO: system() is a horrible function and it needs a cross-platform replacement.

  //  Run whatever file it was saved at with its default program.
  system(std::string("\"" + this->save_name + "\"").c_str());
}
