#include "bck_file.h"

namespace bck
{
  namespace detail
  {
    SubHeader::SubHeader(std::vector<uint8_t>& data, uint32_t offset)
    {
      m_magic = Util::read(data, offset, 4);
      m_size = Util::read_big<uint32_t>(data, offset + 4);
    }

    Header::Header(std::vector<uint8_t>& data, uint32_t offset)
    {
      m_magic = Util::read(data, offset, 8);
      m_size = Util::read_big<uint32_t>(data, offset + 8);
      m_ank = SubHeader(data, 0x20);
    }
  }

  void BCKFile::corrupt(std::vector<uint8_t>& data, std::vector<std::string>& args)
  {
    debug::cout << "Starting to corrupt BCK" << std::endl;
    auto info = std::make_unique<CorruptionInfo>(args);
    auto header = std::make_unique<detail::Header>(data);

    std::vector<uint8_t> subdata(data.begin() + header->size(), data.end());

    NintendoFile::corrupt(subdata, args);

    std::copy(subdata.begin(), subdata.end(), data.begin() + header->size());
  }
}