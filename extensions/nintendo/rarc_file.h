#ifndef _NINTENDO_RARC_H
#define _NINTENDO_RARC_H

#include "nintendo.h"
#include <map>

namespace rarc
{
  namespace detail
  {

    struct FileEntry
    {
      FileEntry();
      FileEntry(std::vector<uint8_t>& data, uint32_t offset = 0);

      inline uint32_t offset()
      {
        return data_offset;
      }

      inline uint32_t name_offset()
      {
        return string_offset;
      }

      inline uint16_t id()
      {
        return m_id;
      }

      inline bool is_dir()
      {
        return m_id == 0xFFFF;
      }

      inline bool is_file()
      {
        return m_id != 0xFFFF;
      }

      inline uint32_t size()
      {
        return data_size;
      }

      inline std::string path()
      {
        return m_path;
      }

      inline void set_path(std::string value)
      {
        m_path = value;
      }

      inline std::string name()
      {
        return m_name;
      }

      inline void set_name(std::string value)
      {
        m_name = value;
      }

    private:
      enum FileEntryOffset
      {
        ID,
        Unknown1 = 2,
        Unknown2 = 4,
        StringTableOffset = 6,
        DataOffset = 8,
        DataSize = 12,
        Zero = 16
      };

      uint16_t m_id;
      uint16_t unknown1;
      uint16_t unknown2;
      uint16_t string_offset;
      uint32_t data_offset;
      uint32_t data_size;
      uint32_t zero;

      std::string m_path;
      std::string m_name;
    };

    struct Header
    {
      Header(std::vector<uint8_t>& data, uint32_t offset = 0);

      bool valid()
      {
        return magic == "RARC";
      }

      inline uint32_t node_count()
      {
        return num_nodes;
      }

      inline uint32_t file_count()
      {
        return num_files;
      }

      inline uint32_t file_table()
      {
        return file_entries_offset + 0x20;
      }

      inline uint32_t string_table()
      {
        return str_table_offset + 0x20;
      }

      inline uint32_t start()
      {
        return data_start + 0x20;
      }

    private:
      enum HeaderOffset
      {
        Magic,
        Size = 4,
        Unknown1 = 8,
        DataStart = 12,
        Unknown2 = 16,
        Nodes = 32,
        Unknown3 = 36,
        TotalFiles = 40,
        FileEntryOffset = 44,
        Unknown4 = 48,
        StringTableOffset = 52,
        Unknown5 = 56
      };

      std::string magic;
      uint32_t size;
      uint32_t unknown1;
      uint32_t data_start;
      uint8_t unknown2[16];
      uint32_t num_nodes;
      uint32_t unknown3;
      uint32_t num_files;
      uint32_t file_entries_offset;
      uint32_t unknown4;
      uint32_t str_table_offset;
      uint64_t unknown5;
    };
    struct Node
    {
      Node(std::vector<uint8_t>& data, std::unique_ptr<detail::Header>& header, uint32_t offset = 0, std::string parent = "./");

      inline uint32_t count()
      {
        return file_entries;
      }

      inline uint32_t name_offset()
      {
        return string_offset;
      }

      inline uint32_t offset()
      {
        return file_offset;
      }

      inline std::map<std::string, detail::FileEntry> files()
      {
        return m_files;
      }

      inline std::vector<detail::Node> nodes()
      {
        return subnodes;
      }

    private:
      enum NodeOffset
      {
        Type,
        StringTableOffset = 4,
        Unknown = 8,
        FileEntries = 10,
        FileStartOffset = 12
      };

      std::string type;
      uint32_t string_offset;
      uint16_t unknown;
      uint16_t file_entries;
      uint32_t file_offset;

      std::string name;
      std::map<std::string, detail::FileEntry> m_files;
      std::vector<detail::Node> subnodes;
    };
  }

  class RARCFile
  {
  public:
    RARCFile(std::vector<uint8_t>& data, std::vector <std::string>& args);

    std::vector<uint8_t> corrupt();
    void save(std::string filename);

  private:
    std::unique_ptr<CorruptionInfo> info;
    std::unique_ptr<detail::Header> header;

    std::vector<detail::Node> nodes;
    std::map<std::string, detail::FileEntry> entries;
    std::vector<uint8_t> data;
    std::vector<std::string> arguments;


    std::map<std::string, detail::FileEntry> compact(detail::Node, std::map<std::string, detail::FileEntry> files = std::map<std::string, detail::FileEntry>());
  };
};

#endif
