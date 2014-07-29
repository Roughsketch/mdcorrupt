#ifndef _GBA_HEADER_H
#define _GBA_HEADER_H

#include <cstdint>
#include <vector>

#include "util.h"

namespace GBAOffset
{
  enum GBA
  {
    Entry = 0x00,
    Logo = 0x04,
    Title = 0xA0,
    GameCode = 0xAC,
    MakerCode = 0xB0,
    Fixed = 0xB2,
    UnitCode = 0xB3,
    DeviceType = 0xB4,
    Version = 0xBC,
    Complement = 0xBD
  };
}

struct GBAHeader
{
  const static uint32_t Size = 0xC0;
  const static uint32_t MaxSize = 0x200; // Arbitrary value. Used just to be safe of multi-boot headers.

  GBAHeader();
  GBAHeader(std::vector<uint8_t>&);

  void initialize(std::vector<uint8_t>&);

  bool valid();

  uint32_t entry_point();
  std::string title();
  std::string game_code();
  std::string maker_code();
  uint8_t software_version();
private:
  std::vector<uint8_t> header;
};

inline uint32_t GBAHeader::entry_point()
{
  return util::read<uint32_t>(ref(header), GBAOffset::Entry);
}

inline std::string GBAHeader::title()
{
  return util::read(ref(header), GBAOffset::Title, 12);
}

inline std::string GBAHeader::game_code()
{
  return util::read(ref(header), GBAOffset::GameCode, 4);
}

inline std::string GBAHeader::maker_code()
{
  return util::read(ref(header), GBAOffset::MakerCode, 2);
}

inline uint8_t GBAHeader::software_version()
{
  return header[GBAOffset::Version];
}

#endif
