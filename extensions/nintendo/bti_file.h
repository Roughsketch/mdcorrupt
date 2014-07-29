#ifndef _BTI_FILE_H
#define _BTI_FILE_H

#include "nintendo.h"

namespace bti
{
  namespace detail
  {
    struct Header
    {
      Header(std::vector<uint8_t>& data, uint32_t offset = 0);
    private:
      uint8_t m_format;
      uint8_t m_unknown1;
      uint16_t m_width;
      uint16_t m_height;
      uint8_t m_wrap_s;
      uint8_t m_wrap_t;
      uint8_t m_unknown2;
      uint8_t m_palette_format;
      uint16_t m_palette_entries;
      uint32_t m_palette_offset;
      uint32_t m_unknown3;
      uint8_t m_min_filter;
      uint8_t m_mag_filter;
      uint16_t m_unknown4;
      uint8_t m_mipmap_count;
      uint8_t m_unknown5;
      uint16_t m_unknown6;
      uint32_t m_data_offset;

      enum Offset
      {
        Format = 0,
        Unknown1 = 1,
        Width = 2,
        Height = 4,
        WrapS = 6,
        WrapT = 7,
        Unknown2 = 8,
        PaletteFormat = 9,
        PaletteEntries = 10,
        PaletteOffset = 12,
        Unknown3 = 16,
        MinFilter = 20,
        MagFilter = 21,
        Unknown4 = 22,
        MipMapCount = 24,
        Unknown5 = 25,
        Unknown6 = 26,
        DataOffset = 28
      };
    };
  }

  class BTIFile
  {
  public:

  private:

  };
}

#endif
