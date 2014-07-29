#ifndef _RARC_ARCHIVE_H
#define _RARC_ARCHIVE_H

/*
  The following code was adapted from the program rarcdump
  which was created by thakis. All the main work done by
  this section is thanks to him; all I did was make it so
  it worked smoothly with the rest of the project.
*/

#include <boost/filesystem.hpp>

#include <cstdint>
#include <iostream>
#include <algorithm>

namespace rarc
{
  struct RarcHeader
  {
    char type[4]; //'RARC'
    uint32_t size; //size of the file
    uint32_t unknown;
    uint32_t dataStartOffset; //where does the actual data start?
    uint32_t unknown2[4];

    uint32_t numNodes;
    uint32_t unknown3[2];
    uint32_t fileEntriesOffset;
    uint32_t unknown4;
    uint32_t stringTableOffset; //where is the string table stored?
    uint32_t unknown5[2];
  };

  struct Node
  {
    char type[4];
    uint32_t filenameOffset; //directory name, offset into string table
    uint16_t unknown;
    uint16_t numFileEntries; //how many files belong to this node?
    uint32_t firstFileEntryOffset;
  };

  struct FileEntry
  {
    uint16_t id; //file id. If this is 0xFFFF, then this entry is a subdirectory link
    uint16_t unknown;
    uint16_t unknown2;
    uint16_t filenameOffset; //file/subdir name, offset into string table
    uint32_t dataOffset; //offset to file data (for subdirs: index of Node representing the subdir)
    uint32_t dataSize; //size of data
    uint32_t zero; //seems to be always '0'
  };

  namespace detail
  {
    inline void toWORD(uint16_t& w)
    {
      uint8_t w1 = w & 0xFF;
      uint8_t w2 = w >> 8;
      w = (w1 << 8) | w2;
    }

    inline void toDWORD(uint32_t& d)
    {
      uint8_t w1 = d & 0xFF;
      uint8_t w2 = (d >> 8) & 0xFF;
      uint8_t w3 = (d >> 16) & 0xFF;
      uint8_t w4 = d >> 24;
      d = (w1 << 24) | (w2 << 16) | (w3 << 8) | w4;
    }

    std::string getString(int pos, FILE* f);
    Node getNode(int i, FILE* f);
    FileEntry getFileEntry(int i, const RarcHeader& h, FILE* f);
    void dumpNode(const Node& n, const RarcHeader& h, FILE* f);
    void readFile(FILE* f);
  }

  void extract(std::string file);
  void compress(std::string path);
}

#endif
