#ifndef _NES_CORRUPTION_INFO_H
#define _NES_CORRUPTION_INFO_H

#include "corruptioninfo.h"

#include <cstdint>
#include <limits>

#include "util.h"

class NESCorruptionInfo : public CorruptionInfo
{
public:
  NESCorruptionInfo();
  NESCorruptionInfo(std::vector<std::string>& args);

  bool prg();
  bool chr();

  uint32_t prg_step();
  uint32_t prg_start();
  uint32_t prg_end();
  uint32_t prg_value();

  uint32_t chr_step();
  uint32_t chr_start();
  uint32_t chr_end();
  uint32_t chr_value();

  CorruptionType prg_type();
  CorruptionType chr_type();
private:
  bool m_prg;
  bool m_chr;

  uint32_t m_prg_step;
  uint32_t m_prg_start;
  uint32_t m_prg_end;

  uint32_t m_prg_value;

  uint32_t m_chr_step;
  uint32_t m_chr_start;
  uint32_t m_chr_end;

  uint32_t m_chr_value;

  CorruptionType m_prg_type;
  CorruptionType m_chr_type;
};

inline bool NESCorruptionInfo::prg()
{
  return m_prg;
}

inline bool NESCorruptionInfo::chr()
{
  return m_chr;
}

inline uint32_t NESCorruptionInfo::prg_step()
{
  return m_prg_step;
}

inline uint32_t NESCorruptionInfo::prg_start()
{
  return m_prg_start;
}

inline uint32_t NESCorruptionInfo::prg_end()
{
  return m_prg_end;
}

inline uint32_t NESCorruptionInfo::prg_value()
{
  return m_prg_value;
}


inline uint32_t NESCorruptionInfo::chr_step()
{
  return m_chr_step;
}

inline uint32_t NESCorruptionInfo::chr_start()
{
  return m_chr_start;
}

inline uint32_t NESCorruptionInfo::chr_end()
{
  return m_chr_end;
}

inline uint32_t NESCorruptionInfo::chr_value()
{
  return m_chr_value;
}


inline CorruptionType NESCorruptionInfo::prg_type()
{
  return m_prg_type;
}

inline CorruptionType NESCorruptionInfo::chr_type()
{
  return m_chr_type;
}

#endif
