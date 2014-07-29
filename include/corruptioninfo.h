#ifndef _CORRUPTION_INFO_H
#define _CORRUPTION_INFO_H

#include <boost/algorithm/string.hpp>
#include <cstdint>
#include <string>
#include <limits>
#include <vector>

#include "util.h"

enum CorruptionType
{
  None,
  Shift,
  Swap,
  Add,
  Set,
  Random,
  RotateLeft,
  RotateRight,
  LogicalAnd,
  LogicalOr,
  LogicalXor,
  LogicalComplement
};

class CorruptionInfo
{
public:
  CorruptionInfo();
  CorruptionInfo(std::vector<std::string>& args);

  void add_file(std::string filename);
  std::vector<std::string> files();

  std::string save_file();
  void set_save_file(std::string);
  CorruptionType type();
  uint32_t value();
  uint32_t step();
  uint32_t start();
  uint32_t end();
  bool list();
protected:
  std::string m_outfile;  //  Output file path
  CorruptionType m_type;  //  What type of corruption

  //  Basic information
  uint32_t m_step;
  uint32_t m_start;
  uint32_t m_end;
  uint32_t m_value;

  bool m_list;  //  Listing file contents (JSON)

  //  Files to corrupt if files are implemented
  std::vector<std::string> m_file_list;
};

inline void CorruptionInfo::add_file(std::string filename)
{
  m_file_list.push_back(filename);
}

inline std::vector<std::string> CorruptionInfo::files()
{
  return std::vector<std::string>(m_file_list);
}

inline std::string CorruptionInfo::save_file()
{
  return m_outfile;
}

inline void CorruptionInfo::set_save_file(std::string value)
{
  m_outfile = value;
}

inline CorruptionType CorruptionInfo::type()
{
  return m_type;
}

inline uint32_t CorruptionInfo::value()
{
  return m_value;
}

inline uint32_t CorruptionInfo::step()
{
  return m_step;
}

inline uint32_t CorruptionInfo::start()
{
  return m_start;
}

inline uint32_t CorruptionInfo::end()
{
  return m_end;
}

inline bool CorruptionInfo::list()
{
  return m_list;
}

#endif
