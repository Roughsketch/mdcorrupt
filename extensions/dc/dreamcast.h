#ifndef _SEGA_DREAMCAST_CORRUPTION_H
#define _SEGA_DREAMCAST_CORRUPTION_H

#include <string>
#include <sstream>
#include "corrupt.h"
#include "img/img.h"

class DreamcastCorruption : public Corruption
{
public:
  DreamcastCorruption();
  DreamcastCorruption(std::string filename, std::vector<std::string>& args);
  ~DreamcastCorruption();

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
