#include "nintendo.h"

using namespace rarc;
using namespace bck;
using namespace bmd;
using namespace btp;
using namespace u8;

std::vector<uint8_t> NintendoFile::start(std::string filename, std::vector<std::string>& args)
{
  return start(Util::read_file(filename), args, filename);
}

std::vector<uint8_t> NintendoFile::start(std::vector<uint8_t>& data, std::vector<std::string>& args, std::string filename)
{
  /*
  //  Won't be used in this version. Looking for a better way to structure this than an if/else if block.
  std::vector<Corrupter> candidates;

  candidates.push_back(Corrupter(BCKFile::corrupt, BCKFile::match));
  candidates.push_back(Corrupter(BTPFile::corrupt, BTPFile::match));
  candidates.push_back(Corrupter(BMDFile::corrupt, BMDFile::match));
  candidates.push_back(Corrupter(BMTFile::corrupt, BMTFile::match));
  candidates.push_back(Corrupter(U8File::corrupt, U8File::match));
  */

  bool yaz0 = Util::read(data, 0, 4) == "Yaz0";

  if (yaz0)
  {
    data = yaz0::decode(data);
  }

  std::string magic = Util::read(data, 0, 4);

  std::cout << "Magic is " << magic << std::endl;

  if (magic == "RARC")
  {
    auto rarc = std::make_unique<RARCFile>(data, args);
    data = rarc->corrupt();
    //rarc->save(filename);
  }
  else if (magic == "J3D1")
  {
    //  For lack of a better approach, similar J3D1-header files will use BCKFile
    if (Util::read(data, 4, 4) == "bck1" ||
        Util::read(data, 4, 4) == "btk1")
    {
      BCKFile::corrupt(data, args);
    }
    else if (Util::read(data, 4, 4) == "btp1")
    {
      //  BTP only needs to jump 0x20 bytes before corrupting
      BTPFile::corrupt(data, args);
    }
  }
  else if (magic == "J3D2")
  {
    if (Util::read(data, 4, 4) == "bmd3")
    {
      BMDFile::corrupt(data, args);
    }
    else if (Util::read(data, 4, 4) == "bmt3")
    {
      //  Just a MAT block of a BMD file
      BMTFile::corrupt(data, args);
    }
  }
  else if (Util::read_big<uint32_t>(data) == 0x55AA382D)
  {
    U8File::corrupt(data, args);
  }
  else
  {
    debug::cout << "No match for file: " << std::hex << Util::read<uint32_t>(data) << "\t" << Util::read<uint32_t>(data, 4) << std::dec << std::endl;
    NintendoFile::corrupt(data, args);
  }

  if (yaz0)
  {
    data = yaz0::encode(data);
  }

  auto info = std::make_unique<CorruptionInfo>(args);

  if (info->save_file() != "")
  {
    Util::write_file(info->save_file(), data);
  }
  else
  {
    Util::write_file(filename, data);
  }

  return data;
}

void NintendoFile::corrupt(std::vector<uint8_t>& data, std::vector<std::string>& args)
{
  debug::cout << "Starting Nintendo data block corruption" << std::endl;
  auto info = std::make_unique<CorruptionInfo>(args);
  uint32_t corruptions = 0;

  std::random_device rd;
  std::mt19937 random;
  std::uniform_int_distribution<int> dist(0x00, 0xFF);

  std::vector<uint32_t> v(std::mt19937::state_size);
  std::generate(v.begin(), v.end(), ref(rd));
  std::seed_seq seed(v.begin(), v.end());
  random.seed(seed);

  for (uint32_t i = info->start();    // Start at header + start value
    i <= data.size() && i < info->end(); // While in a valid range
    i += info->step())                                     // Increase by step size
  {
    if (info->type() == CorruptionType::Shift)
    {
      //  If it's okay to put the other byte in this position then change it
      if (i + info->value() < data.size())
      {
        corruptions++;
        data[i] = data[i + info->value()];
      }
    }
    else if (info->type() == CorruptionType::Swap)
    {
      if (i + info->value() < data.size())
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
      corruptions++;
      data[i] += info->value();
    }
    else if (info->type() == CorruptionType::Set)
    {
      //  If the set value can be placed here then do it
      corruptions++;
      data[i] = info->value();
    }
    else if (info->type() == CorruptionType::Random)
    {
      corruptions++;
      data[i] = dist(random);
    }
    else if (info->type() == CorruptionType::RotateLeft)
    {
      uint8_t rotate = Util::rol<uint8_t>(data[i], info->value());

      data[i] = rotate;
      corruptions++;
    }
    else if (info->type() == CorruptionType::RotateRight)
    {
      uint8_t rotate = Util::ror<uint8_t>(data[i], info->value());

      data[i] = rotate;
      corruptions++;
    }
    else if (info->type() == CorruptionType::LogicalAnd)
    {
      data[i] &= info->value();
      corruptions++;
    }
    else if (info->type() == CorruptionType::LogicalOr)
    {
      data[i] |= info->value();
      corruptions++;
    }
    else if (info->type() == CorruptionType::LogicalXor)
    {
      data[i] ^= info->value();
      corruptions++;
    }
    else if (info->type() == CorruptionType::LogicalComplement)
    {
      data[i] = ~data[i];
      corruptions++;
    }
    else
    {
      break;  //  No corruption selected, might as well quit.
    }
  }

  std::cout << "Replaced a total of " << corruptions << " bytes." << std::endl;
}