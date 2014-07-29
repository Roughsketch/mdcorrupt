#include "gbc_header.h"

GBCHeader::GBCHeader()
{

}

GBCHeader::GBCHeader(std::vector<uint8_t>& rom)
{
  this->initialize(rom);
}

void GBCHeader::initialize(std::vector<uint8_t>& rom)
{
  this->header.reserve(0x50);
  std::copy(rom.begin() + GBCOffset::Start, rom.begin() + GBCOffset::End, std::back_inserter(this->header));
}

uint16_t GBCHeader::entry()
{
  return util::read<uint16_t>(ref(header), GBCLocal::Entry);
}

bool GBCHeader::valid_logo()
{
  static std::vector<uint8_t> logo{ 
    0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B,
    0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
    0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E,
    0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
    0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC,
    0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E 
  };

  bool isequal = std::equal(this->header.begin() + GBCLocal::Logo, this->header.begin() + GBCLocal::Logo + GBCHeader::LogoSize, logo.begin());
  return isequal;
}

std::string GBCHeader::name()
{
  std::string name;
  
  if (this->cgb() & 0x80)
  {
    //  If the CGB flag is active then the title is shorter (11 bytes)
    name = std::string(this->header.begin() + GBCLocal::Title, this->header.begin() + GBCLocal::Title + 0x0B);
  }
  else
  {
    //  If the CGB flag is inactive then the title is longer (15 bytes)
    name = std::string(this->header.begin() + GBCLocal::Title, this->header.begin() + GBCLocal::Title + 0x0F);
  }
  
  return name;
}

uint32_t GBCHeader::manufacturer()
{
  //  Manufacturer is only set when the CGB flag is active
  if (this->cgb() & 0x80)
  {
    return util::read<uint32_t>(ref(header), GBCLocal::Manufacturer);
  }

  //  No CGB flag, no manufacturer
  return 0;
}

uint8_t GBCHeader::cgb()
{
  return this->header[GBCLocal::CGB];
}

uint16_t GBCHeader::company()
{
  return util::read<uint16_t>(ref(header), GBCLocal::Company);
}

uint8_t GBCHeader::sgb()
{
  return this->header[GBCLocal::SGB];
}

uint8_t GBCHeader::type()
{
  /*
    00h  ROM ONLY                 13h  MBC3+RAM+BATTERY
    01h  MBC1                     15h  MBC4
    02h  MBC1+RAM                 16h  MBC4+RAM
    03h  MBC1+RAM+BATTERY         17h  MBC4+RAM+BATTERY
    05h  MBC2                     19h  MBC5
    06h  MBC2+BATTERY             1Ah  MBC5+RAM
    08h  ROM+RAM                  1Bh  MBC5+RAM+BATTERY
    09h  ROM+RAM+BATTERY          1Ch  MBC5+RUMBLE
    0Bh  MMM01                    1Dh  MBC5+RUMBLE+RAM
    0Ch  MMM01+RAM                1Eh  MBC5+RUMBLE+RAM+BATTERY
    0Dh  MMM01+RAM+BATTERY        FCh  POCKET CAMERA
    0Fh  MBC3+TIMER+BATTERY       FDh  BANDAI TAMA5
    10h  MBC3+TIMER+RAM+BATTERY   FEh  HuC3
    11h  MBC3                     FFh  HuC1+RAM+BATTERY
    12h  MBC3+RAM
  */
  return this->header[GBCLocal::Type];
}

uint8_t GBCHeader::rom_size()
{
  /*
    00h -  32KByte (no ROM banking)
    01h -  64KByte (4 banks)
    02h - 128KByte (8 banks)
    03h - 256KByte (16 banks)
    04h - 512KByte (32 banks)
    05h -   1MByte (64 banks)  - only 63 banks used by MBC1
    06h -   2MByte (128 banks) - only 125 banks used by MBC1
    07h -   4MByte (256 banks)
    52h - 1.1MByte (72 banks)
    53h - 1.2MByte (80 banks)
    54h - 1.5MByte (96 banks)
  */
  return this->header[GBCLocal::RomSize];
}

uint8_t GBCHeader::ram_size()
{
  /*
    00h - None
    01h - 2 KBytes
    02h - 8 Kbytes
    03h - 32 KBytes (4 banks of 8KBytes each)
  */
  return this->header[GBCLocal::RamSize];
}

uint8_t GBCHeader::destination()
{
  /*
    00h - Japanese
    01h - Non-Japanese
  */
  return this->header[GBCLocal::Destination];
}

uint8_t GBCHeader::old_license()
{
  return this->header[GBCLocal::OldLicense];
}
