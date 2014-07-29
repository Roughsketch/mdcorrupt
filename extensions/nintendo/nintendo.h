#ifndef _NINTENDO_FILE_PROTECTION_H
#define _NINTENDO_FILE_PROTECTION_H

#include <vector>
#include <memory>
#include <cstdint>
#include <random>
#include <functional>

#include "corrupt.h"
#include "log.h"
#include "yaz0.h"

struct Corrupter
{
  Corrupter(std::function<void(std::vector<uint8_t>&, std::vector<std::string>&)> c,
            std::function<bool(std::vector<uint8_t>&)> m) : m_corrupt(c), m_match(m) {};
private:
  std::function<void(std::vector<uint8_t>&, std::vector<std::string>&)> m_corrupt;
  std::function<bool(std::vector<uint8_t>&)> m_match;
};

class NintendoFile
{
public:
  virtual ~NintendoFile();

  static std::vector<uint8_t> start(std::string file, std::vector<std::string>& args);
  static std::vector<uint8_t> start(std::vector<uint8_t>& data, std::vector<std::string>& args, std::string filename = "");
  static void corrupt(std::vector<uint8_t>& data, std::vector<std::string>& args);

  virtual bool valid_byte() = 0;
};

#include "rarc_file.h"
#include "bck_file.h"
#include "bmd_file.h"
#include "bti_file.h"
#include "btp_file.h"

#include "u8_file.h"
#endif
