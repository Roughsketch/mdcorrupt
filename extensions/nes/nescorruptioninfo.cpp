#include "nescorruptioninfo.h"

NESCorruptionInfo::NESCorruptionInfo()
{
  m_prg = false;
  m_chr = false;

  m_prg_step = 0;
  m_prg_start = 0;
  m_prg_end = std::numeric_limits<uint32_t>::max();
  m_prg_value = 0;

  m_chr_step = 0;
  m_chr_start = 0;
  m_chr_end = std::numeric_limits<uint32_t>::max();
  m_chr_value = 0;

  m_prg_type = CorruptionType::None;
  m_chr_type = CorruptionType::None;
}

NESCorruptionInfo::NESCorruptionInfo(std::vector<std::string>& args)
{
  //  Set default values
  m_prg = false;
  m_chr = false;

  m_prg_step = 0;
  m_prg_start = 0;
  m_prg_end = std::numeric_limits<uint32_t>::max();
  m_prg_value = 0;

  m_chr_step = 0;
  m_chr_start = 0;
  m_chr_end = std::numeric_limits<uint32_t>::max();
  m_chr_value = 0;

  m_prg_type = CorruptionType::None;
  m_chr_type = CorruptionType::None;

  for (uint32_t i = 0; i < args.size(); i++)
  {
    auto& arg = args[i];

    if (arg == "-s" || arg == "--step")
    {
      m_step = Util::to_int32(args[++i]);
    }
    else if (arg == "-ps" || arg == "--prg-step")
    {
      m_prg_step = Util::to_int32(args[++i]);
      m_prg = true;
    }
    else if (arg == "-cs" || arg == "--chr-step")
    {
      m_chr_step = Util::to_int32(args[++i]);
      m_chr = true;
    }
    else if (arg == "-h" || arg == "--shift")
    {
      m_value = Util::to_int32(args[++i]);
      m_type = CorruptionType::Shift;
    }
    else if (arg == "-ph" || arg == "--prg-shift")
    {
      m_prg_value = Util::to_int32(args[++i]);
      m_prg_type = CorruptionType::Shift;
    }
    else if (arg == "-ch" || arg == "--chr-shift")
    {
      m_chr_value = Util::to_int32(args[++i]);
      m_chr_type = CorruptionType::Shift;
    }
    else if (arg == "-w" || arg == "--swap")
    {
      m_value = Util::to_int32(args[++i]);
      m_type = CorruptionType::Swap;
    }
    else if (arg == "-pw" || arg == "--prg-swap")
    {
      m_prg_value = Util::to_int32(args[++i]);
      m_prg_type = CorruptionType::Swap;
    }
    else if (arg == "-cw" || arg == "--chr-swap")
    {
      m_chr_value = Util::to_int32(args[++i]);
      m_chr_type = CorruptionType::Swap;
    }
    else if (arg == "-a" || arg == "--add")
    {
      m_value = Util::to_int32(args[++i]);
      m_type = CorruptionType::Add;
    }
    else if (arg == "-pa" || arg == "--prg-add")
    {
      m_prg_value = Util::to_int32(args[++i]);
      m_prg_type = CorruptionType::Add;
    }
    else if (arg == "-ca" || arg == "--chr-add")
    {
      m_chr_value = Util::to_int32(args[++i]);
      m_chr_type = CorruptionType::Add;
    }
    else if (arg == "-t" || arg == "--set")
    {
      m_value = Util::to_int32(args[++i]);
      m_type = CorruptionType::Set;
    }
    else if (arg == "-pt" || arg == "--prg-set")
    {
      m_prg_value = Util::to_int32(args[++i]);
      m_prg_type = CorruptionType::Set;
    }
    else if (arg == "-ct" || arg == "--chr-set")
    {
      m_chr_value = Util::to_int32(args[++i]);
      m_chr_type = CorruptionType::Set;
    }
    else if (arg == "-r" || arg == "--random")
    {
      m_type = CorruptionType::Random;
    }
    else if (arg == "-pr" || arg == "--prg-random")
    {
      m_prg_type = CorruptionType::Random;
    }
    else if (arg == "-cr" || arg == "--chr-random")
    {
      m_chr_type = CorruptionType::Random;
    }
    else if (arg == "--rotate-left")
    {
      m_value = Util::to_int32(args[++i]);
      m_type = CorruptionType::RotateLeft;
    }
    else if (arg == "--prg-bitshift-left")
    {
      m_prg_value = Util::to_int32(args[++i]);
      m_prg_type = CorruptionType::RotateLeft;
    }
    else if (arg == "--chr-bitshift-left")
    {
      m_chr_value = Util::to_int32(args[++i]);
      m_chr_type = CorruptionType::RotateLeft;
    }
    else if (arg == "--rotate-right")
    {
      m_value = Util::to_int32(args[++i]);
      m_type = CorruptionType::RotateRight;
    }
    else if (arg == "--prg-bitshift-right")
    {
      m_prg_value = Util::to_int32(args[++i]);
      m_prg_type = CorruptionType::RotateRight;
    }
    else if (arg == "--chr-bitshift-right")
    {
      m_chr_value = Util::to_int32(args[++i]);
      m_chr_type = CorruptionType::RotateRight;
    }
    else if (arg == "--logical-and")
    {
      m_value = Util::to_int32(args[++i]);
      m_type = CorruptionType::LogicalAnd;
    }
    else if (arg == "--prg-logical-and")
    {
      m_prg_value = Util::to_int32(args[++i]);
      m_prg_type = CorruptionType::LogicalAnd;
    }
    else if (arg == "--chr-logical-and")
    {
      m_chr_value = Util::to_int32(args[++i]);
      m_chr_type = CorruptionType::LogicalAnd;
    }
    else if (arg == "--logical-or")
    {
      m_value = Util::to_int32(args[++i]);
      m_type = CorruptionType::LogicalOr;
    }
    else if (arg == "--prg-logical-or")
    {
      m_prg_value = Util::to_int32(args[++i]);
      m_prg_type = CorruptionType::LogicalOr;
    }
    else if (arg == "--chr-logical-or")
    {
      m_chr_value = Util::to_int32(args[++i]);
      m_chr_type = CorruptionType::LogicalOr;
    }
    else if (arg == "--logical-xor")
    {
      m_value = Util::to_int32(args[++i]);
      m_type = CorruptionType::LogicalXor;
    }
    else if (arg == "--prg-logical-xor")
    {
      m_prg_value = Util::to_int32(args[++i]);
      m_prg_type = CorruptionType::LogicalXor;
    }
    else if (arg == "--chr-logical-xor")
    {
      m_chr_value = Util::to_int32(args[++i]);
      m_chr_type = CorruptionType::LogicalXor;
    }
    else if (arg == "--logical-complement")
    {
      m_type = CorruptionType::LogicalComplement;
    }
    else if (arg == "--prg-logical-complement")
    {
      m_prg_type = CorruptionType::LogicalComplement;
    }
    else if (arg == "--chr-logical-complement")
    {
      m_chr_type = CorruptionType::LogicalComplement;
    }
    else if (arg == "-pb" || arg == "--prg-start")
    {
      m_prg_start = Util::to_int32(args[++i]);
    }
    else if (arg == "-cb" || arg == "--chr-start")
    {
      m_chr_start = Util::to_int32(args[++i]);
    }
    else if (arg == "-e" || arg == "--stop")
    {
      m_end = Util::to_int32(args[++i]);
    }
    else if (arg == "-pe" || arg == "--prg-stop")
    {
      m_prg_end = Util::to_int32(args[++i]);
    }
    else if (arg == "-ce" || arg == "--chr-stop")
    {
      m_chr_end = Util::to_int32(args[++i]);
    }
    else if (arg == "-o" || arg == "--out")
    {
      m_outfile = args[++i];
    }
    else
    {
      //  Ignore it instead of throwing an exception
      //throw InvalidArgumentException(arg);
    }
  }
}
