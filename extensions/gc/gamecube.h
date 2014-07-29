#ifndef _GAMECUBE_CORRUPTION_H
#define _GAMECUBE_CORRUPTION_H

#include <boost/algorithm/string.hpp>

#include "corrupt.h"
#include "gamecube_except.h"
#include "gamecube_header.h"

#include "gamecube_image.h"

class GamecubeCorruption : public Corruption
{
public:
  GamecubeCorruption();
  GamecubeCorruption(std::string filename, std::vector<std::string>& args);
  ~GamecubeCorruption();

  virtual void corrupt();

  virtual void initialize(std::string filename, std::vector<std::string>& args);
  virtual bool valid();
  virtual bool valid_byte(uint8_t byte, uint32_t location);
  virtual void print_header();
  virtual void save(std::string filename);
private:
  std::unique_ptr<GamecubeImage> rom;
  std::string m_original_file;  //  Original filename
  std::string m_temp_file;      //  Temp file name
  bool m_saved;
  bool m_save;
};

#endif
