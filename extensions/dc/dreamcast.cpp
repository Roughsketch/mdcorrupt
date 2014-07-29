#include "dreamcast.h"

DreamcastCorruption::DreamcastCorruption()
{
  this->m_saved = false;
}

DreamcastCorruption::DreamcastCorruption(std::string filename, std::vector<std::string>& args)
{
  this->initialize(filename, args);
}

DreamcastCorruption::~DreamcastCorruption()
{
  if (boost::filesystem::exists(this->m_temp_file))
  {
    if (std::remove(this->m_temp_file.c_str()) != 0)
    {
      throw InvalidFileException("Could not delete temp file: " + this->m_temp_file);
    }
  }
}

/*
Initialize a DreamcastCorruption with a particular file

@param filename - The file to initialize with
*/
void DreamcastCorruption::initialize(std::string filename, std::vector<std::string>& args)
{
  this->m_original_file = filename;
  this->m_temp_file = "tmp_" + std::to_string(std::time(0)) + ".img";

  this->rom = std::make_unique<IMG>(filename, false);
  this->m_saved = false;
}

/*
Corrupt the loaded file with the parameters in the DreamcastCorruptionInfo class
*/
void DreamcastCorruption::corrupt()
{
  if (info->step() > 0 && info->files().size() > 0)
  {
    //  Copy file only if step is valid and there are files to corrupt.
    boost::filesystem::copy_file(this->m_original_file, this->m_temp_file, boost::filesystem::copy_option::overwrite_if_exists);
  }
  else
  {
    return; //  If no step or files to corrupt then return
  }

  //  For counting amount of corruptions
  uint32_t corruptions = 0;

  //  Random number distribution over the byte value range
  std::uniform_int_distribution<int> random(0x00, 0xFF);

  //  Open file for read/write in binary mode
  std::fstream img(this->m_temp_file, std::ios::in | std::ios::out | std::ios::binary);

  //  If image could not be read then throw an exception
  if (!img.good())
  {
    throw InvalidFileException("Could not open temp file: " + this->m_temp_file);
  }

  for (auto& file : info->files())
  {
    //  Read the file into wad
    Entry entry = (*this->rom)[file];
    //  Get the raw data of the entry
    std::vector<uint8_t> data;

    try
    {
      data = entry.get(img);
    }
    catch (...)
    {
      debug::cout << "Could not find file '" << file << "'" << std::endl;
      continue;
    }

    //  If no data, then throw an exception
    if (data.empty())
    {
      debug::cout << "No data found in file '" << file << "'" << std::endl;
      continue;
    }

    for (uint32_t i = info->start(); i < data.size() - info->step() && i < info->end(); i += info->step())
    {
      if (info->type() == CorruptionType::Shift)
      {
        //  If it's okay to put the other byte in this position then change it
        if (i + info->value() < data.size() && valid_byte(data[i + info->value()], i))
        {
          corruptions++;
          data[i] = data[i + info->value()];
        }
      }
      else if (info->type() == CorruptionType::Swap)
      {

        if (i + info->value() < data.size() &&
          valid_byte(data[i + info->value()], i) &&  //  If it's okay to put the other byte in this position
          valid_byte(data[i], i + info->value()))    //  And it's okay to put this byte in the other position
        {
          corruptions++;
          uint8_t temp = data[i + info->value()];
          data[i] = temp;
          data[i + info->value()] = temp;
        }
      }
      else if (info->type() == CorruptionType::Add)
      {
        //  If the new byte value is valid then do the corruption
        if (valid_byte(data[i] + info->value(), i))
        {
          corruptions++;
          data[i] += info->value();
        }
      }
      else if (info->type() == CorruptionType::Set)
      {
        //  If the set value can be placed here then do it
        if (valid_byte(info->value(), i))
        {
          corruptions++;
          data[i] = info->value();
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
            data[i] = rand;
            corrupted = true;
          }
        }
      }
      else if (info->type() == CorruptionType::RotateLeft)
      {
        uint8_t rotate = Util::rol<uint8_t>(data[i], info->value());

        if (valid_byte(rotate, i))
        {
          data[i] = rotate;
          corruptions++;
        }
      }
      else if (info->type() == CorruptionType::RotateRight)
      {
        uint8_t rotate = Util::ror<uint8_t>(data[i], info->value());

        if (valid_byte(rotate, i))
        {
          data[i] = rotate;
          corruptions++;
        }
      }
      else if (info->type() == CorruptionType::LogicalAnd)
      {
        if (valid_byte(data[i] & info->value(), i))
        {
          data[i] &= info->value();
          corruptions++;
        }
      }
      else if (info->type() == CorruptionType::LogicalOr)
      {
        if (valid_byte(data[i] | info->value(), i))
        {
          data[i] |= info->value();
          corruptions++;
        }
      }
      else if (info->type() == CorruptionType::LogicalXor)
      {
        if (valid_byte(data[i] ^ info->value(), i))
        {
          data[i] ^= info->value();
          corruptions++;
        }
      }
      else if (info->type() == CorruptionType::LogicalComplement)
      {
        if (valid_byte(~data[i], i))
        {
          data[i] = ~data[i];
          corruptions++;
        }
      }
      else
      {
        break;  //  No corruption selected, might as well quit.
      }
    }

    //  Write the modified data back to the file
    entry.write(img, data);
  }

  //  Close the file
  img.close();

  //  Tell the user how many bytes were corrupted
  std::cout << corruptions << " bytes corrupted." << std::endl;

  /*
  //  Read the file into wad
  Entry wad = (*this->rom)["wad.wad"];
  //  Get the raw data of the entry
  std::vector<uint8_t> data = wad.get(img);

  //  If no data, then throw an exception
  if (data.empty())
  {
  throw FileNotFoundException("File was not found in the IMG.");
  }

  for (uint32_t i = 0; i < data.size() - 20; i += info->step())
  {
  uint8_t old = data[i];
  data[i] = dist(this->random);

  if (old != data[i])
  {
  corruptions++;
  }
  }

  //  Write the modified data back to the file
  wad.write(img, data);

  //  Close the file
  img.close();

  //  Tell the user how many bytes were corrupted
  std::cout << corruptions << " bytes corrupted." << std::endl;
  */
}

bool DreamcastCorruption::valid()
{
  return true;
}

/*
Determines whether a given byte can be replaced or not.

No implementation for Dreamcast as of now.
*/
bool DreamcastCorruption::valid_byte(uint8_t byte, uint32_t location)
{
  return true;
}

void DreamcastCorruption::print_header()
{

}

/*
Saves a corrupted rom to the given filename. It will automatically provide
an extension if one is not given.

@param filename - The file to save to.
*/
void DreamcastCorruption::save(std::string filename)
{
  filename += ".cdi";

  if (boost::filesystem::exists(filename) && boost::filesystem::remove(filename) == false)
  {
    throw InvalidFileException("Could not delete already existing file: " + filename);
  }

  if (info->save_file() != "")
  {
    boost::filesystem::copy_file(this->m_temp_file, info->save_file());
  }
  else
  {
    boost::filesystem::copy_file(this->m_temp_file, filename);
  }

  if (boost::filesystem::remove(this->m_temp_file) == false)
  {
    throw InvalidFileException("Could not remove temp file: " + this->m_temp_file);
  }

  this->m_saved = true;
}
