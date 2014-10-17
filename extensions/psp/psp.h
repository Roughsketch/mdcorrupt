#ifndef _PSP_CORRUPTION_H
#define _PSP_CORRUPTION_H
#include <boost/algorithm/string.hpp>
#include <string>

#include "corrupt.h"
#include "psx/psx_except.h"

#include "img/img.h"

class PSPCorruption : public Corruption
{
public:
  PSPCorruption();
  PSPCorruption(std::string filename, std::vector<std::string>& args);
  ~PSPCorruption();

  virtual void corrupt();

  virtual void initialize(std::string filename, std::vector<std::string>& args);
  virtual bool valid();
  virtual bool valid_byte(uint8_t byte, uint32_t location);
  virtual void print_header();
  virtual void save(std::string filename);

private:
  std::unique_ptr<IMG> rom;

  std::string m_original_file;  //  Original filename
  std::string m_temp_file;      //  Temp file name
  bool m_saved;
};

#endif
