#ifndef _GENESIS_CORRUPTION_H
#define _GENESIS_CORRUPTION_H

#include "corrupt.h"

#include "genesis_except.h"
#include "genesis_header.h"

class GenesisCorruption : public Corruption
{
public:
  GenesisCorruption();
  GenesisCorruption(std::string filename, std::vector<std::string>& args);
  ~GenesisCorruption();

  virtual void initialize(std::string filename, std::vector<std::string>& args);
  virtual bool valid();
  virtual bool valid_byte(uint8_t byte, uint32_t location);
  virtual void print_header();
  virtual void save(std::string filename);

private:
  std::unique_ptr<GenesisHeader> header;

  static bool is_branch(uint16_t instruction);

  uint16_t checksum();
};

#endif
