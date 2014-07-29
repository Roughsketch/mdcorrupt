#ifndef _U8_FILE_H
#define _U8_FILE_H

#include "nintendo.h"

namespace u8
{
  namespace detail
  {
    struct FileEntry
    {
      static const uint32_t EntrySize = 0x0C;

      FileEntry() : m_begin_or_index(0), m_type_and_offset(0), m_size_or_index_first(0) {};
      FileEntry(std::vector<uint8_t>& data, uint32_t offset = 0);

      inline bool is_file()
      {
        return type() == 0;
      }

      inline bool is_dir()
      {
        return type() == 1;
      }

      inline uint32_t type()
      {
        return (m_type_and_offset & 0xFF000000) >> 24;
      }

      inline uint32_t string_offset()
      {
        return m_type_and_offset & 0x00FFFFFF;
      }

      inline uint32_t data_start()
      {
        return m_begin_or_index;
      }

      inline uint32_t parent_index()
      {
        return m_begin_or_index;
      }

      inline uint32_t data_size()
      {
        return m_size_or_index_first;
      }

      inline uint32_t next_index()
      {
        return m_size_or_index_first;
      }

    private:
      uint32_t m_type_and_offset;     //  & 0xFF000000 determines whether it is a file (0) or directory (1)
                                      //  & 0x00FFFFFF is the offset into the FST for the filename
      uint32_t m_begin_or_index;      //  For files it is the beginning point of data.
                                      //  For directories it is the index of the parent directory
      uint32_t m_size_or_index_first; //  For files it is the size of the data.
                                      //  For directories it is the index of the first node that is not part of this directory

      enum Offset
      {
        Type = 0,
        Offset = 1,
        Begin = 4,
        Size = 8
      };
    };

    struct FST
    {
      FST(std::vector<uint8_t>& data, uint32_t offset = 0);

      inline uint32_t file_count()
      {
        return m_files;
      }

      inline std::vector<FileEntry> files()
      {
        std::vector<FileEntry> ret;

        for (auto& entry : m_entries)
        {
          if (entry.is_file())
          {
            ret.push_back(entry);
          }
        }

        return ret;
      }

    private:
      FileEntry m_root;
      std::vector<FileEntry> m_entries;

      uint32_t m_files;
    };

    struct Header
    {
      Header(std::vector<uint8_t>& data, uint32_t offset = 0);

      inline uint32_t node_offset()
      {
        return m_node_offset;
      }

      inline uint32_t fst_size()
      {
        return m_fst_size;
      }

      inline uint32_t data_offset()
      {
        return m_data_offset;
      }
    private:
      uint32_t m_magic;
      uint32_t m_node_offset;
      uint32_t m_fst_size;
      uint32_t m_data_offset;

      enum Offset
      {
        Magic = 0,
        NodeOffset = 4,
        FSTSize = 8,
        DataOffset = 12,
      };
    };
  }

  class U8File
  {
  public:
    U8File(std::vector<uint8_t>& data, std::vector<std::string>& args);

    static std::vector<uint8_t> corrupt(std::vector<uint8_t>& data, std::vector<std::string>& args);

    static inline bool match(std::vector<uint8_t>& data)
    {
      return util::read_big<uint32_t>(data) == 0x55AA382D;
    }
  private:
    std::unique_ptr<CorruptionInfo> info;
    std::unique_ptr<detail::Header> header;
    std::unique_ptr<detail::FST> fst;
  };
}

#endif
