#ifndef _PLAYSTATION_CORRUPTION_H
#define _PLAYSTATION_CORRUPTION_H

/*
  Basic Information:
    This file is for Playstation One corruptions. There is no
    real specific information for this corruption as of now.

=============================================================
  Changelog:

  1.0
    Basic corruptions working. Can corrupt a single file in a CD.

  TODO:
    Everything
=============================================================

*/
#include <boost/algorithm/string.hpp>
#include <string>

#include "corrupt.h"

#include "psx_except.h"
#include "psx_header.h"
#include "img/img.h"

class PSXCorruption : public Corruption
{
public:
  PSXCorruption();
  PSXCorruption(std::string filename, std::vector<std::string>& args);
  ~PSXCorruption();

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
