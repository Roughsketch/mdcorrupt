#include "rarc_file.h"

namespace rarc
{
  namespace detail
  {
    FileEntry::FileEntry() { }
    FileEntry::FileEntry(std::vector<uint8_t>& data, uint32_t offset)
    {
      m_id = util::read_big<uint16_t>(data, offset + FileEntryOffset::ID);
      unknown1 = util::read_big<uint16_t>(data, offset + FileEntryOffset::Unknown1);
      unknown2 = util::read_big<uint16_t>(data, offset + FileEntryOffset::Unknown2);
      string_offset = util::read_big<uint16_t>(data, offset + FileEntryOffset::StringTableOffset);
      data_offset = util::read_big<uint32_t>(data, offset + FileEntryOffset::DataOffset);
      data_size = util::read_big<uint32_t>(data, offset + FileEntryOffset::DataSize);
      zero = util::read_big<uint32_t>(data, offset + FileEntryOffset::Zero);
    }

    Header::Header(std::vector<uint8_t>& data, uint32_t offset)
    {
      //  Set up header data
      magic = util::read(data, offset + HeaderOffset::Magic, 4);
      size = util::read_big<uint32_t>(data, offset + HeaderOffset::Size);
      unknown1 = util::read_big<uint32_t>(data, offset + HeaderOffset::Unknown1);
      data_start = util::read_big<uint32_t>(data, offset + HeaderOffset::DataStart);

      for (int i = 0; i < 16; i++)
      {
        unknown2[i] = data[offset + HeaderOffset::Unknown2 + i];
      }

      num_nodes = util::read_big<uint32_t>(data, offset + HeaderOffset::Nodes);
      unknown3 = util::read_big<uint32_t>(data, offset + HeaderOffset::Unknown3);
      num_files = util::read_big<uint32_t>(data, offset + HeaderOffset::TotalFiles);
      file_entries_offset = util::read_big<uint32_t>(data, offset + HeaderOffset::FileEntryOffset);
      unknown4 = util::read_big<uint32_t>(data, offset + HeaderOffset::Unknown4);
      str_table_offset = util::read_big<uint32_t>(data, offset + HeaderOffset::StringTableOffset);
      unknown5 = util::read_big<uint64_t>(data, offset + HeaderOffset::Unknown5);
    }

    Node::Node(std::vector<uint8_t>& data, std::unique_ptr<detail::Header>& header, uint32_t offset, std::string parent)
    {
      type = util::read(data, offset + NodeOffset::Type, 4);
      string_offset = util::read_big<uint32_t>(data, offset + NodeOffset::StringTableOffset);
      unknown = util::read_big<uint16_t>(data, offset + NodeOffset::Unknown);
      file_entries = util::read_big<uint16_t>(data, offset + NodeOffset::FileEntries);
      file_offset = util::read_big<uint32_t>(data, offset + NodeOffset::FileStartOffset);

      //  Read name from the string table
      name = util::read(data, header->string_table() + string_offset);

      uint32_t subdirs = 0;

      for (uint32_t i = 0; i < file_entries; i++)
      {
        //  Create FileEntry
        FileEntry entry = detail::FileEntry(data, (file_offset * 0x14) + header->file_table() + (i * 0x14));
        std::string entry_name = util::read(data, header->string_table() + entry.name_offset());
        std::string path = parent + name + "/" + entry_name;

        //  Only add if it's not a . or .. directory
        if (entry_name != "." && entry_name != "..")
        {
          //  Set full path for easier handling later
          entry.set_name(entry_name);
          entry.set_path(path);
          m_files[path] = entry;

          //  If this is a directory then recurse into that node
          if (m_files[path].id() == 0xFFFF)
          {
            subnodes.push_back(Node(data, header, offset + ++subdirs * 0x10, parent + name + "/"));
          }
        }
      }
    }
  }

  RARCFile::RARCFile(std::vector<uint8_t>& data, std::vector<std::string>& args)
  {
    info = std::make_unique<CorruptionInfo>(args);
    header = std::make_unique<detail::Header>(data);

    //  Push back all nodes
    for (uint32_t i = 0; i < header->node_count(); i++)
    {
      nodes.push_back(detail::Node(data, header, 0x40 + (i * 0x10)));
    }

    //  Compact all the file entry data into a single map for easier access
    for (auto& node : nodes)
    {
      auto comp = compact(node);
      entries.insert(comp.begin(), comp.end());
    }

    //  Create a copy of the file data to pull individual files from
    this->data.insert(this->data.end(), data.begin(), data.end());

    //  Save arguments 
    this->arguments = args;
  }

  std::map<std::string, detail::FileEntry> RARCFile::compact(detail::Node node, std::map<std::string, detail::FileEntry> files)
  {
    //  Recurse into deeper nodes
    for (auto& node : node.nodes())
    {
      auto other = compact(node, files);
      files.insert(other.begin(), other.end());
    }

    //  Add files to the file pool
    auto other = node.files();
    files.insert(other.begin(), other.end());

    return files;
  }

  std::vector<uint8_t> RARCFile::corrupt()
  {
    //  Commented section corrupts individual files passed on the command line
    //  which won't be used until the GUI has a better design
    /*
    for (auto& path : info->files())
    {
      if (entries.count(path) == 0)
      {
        //  File doesn't exist as it was given, print error and go to next iteration
        debug::cerr << "Entry doesn't exist: " << path << std::endl;
        continue;
      }

      detail::FileEntry entry = entries.at(path);
      uint32_t offset = header->start() + entry.offset();
      std::vector<uint8_t> filedata(data.begin() + offset, data.begin() + offset + entry.size());

      std::ofstream ofs(entry.name(), std::ios::binary);
      ofs.write(reinterpret_cast<char *>(&filedata[0]), entry.size());
      ofs.close();

      //  Since RARC files are archives of Nintendo files, corrupt it with Nintendo file protection
      NintendoFile::start(filedata, arguments);

      std::copy(filedata.begin(), filedata.begin() + entry.size(), data.begin() + offset);
    }
    */

    for (auto& e : entries)
    {
      detail::FileEntry entry = e.second;
      uint32_t offset = header->start() + entry.offset();

      if (data.size() < offset + entry.size())
      {
        continue;
      }

      std::vector<uint8_t> filedata(data.begin() + offset, data.begin() + offset + entry.size());

      //  If the file is Yay0 or Yaz0 compressed then ignore it since the sizes most likely won't match
      if (filedata.size() > 4 && util::read(filedata, 0, 4) != "Yaz0" && util::read(filedata, 0, 4) != "Yay0")
      {
        //  Since RARC files are archives of Nintendo files, corrupt it with Nintendo file protection
        NintendoFile::start(filedata, arguments);

        //  Copy the corrupted bytes back into place
        //  NOTE: This does not account for differing sizes for Yaz0 compressed files!
        std::copy(filedata.begin(), filedata.begin() + entry.size(), data.begin() + offset);
      }
    }

    return data;
  }

  void RARCFile::save(std::string filename)
  {
    std::ofstream ofs(filename, std::ios::binary);
    ofs.write(reinterpret_cast<char *>(&data[0]), data.size());
    ofs.close();
  }
}
