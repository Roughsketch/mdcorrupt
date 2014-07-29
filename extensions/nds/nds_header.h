#ifndef _NINTENDO_DS_HEADER_H
#define _NINTENDO_DS_HEADER_H

#include <cstdint>
#include <vector>
#include <iterator>

#include "helpers.h"

namespace NDSOffset
{
  enum NDS
  {
    Title = 0x00,
    GameCode = 0x0C,
    MakerCode = 0x10,
    UnitCode = 0x12,
    EncryptionSeed = 0x13,
    Capacity = 0x14,
    Version = 0x1E,
    AutoStart = 0x1F,
    ARM9Rom = 0x20,
    ARM9Entry = 0x24,
    ARM9RAM = 0x28,
    ARM9Size = 0x2C,
    ARM7Rom = 0x30,
    ARM7Entry = 0x34,
    ARM7RAM = 0x38,
    ARM7Size = 0x3C,
    FileTableOffset = 0x40,
    FileTableSize = 0x44,
    FileAllocationOffset = 0x48,
    FileAllocationSize = 0x4C,
    ARM9Overlay = 0x50,
    ARM9OverlaySize = 0x54,
    ARM7Overlay = 0x58,
    ARM7OverlaySize = 0x5C,
    CommandPortNormal = 0x60,
    CommandPortKey1 = 0x64,
    IconTitle = 0x68,
    SecureChecksum = 0x6C,
    SecureLoadingTimeout = 0x6E,
    ARM9AutoLoadRAM = 0x70,
    ARM7AutoLoadRAM = 0x74,
    SecureAreaDisable = 0x78,
    SizeUsed = 0x80,
    HeaderSize = 0x84,
    Logo = 0xC0,
    LogoChecksum = 0x15C,
    HeaderChecksum = 0x15E,
    DebugRomOffset = 0x160,
    DebugSize = 0x164,
    DebugRAMAddress = 0x168,
  };
}

struct NDSHeader
{
  static const int Header_Size = 0x200;

  NDSHeader();
  NDSHeader(std::vector<uint8_t>& rom);
  ~NDSHeader();

  void initialize(std::vector<uint8_t>&);

  uint32_t begin();

  std::string title();
  std::string game_code();
  std::string maker_code();

  uint8_t unit_code();
  uint8_t encryption_seed();
  uint32_t capacity();
  uint8_t version();
  uint8_t autostart();

  uint32_t arm9_rom_offset();
  uint32_t arm9_entry_address();
  uint32_t arm9_ram_address();
  uint32_t arm9_size();

  uint32_t arm7_rom_offset();
  uint32_t arm7_entry_address();
  uint32_t arm7_ram_address();
  uint32_t arm7_size();

  uint32_t file_name_table() { return Util::read<uint32_t>(ref(this->header), NDSOffset::FileTableOffset); }
  uint32_t file_name_size();
  uint32_t file_alloc_table();
  uint32_t file_alloc_size();

  uint32_t command_port_normal();
  uint32_t command_port_key1();

  uint32_t icon_title_offset();

  uint16_t secure_checksum();
  uint16_t secure_loading_timeout();

  uint32_t arm9_auto_load();
  uint32_t arm7_auto_load();

  uint64_t secure_area_disable();

  uint32_t size_used();
  uint32_t header_size();
private:
  std::vector<uint8_t> header;
  uint32_t header_offset;
};

inline uint32_t NDSHeader::begin()
{
  return 0x8000; // Not testing for any values yet, but 0x8000 will skip ARM9 boot loading code.
}

inline std::string NDSHeader::title()
{
  return std::string(header.begin() + NDSOffset::Title, header.begin() + NDSOffset::Title + 12);
}

inline std::string NDSHeader::game_code()
{
  return std::string(header.begin() + NDSOffset::GameCode, header.begin() + NDSOffset::GameCode + 4);
}

inline std::string NDSHeader::maker_code()
{
  return std::string(header.begin() + NDSOffset::MakerCode, header.begin() + NDSOffset::MakerCode + 2);
}

inline uint8_t NDSHeader::unit_code()
{
  return header[NDSOffset::UnitCode];
}

inline uint8_t NDSHeader::encryption_seed()
{
  return header[NDSOffset::EncryptionSeed];
}

inline uint32_t NDSHeader::capacity()
{
  return 0x20000 << header[NDSOffset::Capacity];
}

inline uint8_t NDSHeader::version()
{
  return header[NDSOffset::Version];
}

inline uint8_t NDSHeader::autostart()
{
  return header[NDSOffset::AutoStart];
}

inline uint32_t NDSHeader::arm9_rom_offset()
{
  return Util::read<uint32_t>(ref(this->header), NDSOffset::ARM9Rom);
}

inline uint32_t NDSHeader::arm9_entry_address()
{
  return Util::read<uint32_t>(ref(this->header), NDSOffset::ARM9Entry);
}

inline uint32_t NDSHeader::arm9_ram_address()
{
  return Util::read<uint32_t>(ref(this->header), NDSOffset::ARM9RAM);
}

inline uint32_t NDSHeader::arm9_size()
{
  return Util::read<uint32_t>(ref(this->header), NDSOffset::ARM9Size);
}

inline uint32_t NDSHeader::arm7_rom_offset()
{
  return Util::read<uint32_t>(ref(this->header), NDSOffset::ARM7Rom);
}

inline uint32_t NDSHeader::arm7_entry_address()
{
  return Util::read<uint32_t>(ref(this->header), NDSOffset::ARM7Entry);
}

inline uint32_t NDSHeader::arm7_ram_address()
{
  return Util::read<uint32_t>(ref(this->header), NDSOffset::ARM7RAM);
}

inline uint32_t NDSHeader::arm7_size()
{
  return Util::read<uint32_t>(ref(this->header), NDSOffset::ARM7Size);
}

inline uint32_t NDSHeader::file_name_size()
{
  return Util::read<uint32_t>(ref(this->header), NDSOffset::FileTableSize);
}

inline uint32_t NDSHeader::file_alloc_table()
{
  return Util::read<uint32_t>(ref(this->header), NDSOffset::FileAllocationOffset);
}

inline uint32_t NDSHeader::file_alloc_size()
{
  return Util::read<uint32_t>(ref(this->header), NDSOffset::FileAllocationSize);
}

inline uint32_t NDSHeader::command_port_normal()
{
  return Util::read<uint32_t>(ref(this->header), NDSOffset::CommandPortNormal);
}

inline uint32_t NDSHeader::command_port_key1()
{
  return Util::read<uint32_t>(ref(this->header), NDSOffset::CommandPortKey1);
}

inline uint32_t NDSHeader::icon_title_offset()
{
  return Util::read<uint32_t>(ref(this->header), NDSOffset::IconTitle);
}

inline uint16_t NDSHeader::secure_checksum()
{
  return Util::read<uint16_t>(ref(this->header), NDSOffset::SecureChecksum);
}

inline uint16_t NDSHeader::secure_loading_timeout()
{
  return Util::read<uint16_t>(ref(this->header), NDSOffset::SecureLoadingTimeout);
}

inline uint32_t NDSHeader::arm9_auto_load()
{
  return Util::read<uint32_t>(ref(this->header), NDSOffset::ARM9AutoLoadRAM);
}

inline uint32_t NDSHeader::arm7_auto_load()
{
  return Util::read<uint32_t>(ref(this->header), NDSOffset::ARM7AutoLoadRAM);
}

inline uint64_t NDSHeader::secure_area_disable()
{
  return Util::read<uint64_t>(ref(this->header), NDSOffset::SecureAreaDisable);
}

inline uint32_t NDSHeader::size_used()
{
  return Util::read<uint32_t>(ref(this->header), NDSOffset::SizeUsed);
}

inline uint32_t NDSHeader::header_size()
{
  return Util::read<uint32_t>(ref(this->header), NDSOffset::HeaderSize);
}

#endif
