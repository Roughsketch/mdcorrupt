#include "bmd_file.h"

namespace bmd
{
  namespace detail
  {
    INFHeader::INFHeader(std::vector<uint8_t>& data, uint32_t offset)
    {
      m_magic = util::read(data, offset + INFHeader::Offset::Magic, 4);
      m_size = util::read_big<uint32_t>(data, offset + INFHeader::Offset::Size);
      m_unknown1 = util::read_big<uint16_t>(data, offset + INFHeader::Offset::Unknown1);
      m_pad = util::read_big<uint16_t>(data, offset + INFHeader::Offset::Padding);
      m_unknown2 = util::read_big<uint32_t>(data, offset + INFHeader::Offset::Unknown2);
      m_vertex_count = util::read_big<uint32_t>(data, offset + INFHeader::Offset::VertexCount);
      m_data_offset = util::read_big<uint32_t>(data, offset + INFHeader::Offset::DataOffset);

      m_header_offset = offset;
    }

    VTXHeader::VTXHeader(std::vector<uint8_t>& data, uint32_t offset)
    {
      m_magic = util::read(data, offset + VTXHeader::Offset::Magic, 4);
      m_size = util::read_big<uint32_t>(data, offset + VTXHeader::Offset::Size);
      m_array_fmt_offset = util::read_big<uint32_t>(data, offset + VTXHeader::Offset::ArrayFormat);

      //  Read in each of the 13 offsets
      for (int i = 0; i < 13; i++)
      {
        m_offsets[i] = util::read_big<uint32_t>(data, offset + VTXHeader::Offset::Offsets + (i * 4));
      }

      m_header_offset = offset;
    }

    EVPHeader::EVPHeader(std::vector<uint8_t>& data, uint32_t offset)
    {
      m_magic = util::read(data, offset + EVPHeader::Offset::Magic, 4);
      m_size = util::read_big<uint32_t>(data, offset + EVPHeader::Offset::Size);
      m_count = util::read_big<uint16_t>(data, offset + EVPHeader::Offset::Count);
      m_pad = util::read_big<uint16_t>(data, offset + EVPHeader::Offset::Padding);

      //  Read in each of the 4 offsets
      for (int i = 0; i < 4; i++)
      {
        m_offsets[i] = util::read_big<uint32_t>(data, offset + EVPHeader::Offset::Offsets + (i * 4));
      }

      m_header_offset = offset;
    }

    DRWHeader::DRWHeader(std::vector<uint8_t>& data, uint32_t offset)
    {
      m_magic = util::read(data, offset + DRWHeader::Offset::Magic, 4);
      m_size = util::read_big<uint32_t>(data, offset + DRWHeader::Offset::Size);
      m_count = util::read_big<uint16_t>(data, offset + DRWHeader::Offset::Count);
      m_pad = util::read_big<uint16_t>(data, offset + DRWHeader::Offset::Padding);
      m_offset_weighted = util::read_big<uint32_t>(data, offset + DRWHeader::Offset::Weight);
      m_data_offset = util::read_big<uint32_t>(data, offset + DRWHeader::Offset::Data);

      m_header_offset = offset;
    }

    JNTHeader::JNTHeader(std::vector<uint8_t>& data, uint32_t offset)
    {
      m_magic = util::read(data, offset + JNTHeader::Offset::Magic, 4);
      m_size = util::read_big<uint32_t>(data, offset + JNTHeader::Offset::Size);
      m_count = util::read_big<uint16_t>(data, offset + JNTHeader::Offset::Count);
      m_pad = util::read_big<uint16_t>(data, offset + JNTHeader::Offset::Padding);
      m_joint_offset = util::read_big<uint32_t>(data, offset + JNTHeader::Offset::Joint);
      m_unknown = util::read_big<uint32_t>(data, offset + JNTHeader::Offset::Unknown);
      m_string_table = util::read_big<uint32_t>(data, offset + JNTHeader::Offset::StringTable);

      m_header_offset = offset;
    }

    SHPHeader::SHPHeader(std::vector<uint8_t>& data, uint32_t offset)
    {
      m_magic = util::read(data, offset + SHPHeader::Offset::Magic, 4);
      m_size = util::read_big<uint32_t>(data, offset + SHPHeader::Offset::Size);
      m_count = util::read_big<uint16_t>(data, offset + SHPHeader::Offset::Count);
      m_pad = util::read_big<uint16_t>(data, offset + SHPHeader::Offset::Padding);
      m_batch_offset = util::read_big<uint32_t>(data, offset + SHPHeader::Offset::Batch);
      unknown = util::read_big<uint32_t>(data, offset + SHPHeader::Offset::Unknown);
      zero = util::read_big<uint32_t>(data, offset + SHPHeader::Offset::Zero);
      m_attrib_offset = util::read_big<uint32_t>(data, offset + SHPHeader::Offset::AttributesOffset);
      m_matrix_offset = util::read_big<uint32_t>(data, offset + SHPHeader::Offset::MatrixOffset);
      m_data_offset = util::read_big<uint32_t>(data, offset + SHPHeader::Offset::DataOffset);
      m_matrix_data_offset = util::read_big<uint32_t>(data, offset + SHPHeader::Offset::MatrixData);
      m_packet_data_offset = util::read_big<uint32_t>(data, offset + SHPHeader::Offset::PacketData);

      m_header_offset = offset;
    }

    MATHeader::MATHeader(std::vector<uint8_t>& data, uint32_t offset)
    {
      m_magic = util::read(data, offset + MATHeader::Offset::Magic, 4);
      m_size = util::read_big<uint32_t>(data, offset + MATHeader::Offset::Size);
      m_count = util::read_big<uint16_t>(data, offset + MATHeader::Offset::Count);
      m_pad = util::read_big<uint16_t>(data, offset + MATHeader::Offset::Padding);


      /*
        MAT2 blocks can be missing sections and are smaller, but for now
        we will just handle it like it was a MAT3 block and check if the
        offset data is plausible based on these criteria:
          Offset[i] is greater than Offset[i - 1]
          Offset[i] is smaller than MAT3.Size
      */
      for (int i = 0; i < 30; i++)
      {
        m_offsets[i] = util::read_big<uint32_t>(data, offset + MATHeader::Offset::Offsets + (i * 4));
      }

      m_header_offset = offset;
    }

    TEXHeader::TEXHeader(std::vector<uint8_t>& data, uint32_t offset)
    {
      m_magic = util::read(data, offset + TEXHeader::Offset::Magic, 4);
      m_size = util::read_big<uint32_t>(data, offset + TEXHeader::Offset::Size);
      m_num_images = util::read_big<uint16_t>(data, offset + TEXHeader::Offset::Images);
      m_unknown = util::read_big<uint16_t>(data, offset + TEXHeader::Offset::Unknown);
      m_tex_offset = util::read_big<uint32_t>(data, offset + TEXHeader::Offset::TextureOffset);

      //  For each image read in the BTI header
      for (int i = 0; i < m_num_images; i++)
      {

      }

      m_header_offset = offset;
    }

    Header::Header(std::vector<uint8_t>& data, uint32_t offset)
    {
      m_magic = util::read(data, offset + Header::Offset::Magic, 8);
      m_size = util::read<uint32_t>(data, offset + Header::Offset::Size);

      offset += 0x20; //  So far seems to always be 0x20?

      m_inf = std::make_unique<INFHeader>(data, offset);
      offset += m_inf->size();

      m_vtx = std::make_unique<VTXHeader>(data, offset);
      offset += m_vtx->size();

      m_evp = std::make_unique<EVPHeader>(data, offset);
      offset += m_evp->size();

      m_drw = std::make_unique<DRWHeader>(data, offset);
      offset += m_drw->size();

      m_jnt = std::make_unique<JNTHeader>(data, offset);
      offset += m_jnt->size();

      m_shp = std::make_unique<SHPHeader>(data, offset);
      offset += m_shp->size();

      m_mat = std::make_unique<MATHeader>(data, offset);
      offset += m_mat->size();

      m_tex = std::make_unique<TEXHeader>(data, offset);
    }

    BMTHeader::BMTHeader(std::vector<uint8_t>& data, uint32_t offset)
    {
      m_magic = util::read(data, offset + BMTHeader::Offset::Magic, 8);
      m_size = util::read<uint32_t>(data, offset + BMTHeader::Offset::Size);

      offset += 0x20; //  So far seems to always be 0x20?

      m_mat = std::make_unique<MATHeader>(data, offset);
    }
  }

  void BMDFile::corrupt(std::vector<uint8_t>& data, std::vector<std::string>& args)
  {
    auto header = std::make_unique<detail::Header>(data);

    //  Get the sections into their own vectors
    std::vector<uint8_t> inf(data.begin() + header->inf_begin(), data.begin() + header->inf_end());
    NintendoFile::corrupt(inf, args);
    std::copy(inf.begin(), inf.end(), data.begin() + header->inf_begin());

    std::vector<uint8_t> vtx(data.begin() + header->vtx_begin(), data.begin() + header->vtx_end());
    NintendoFile::corrupt(vtx, args);
    std::copy(vtx.begin(), vtx.end(), data.begin() + header->vtx_begin());

    std::vector<uint8_t> evp(data.begin() + header->evp_begin(), data.begin() + header->evp_end());
    NintendoFile::corrupt(evp, args);
    std::copy(evp.begin(), evp.end(), data.begin() + header->evp_begin());

    std::vector<uint8_t> drw(data.begin() + header->drw_begin(), data.begin() + header->drw_end());
    NintendoFile::corrupt(drw, args);
    std::copy(drw.begin(), drw.end(), data.begin() + header->drw_begin());

    std::vector<uint8_t> jnt(data.begin() + header->jnt_begin(), data.begin() + header->jnt_end());
    NintendoFile::corrupt(jnt, args);
    std::copy(jnt.begin(), jnt.end(), data.begin() + header->jnt_begin());

    std::vector<uint8_t> shp(data.begin() + header->shp_begin(), data.begin() + header->shp_end());
    NintendoFile::corrupt(shp, args);
    std::copy(shp.begin(), shp.end(), data.begin() + header->shp_begin());

    std::vector<uint8_t> mat(data.begin() + header->mat_begin(), data.begin() + header->mat_end());
    NintendoFile::corrupt(mat, args);
    std::copy(mat.begin(), mat.end(), data.begin() + header->mat_begin());

    std::vector<uint8_t> tex(data.begin() + header->tex_begin(), data.begin() + header->tex_end());
    NintendoFile::corrupt(tex, args);
    std::copy(tex.begin(), tex.end(), data.begin() + header->tex_begin());
  }

  void BMTFile::corrupt(std::vector<uint8_t>& data, std::vector<std::string>& args)
  {
    auto header = std::make_unique<detail::BMTHeader>(data, 0);

    std::vector<uint8_t> mat(data.begin() + header->mat_begin(), data.begin() + header->mat_end());
    NintendoFile::corrupt(mat, args);
    std::copy(mat.begin(), mat.end(), data.begin() + header->mat_begin());
  }
}