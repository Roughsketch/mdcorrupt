#ifndef _NINTENDO_DS_CORRUPTION_H
#define _NINTENDO_DS_CORRUPTION_H

#include <boost/algorithm/string.hpp>
#include <string>

#include "corrupt.h"

#include "nds_except.h"
#include "nds_header.h"

#include "nds_filesystem.h"

class NDSCorruption : public Corruption
{
public:
  NDSCorruption();
  NDSCorruption(std::string filename, std::vector<std::string>& args);
  ~NDSCorruption();

  virtual void corrupt();

  virtual void initialize(std::string filename, std::vector<std::string>& args);
  virtual bool valid();
  virtual bool valid_byte(uint8_t byte, uint32_t location);
  virtual void print_header();
  virtual void save(std::string filename);
private:
  std::unique_ptr<NDSHeader> header;
  std::unique_ptr<NDSFileSystem> filesystem;

  std::string m_original_file;
  std::string m_temp_file;

  bool m_save;
};

#endif
