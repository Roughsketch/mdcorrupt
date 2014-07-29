#include "u8_file.h"

namespace u8
{
  namespace detail
  {
    FileEntry::FileEntry(std::vector<uint8_t>& data, uint32_t offset)
    {
      m_type_and_offset = util::read_big<uint32_t>(data, offset + FileEntry::Offset::Type);
      m_begin_or_index = util::read_big<uint32_t>(data, offset + FileEntry::Offset::Begin);
      m_size_or_index_first = util::read_big<uint32_t>(data, offset + FileEntry::Offset::Size);
    }

    FST::FST(std::vector<uint8_t>& data, uint32_t offset)
    {
      m_root = FileEntry(util::subset(data, offset, FileEntry::EntrySize));

      for (uint32_t i = 1; i < m_root.data_size(); i++)
      {
        m_entries.push_back(FileEntry(util::subset(data, offset + (i * FileEntry::EntrySize), FileEntry::EntrySize)));

        if (m_entries.back().is_file())
        {
          m_files++;
        }
      }
    }

    Header::Header(std::vector<uint8_t>& data, uint32_t offset)
    {
      m_magic = util::read_big<uint32_t>(data, offset + Header::Offset::Magic);
      m_node_offset = util::read_big<uint32_t>(data, offset + Header::Offset::NodeOffset);
      m_fst_size = util::read_big<uint32_t>(data, offset + Header::Offset::FSTSize);
      m_data_offset = util::read_big<uint32_t>(data, offset + Header::Offset::DataOffset);
    }
  }

  U8File::U8File(std::vector<uint8_t>& data, std::vector<std::string>& args)
  {
    info = std::make_unique<CorruptionInfo>(args);
    header = std::make_unique<detail::Header>(data);
    fst = std::make_unique<detail::FST>(data, header->node_offset());

    //std::cout << "Found " << fst->file_count() << " files." << std::endl;
  }

  std::vector<uint8_t> U8File::corrupt(std::vector<uint8_t>& data, std::vector<std::string>& args)
  {
    auto info = std::make_unique<CorruptionInfo>(args);
    auto header = std::make_unique<detail::Header>(data);
    auto fst = std::make_unique<detail::FST>(data, header->node_offset());

    for (auto& entry : fst->files())
    {
      std::vector<uint8_t> filedata = util::subset(data, entry.data_start(), entry.data_size());

      /*
      std::ofstream ofs(entry.name(), std::ios::binary);
      ofs.write(reinterpret_cast<char *>(&filedata[0]), entry.size());
      ofs.close();
      */

      //  If the file is Yaz0 compressed then ignore it since the sizes most likely won't match
      if (util::read(filedata, 0, 4) != "Yaz0")
      {
        //  Since RARC files are archives of Nintendo files, corrupt it with Nintendo file protection
        NintendoFile::start(filedata, args);

        //  Copy the corrupted bytes back into place
        //  NOTE: This does not account for differing sizes for Yaz0 compressed files!
        std::copy(filedata.begin(), filedata.begin() + entry.data_size(), data.begin() + entry.data_start());
      }
    }

    return data;
  }
}