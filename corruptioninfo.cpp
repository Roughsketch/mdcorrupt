#include "corruptioninfo.h"

CorruptionInfo::CorruptionInfo()
{
  m_outfile = "";
  m_type = CorruptionType::None;


  m_step = 0;
  m_start = 0;
  m_end = std::numeric_limits<uint32_t>::max();
  m_value = 0;

  m_list = false;
}

CorruptionInfo::CorruptionInfo(std::vector<std::string>& args)
{
  //  Set default values
  m_outfile = "";
  m_type = CorruptionType::None;

  m_step = 0;
  m_start = 0;
  m_end = std::numeric_limits<uint32_t>::max();
  m_value = 0;

  m_list = false;

  for (uint32_t i = 0; i < args.size(); i++)
  {
    auto& arg = args[i];

    if (arg == "-s" || arg == "--step")
    {
      m_step = Util::to_int32(args[++i]);
    }
    else if (arg == "-h" || arg == "--shift")
    {
      m_value = m_value = Util::to_int32(args[++i]);
      m_type = CorruptionType::Shift;
    }
    else if (arg == "-w" || arg == "--swap")
    {
      m_value = m_value = Util::to_int32(args[++i]);
      m_type = CorruptionType::Swap;
    }
    else if (arg == "-a" || arg == "--add")
    {
      m_value = Util::to_int32(args[++i]);
      m_type = CorruptionType::Add;
    }
    else if (arg == "-t" || arg == "--set")
    {
      m_value = Util::to_int32(args[++i]);
      m_type = CorruptionType::Set;
    }
    else if (arg == "-r" || arg == "--random")
    {
      m_type = CorruptionType::Random;
    }
    else if (arg == "--rotate-left")
    {
      m_value = Util::to_int32(args[++i]);
      m_type = CorruptionType::RotateLeft;
    }
    else if (arg == "--rotate-right")
    {
      m_value = Util::to_int32(args[++i]);
      m_type = CorruptionType::RotateRight;
    }
    else if (arg == "--logical-and")
    {
      m_value = Util::to_int32(args[++i]);
      m_type = CorruptionType::LogicalAnd;
    }
    else if (arg == "--logical-or")
    {
      m_value = Util::to_int32(args[++i]);
      m_type = CorruptionType::LogicalOr;
    }
    else if (arg == "--logical-xor")
    {
      m_value = Util::to_int32(args[++i]);
      m_type = CorruptionType::LogicalXor;
    }
    else if (arg == "--logical-complement")
    {
      m_type = CorruptionType::LogicalComplement;
    }
    else if (arg == "-b" || arg == "--start")
    {
      m_start = Util::to_int32(args[++i]);
    }
    else if (arg == "-e" || arg == "--stop")
    {
      m_end = Util::to_int32(args[++i]);
    }
    else if (arg == "-o" || arg == "--out")
    {
      m_outfile = args[i + 1];
      i++;
    }
    else if (arg == "-f" || arg == "--files")
    {
      for (uint32_t file = i + 1; file < args.size() && args[file][0] != '-'; file++)
      {
        add_file(args[++i]);
      }
    }
    else if (arg == "--filelist")
    {
      std::ifstream ifs(args[i + 1]);

      if (ifs.is_open())
      {
        std::string raw((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        std::vector<std::string> files;

        boost::split(files, raw, boost::is_any_of(" "), boost::token_compress_on);

        for (auto& file : files)
        {
          add_file(file);
        }
      }
      else
      {
        //throw InvalidArgumentException("Could not open file " + args[i + 1]);
        std::cerr << "Could not open file " << args[i + 1] << std::endl;
      }

      ifs.close();
      i++;
    }
    else if (arg == "--list")
    {
      m_list = true;
      //std::cout << "JSON File Listing: " << rom->to_json() << std::endl;
    }
    else
    {
      //throw InvalidArgumentException(arg + " is not a recognized flag.");
      std::cerr << arg << " is not a recognized flag." << std::endl;
    }
  }
}
