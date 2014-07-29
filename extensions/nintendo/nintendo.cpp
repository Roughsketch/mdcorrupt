#include "nintendo.h"

#include "timer.h"

using namespace rarc;
using namespace bck;
using namespace bmd;
using namespace btp;
using namespace u8;

std::vector<uint8_t> NintendoFile::start(std::string filename, std::vector<std::string>& args)
{
  return start(util::read_file(filename), args, filename);
  //return start(std::vector<uint8_t>(), args, filename);
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

  uint64_t current = 0;

  if (data.size() < 4)
  {
    return data;
  }

  bool yaz0 = util::read(data, 0, 4) == "Yaz0";
  bool yay0 = util::read(data, 0, 4) == "Yay0";

  if (yaz0)
  {
    data = yaz0::decode(data);
  }

  if (yay0)
  {
    data = yay0::decode(data);
  }

  std::string magic = util::read(data, 0, 4);

  try
  {
    if (magic == "RARC")
    {
      auto rarc = std::make_unique<RARCFile>(data, args);
      data = rarc->corrupt();
    }
    else if (magic == "J3D1")
    {
      //  For lack of a better approach, similar J3D1-header files will use BCKFile
      if (util::read(data, 4, 4) == "bck1" ||
          util::read(data, 4, 4) == "btk1")
      {
        BCKFile::corrupt(data, args);
      }
      else if (util::read(data, 4, 4) == "btp1")
      {
        //  BTP only needs to jump 0x20 bytes before corrupting
        BTPFile::corrupt(data, args);
      }
    }
    else if (magic == "J3D2")
    {
      if (util::read(data, 4, 4) == "bmd3")
      {
        BMDFile::corrupt(data, args);
      }
      else if (util::read(data, 4, 4) == "bmt3")
      {
        //  Just a MAT block of a BMD file
        BMTFile::corrupt(data, args);
      }
    }
    else if (util::read_big<uint32_t>(data) == 0x55AA382D)
    {
      U8File::corrupt(data, args);
    }
    else
    {
      NintendoFile::corrupt(data, args);
    }
  }
  catch (...)
  {
    std::cout << "Error corrupting '" << filename << "'" << std::endl;
  }

  /*
    Decoding is disabled for now
  if (yaz0)
  {
    //std::cout << "Yaz0 Encoding... ";
    //data = yaz0::encode(data);
    //std::cout << "Done" << std::endl;
  }
  */

  auto info = std::make_unique<CorruptionInfo>(args);

  if (filename != "")
  {
    if (info->save_file() != "")
    {
      util::write_file(info->save_file(), data);
    }
    else
    {
      util::write_file(filename, data);
    }
  }

  return data;
}

void NintendoFile::corrupt(std::vector<uint8_t>& data, std::vector<std::string>& args)
{
  auto info = std::make_unique<CorruptionInfo>(args);
  uint32_t corruptions = 0;

  std::random_device rd;
  std::mt19937 random;
  std::uniform_int_distribution<int> dist(0x00, 0xFF);

  std::vector<uint32_t> v(std::mt19937::state_size);
  std::generate(v.begin(), v.end(), ref(rd));
  std::seed_seq seed(v.begin(), v.end());
  random.seed(seed);

  for (uint32_t i = info->start();        // Start at header + start value
    i < data.size() && i < info->end();   // While in a valid range
    i += info->step())                    // Increase by step size
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
      uint8_t rotate = util::rol<uint8_t>(data[i], info->value());

      data[i] = rotate;
      corruptions++;
    }
    else if (info->type() == CorruptionType::RotateRight)
    {
      uint8_t rotate = util::ror<uint8_t>(data[i], info->value());

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

  //  Use stringstream so that lines won't be mangled from multi-threading
  std::stringstream ss;
  ss << "Replaced a total of " << corruptions << " bytes." << std::endl;
  std::cout << ss.str();
}