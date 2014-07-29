#include "bti_file.h"

namespace bti
{
  namespace detail
  {
    Header::Header(std::vector<uint8_t>& data, uint32_t offset)
    {
      m_format = Util::read_big<uint8_t>(data, offset + Header::Offset::Format);
      m_unknown1 = Util::read_big<uint8_t>(data, offset + Header::Offset::Unknown1);
      m_width = Util::read_big<uint16_t>(data, offset + Header::Offset::Width);
      m_height = Util::read_big<uint16_t>(data, offset + Header::Offset::Height);
      m_wrap_s = Util::read_big<uint8_t>(data, offset + Header::Offset::WrapS);
      m_wrap_t = Util::read_big<uint8_t>(data, offset + Header::Offset::WrapT);
      m_unknown2 = Util::read_big<uint8_t>(data, offset + Header::Offset::Unknown2);
      m_palette_format = Util::read_big<uint8_t>(data, offset + Header::Offset::PaletteFormat);
      m_palette_entries = Util::read_big<uint16_t>(data, offset + Header::Offset::PaletteEntries);
      m_palette_offset = Util::read_big<uint32_t>(data, offset + Header::Offset::PaletteOffset);
      m_unknown3 = Util::read_big<uint32_t>(data, offset + Header::Offset::Unknown3);
      m_min_filter = Util::read_big<uint8_t>(data, offset + Header::Offset::MinFilter);
      m_mag_filter = Util::read_big<uint8_t>(data, offset + Header::Offset::MagFilter);
      m_unknown4 = Util::read_big<uint16_t>(data, offset + Header::Offset::Unknown4);
      m_mipmap_count = Util::read_big<uint8_t>(data, offset + Header::Offset::MipMapCount);
      m_unknown5 = Util::read_big<uint8_t>(data, offset + Header::Offset::Unknown5);
      m_unknown6 = Util::read_big<uint16_t>(data, offset + Header::Offset::Unknown6);
      m_data_offset = Util::read_big<uint32_t>(data, offset + Header::Offset::DataOffset);
    }
  }
}