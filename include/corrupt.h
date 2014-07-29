#ifndef _CORRUPTION_H
#define _CORRUPTION_H

#include <boost/filesystem.hpp>

#include "corruption_exceptions.h"
#include "corruptioninfo.h"

#include <ctime>
#include <cstdint>
#include <cstdlib>

#include <limits>
#include <random>
#include <vector>
#include <fstream>
#include <iterator>
#include <string>
#include <algorithm>
#include <memory>

#include "patch.h"

#include "log.h"

class Corruption
{
public:
  Corruption();
  Corruption(std::string filename, std::vector<std::string>& args);

  virtual ~Corruption();

  virtual void corrupt();
  virtual void save(std::string filename);

  virtual int size();

  virtual void initialize(std::string filename, std::vector<std::string>& args);
  virtual void print_header();
  virtual bool valid();
  virtual bool valid_byte(uint8_t byte, uint32_t location);
  virtual void run();
protected:
  //  Holds the raw rom data
  std::vector<uint8_t> rom;
  //  fstream for reading and writing to files
  std::fstream rom_file;
  //  Last save name for running/opening the file
  std::string save_name;

  std::random_device rd;
  std::mt19937 random;

  std::unique_ptr<CorruptionInfo> info;

  void read(std::string filename);
};

// Add new Corruption class headers here
#include "nes/nes.h"
#include "snes/snes.h"
#include "n64/n64.h"
#include "gbc/gbc.h"
#include "psx/psx.h"
#include "nds/nds.h"
#include "gba/gba.h"
#include "gc/gamecube.h"
#include "genesis/genesis.h"
#include "dc/dreamcast.h"

#include "nintendo/nintendo.h"
#endif
