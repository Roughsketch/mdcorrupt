#include "bti_file.h"

namespace bti
{
  namespace detail
  {
    Header::Header(std::vector<uint8_t>& data, uint32_t offset)
    {
      m_format = util::read_big<uint8_t>(data, offset + Header::Offset::Format);
      m_unknown1 = util::read_big<uint8_t>(data, offset + Header::Offset::Unknown1);
      m_width = util::read_big<uint16_t>(data, offset + Header::Offset::Width);
      m_height = util::read_big<uint16_t>(data, offset + Header::Offset::Height);
      m_wrap_s = util::read_big<uint8_t>(data, offset + Header::Offset::WrapS);
      m_wrap_t = util::read_big<uint8_t>(data, offset + Header::Offset::WrapT);
      m_unknown2 = util::read_big<uint8_t>(data, offset + Header::Offset::Unknown2);
      m_palette_format = util::read_big<uint8_t>(data, offset + Header::Offset::PaletteFormat);
      m_palette_entries = util::read_big<uint16_t>(data, offset + Header::Offset::PaletteEntries);
      m_palette_offset = util::read_big<uint32_t>(data, offset + Header::Offset::PaletteOffset);
      m_unknown3 = util::read_big<uint32_t>(data, offset + Header::Offset::Unknown3);
      m_min_filter = util::read_big<uint8_t>(data, offset + Header::Offset::MinFilter);
      m_mag_filter = util::read_big<uint8_t>(data, offset + Header::Offset::MagFilter);
      m_unknown4 = util::read_big<uint16_t>(data, offset + Header::Offset::Unknown4);
      m_mipmap_count = util::read_big<uint8_t>(data, offset + Header::Offset::MipMapCount);
      m_unknown5 = util::read_big<uint8_t>(data, offset + Header::Offset::Unknown5);
      m_unknown6 = util::read_big<uint16_t>(data, offset + Header::Offset::Unknown6);
      m_data_offset = util::read_big<uint32_t>(data, offset + Header::Offset::DataOffset);
    }
  }
}