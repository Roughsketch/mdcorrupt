#ifndef _GBA_CORRUPTION_H
#define _GBA_CORRUPTION_H

#include <string>

#include "corrupt.h"

#include "gba_except.h"
#include "gba_header.h"

class GBACorruption : public Corruption
{
public:
  GBACorruption();
  GBACorruption(std::string filename, std::vector<std::string>& args);
  ~GBACorruption();

  virtual void initialize(std::string filename, std::vector<std::string>& args);
  virtual bool valid();
  virtual bool valid_byte(uint8_t byte, uint32_t location);
  virtual void print_header();
  virtual void save(std::string filename);
private:
  std::unique_ptr<GBAHeader> header;
};

#endif
