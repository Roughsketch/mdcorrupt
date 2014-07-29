#include "n64_header.h"

N64Header::N64Header()
{

}

N64Header::N64Header(std::vector<uint8_t>& rom)
{
  this->initialize(rom);
}

void N64Header::initialize(std::vector<uint8_t>& rom)
{
  this->header.reserve(0x3F);
  std::copy(rom.begin(), rom.begin() + 0x3F, this->header.begin());
}

uint32_t N64Header::domain()
{
  return util::read<uint32_t>(ref(header), N64Offset::Domain);
}

uint32_t N64Header::clock_rate()
{
  return util::read<uint32_t>(ref(header), N64Offset::ClockRate);
}

uint32_t N64Header::program_counter()
{
  return util::read<uint32_t>(ref(header), N64Offset::ProgramCounter);
}

uint32_t N64Header::release()
{
  return util::read<uint32_t>(ref(header), N64Offset::Release);
}

uint32_t N64Header::crc1()
{
  return util::read<uint32_t>(ref(header), N64Offset::CRC1);
}

uint32_t N64Header::crc2()
{
  return util::read<uint32_t>(ref(header), N64Offset::CRC2);
}

std::string N64Header::name()
{
  std::string name = std::string(header.begin() + N64Offset::Name, header.begin() + N64Offset::Name + 20);
  return name;
}

char N64Header::format()
{
  return static_cast<char>(util::read<uint32_t>(ref(header), N64Offset::Format));
}

uint16_t N64Header::id()
{
  return util::read<uint16_t>(ref(header), N64Offset::ID);
}

uint8_t N64Header::region()
{
  return util::read<uint8_t>(ref(header), N64Offset::Region);
}

double N64Header::version()
{
  return util::read<uint8_t>(ref(header), N64Offset::Version);
}

bool N64Header::is_big_endian()
{
  return (this->domain() & 0x80000000) == 1;
}

bool N64Header::is_little_endian()
{
  return (this->domain() & 0x80000000) == 0;
}
