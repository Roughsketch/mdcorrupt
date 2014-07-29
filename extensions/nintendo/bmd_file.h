#ifndef _BMD_FILE_H
#define _BMD_FILE_H

#include "nintendo.h"

namespace bmd
{
  namespace detail
  {
    struct INFHeader
    {
      INFHeader(std::vector<uint8_t>& data, uint32_t offset = 0);

      inline uint32_t size()
      {
        return m_size;
      }

      inline uint32_t offset()
      {
        return m_header_offset;
      }

      inline uint32_t begin()
      {
        //  Start at data offset which is relative to the header
        return m_header_offset + m_data_offset;
      }

      inline uint32_t end()
      {
        //  Go to end of this section
        return m_header_offset + m_size;
      }

    private:
      std::string m_magic;
      uint32_t m_size;
      uint16_t m_unknown1;
      uint16_t m_pad;
      uint32_t m_unknown2;
      uint32_t m_vertex_count;
      uint32_t m_data_offset;

      uint32_t m_header_offset;

      enum Offset
      {
        Magic = 0,
        Size = 4,
        Unknown1 = 8,
        Padding = 10,
        Unknown2 = 12,
        VertexCount = 16,
        DataOffset = 20
      };
    };

    struct VTXHeader
    {
      VTXHeader(std::vector<uint8_t>& data, uint32_t offset = 0);

      inline uint32_t size()
      {
        return m_size;
      }

      inline uint32_t offset()
      {
        return m_header_offset;
      }

      inline uint32_t begin()
      {
        //  First offset relative to the header starts the data
        return m_header_offset + m_offsets[0];
      }

      inline uint32_t end()
      {
        //  Go until the end of this section
        return m_header_offset + m_size;
      }
    private:
      std::string m_magic;
      uint32_t m_size;
      uint32_t m_array_fmt_offset;
      uint32_t m_offsets[13];

      uint32_t m_header_offset;

      enum Offset
      {
        Magic = 0,
        Size = 4,
        ArrayFormat = 8,
        Offsets = 12
      };
    };

    struct EVPHeader
    {
      EVPHeader(std::vector<uint8_t>& data, uint32_t offset = 0);

      inline uint32_t size()
      {
        return m_size;
      }

      inline uint32_t offset()
      {
        return m_header_offset;
      }

      inline uint32_t begin()
      {
        //  Start at the 4th offset which is matrix data
        return m_header_offset + m_offsets[3];
      }

      inline uint32_t end()
      {
        //  Go to the end of this section
        return m_header_offset + m_size;
      }
    private:
      std::string m_magic;
      uint32_t m_size;
      uint16_t m_count;
      uint16_t m_pad;
      uint32_t m_offsets[4];

      uint32_t m_header_offset;

      enum Offset
      {
        Magic = 0,
        Size = 4,
        Count = 8,
        Padding = 10,
        Offsets = 12
      };
    };

    struct DRWHeader
    {
      DRWHeader(std::vector<uint8_t>& data, uint32_t offset = 0);

      inline uint32_t size()
      {
        return m_size;
      }

      inline uint32_t offset()
      {
        return m_header_offset;
      }

      inline uint32_t begin()
      {
        return m_header_offset + m_data_offset;
      }

      inline uint32_t end()
      {
        return m_header_offset + m_size;
      }
    private:
      std::string m_magic;
      uint32_t m_size;
      uint16_t m_count;
      uint16_t m_pad;
      uint32_t m_offset_weighted;
      uint32_t m_data_offset;

      uint32_t m_header_offset;

      enum Offset
      {
        Magic = 0,
        Size = 4,
        Count = 8,
        Padding = 10,
        Weight = 12,
        Data = 16
      };
    };

    struct JNTHeader
    {
      JNTHeader(std::vector<uint8_t>& data, uint32_t offset = 0);

      inline uint32_t size()
      {
        return m_size;
      }

      inline uint32_t offset()
      {
        return m_header_offset;
      }

      inline uint32_t begin()
      {
        //  Start at joint data
        return m_header_offset + m_joint_offset;
      }

      inline uint32_t end()
      {
        //  Only corrupt the joint data (each joint is 0x40 bytes)
        return m_header_offset + m_joint_offset + (m_count * 0x40);
      }
    private:
      std::string m_magic;
      uint32_t m_size;
      uint16_t m_count;
      uint16_t m_pad;
      uint32_t m_joint_offset;
      uint32_t m_unknown;
      uint32_t m_string_table;

      uint32_t m_header_offset;

      enum Offset
      {
        Magic = 0,
        Size = 4,
        Count = 8,
        Padding = 10,
        Joint = 12,
        Unknown = 16,
        StringTable = 20
      };
    };

    struct SHPHeader
    {
      SHPHeader(std::vector<uint8_t>& data, uint32_t offset = 0);

      inline uint32_t size()
      {
        return m_size;
      }

      inline uint32_t offset()
      {
        return m_header_offset;
      }

      inline uint32_t begin()
      {
        return m_header_offset + m_data_offset;
      }

      inline uint32_t end()
      {
        //  Don't corrupt packet data, only the primitive and matrix data
        return m_header_offset + m_packet_data_offset;
      }
    private:
      std::string m_magic;
      uint32_t m_size;
      uint16_t m_count;
      uint16_t m_pad;
      uint32_t m_batch_offset;
      uint32_t unknown;
      uint32_t zero;
      uint32_t m_attrib_offset;
      uint32_t m_matrix_offset;
      uint32_t m_data_offset;
      uint32_t m_matrix_data_offset;
      uint32_t m_packet_data_offset;

      uint32_t m_header_offset;

      enum Offset
      {
        Magic = 0,
        Size = 4,
        Count = 8,
        Padding = 10,
        Batch = 12,
        Unknown = 16,
        Zero = 20,
        AttributesOffset = 24,
        MatrixOffset = 28,
        DataOffset = 32,
        MatrixData = 36,
        PacketData = 40
      };
    };

    struct MATHeader
    {
      MATHeader(std::vector<uint8_t>& data, uint32_t offset = 0);

      inline uint32_t size()
      {
        return m_size;
      }

      inline uint32_t offset()
      {
        return m_header_offset;
      }

      inline uint32_t begin()
      {
        //  Start corrupting at first offset
        return m_header_offset + m_offsets[0];
      }

      inline uint32_t end()
      {
        //  Go until end of this section
        return m_header_offset + m_size;
      }
    private:
      std::string m_magic;
      uint32_t m_size;
      uint16_t m_count;
      uint16_t m_pad;
      uint32_t m_offsets[30];

      uint32_t m_header_offset;

      enum Offset
      {
        Magic = 0,
        Size = 4,
        Count = 8,
        Padding = 10,
        Offsets = 12
      };
    };

    struct TEXHeader
    {
      TEXHeader(std::vector<uint8_t>& data, uint32_t offset = 0);

      inline uint32_t size()
      {
        return m_size;
      }

      inline uint32_t offset()
      {
        return m_header_offset;
      }

      inline uint32_t begin()
      {
        //  Start at tex offset and skip each BTI header (0x20 bytes each)
        return m_header_offset + m_tex_offset + (m_num_images * 0x20);
      }

      inline uint32_t end()
      {
        //  Go until end of this section
        return m_header_offset + m_size;
      }
    private:
      std::string m_magic;
      uint32_t m_size;
      uint16_t m_num_images;
      uint16_t m_unknown;
      uint32_t m_tex_offset;

      uint32_t m_header_offset;

      //std::vector<bti::BTIFile> m_tex_files;

      enum Offset
      {
        Magic = 0,
        Size = 4,
        Images = 8,
        Unknown = 10,
        TextureOffset = 12
      };
    };

    struct Header
    {
      Header(std::vector<uint8_t>& data, uint32_t offset = 0);

      inline uint32_t size()
      {
        return m_size;
      }

      inline uint32_t inf_begin()
      {
        return m_inf->begin();
      }

      inline uint32_t inf_end()
      {
        return m_inf->end();
      }

      inline uint32_t vtx_begin()
      {
        return m_vtx->begin();
      }

      inline uint32_t vtx_end()
      {
        return m_vtx->end();
      }

      inline uint32_t evp_begin()
      {
        return m_evp->begin();
      }

      inline uint32_t evp_end()
      {
        return m_evp->end();
      }

      inline uint32_t drw_begin()
      {
        return m_drw->begin();
      }

      inline uint32_t drw_end()
      {
        return m_drw->end();
      }

      inline uint32_t jnt_begin()
      {
        return m_jnt->begin();
      }

      inline uint32_t jnt_end()
      {
        return m_jnt->end();
      }

      inline uint32_t shp_begin()
      {
        return m_shp->begin();
      }

      inline uint32_t shp_end()
      {
        return m_shp->end();
      }

      inline uint32_t mat_begin()
      {
        return m_mat->begin();
      }

      inline uint32_t mat_end()
      {
        return m_mat->end();
      }

      inline uint32_t tex_begin()
      {
        return m_tex->begin();
      }

      inline uint32_t tex_end()
      {
        return m_tex->end();
      }
    private:
      std::string m_magic;
      uint32_t m_size;

      std::unique_ptr<INFHeader> m_inf;
      std::unique_ptr<VTXHeader> m_vtx;
      std::unique_ptr<EVPHeader> m_evp;
      std::unique_ptr<DRWHeader> m_drw;
      std::unique_ptr<JNTHeader> m_jnt;
      std::unique_ptr<SHPHeader> m_shp;
      std::unique_ptr<MATHeader> m_mat;
      std::unique_ptr<TEXHeader> m_tex;

      enum Offset
      {
        Magic = 0,
        Size = 8
      };
    };

    struct BMTHeader
    {
      BMTHeader(std::vector<uint8_t>& data, uint32_t offset = 0);

      inline uint32_t size()
      {
        return m_size;
      }

      inline uint32_t mat_begin()
      {
        return m_mat->begin();
      }

      inline uint32_t mat_end()
      {
        return m_mat->end();
      }
    private:
      std::string m_magic;
      uint32_t m_size;

      std::unique_ptr<MATHeader> m_mat;

      enum Offset
      {
        Magic = 0,
        Size = 8
      };
    };
  }

  class BMDFile
  {
  public:
    static void corrupt(std::vector<uint8_t>& data, std::vector<std::string>& args);

    static inline bool match(std::vector<uint8_t>& data)
    {
      return util::read(data, 0, 8) == "J3D2bmt3";
    }
  private:
  };

  class BMTFile
  {
  public:
    static void corrupt(std::vector<uint8_t>& data, std::vector<std::string>& args);

    static inline bool match(std::vector<uint8_t>& data)
    {
      return util::read(data, 0, 8) == "J3D2bmd3";
    }
  private:
  };
}

#endif
