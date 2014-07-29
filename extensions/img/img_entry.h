#ifndef _IMG_ENTRY_H
#define _IMG_ENTRY_H

#include <memory>
#include <vector>
#include <fstream>

#include "helpers.h"

namespace EntryOffset
{
  enum
  {
    Size = 0,
    Extended = 1,
    ExtentLocation = 2,
    DataLength = 10,
    Date = 18,
    FileFlags = 25,
    FileUnitSize = 26,
    InterleaveGap = 27,
    VolumeSequenceNumber = 28,
    FileIdentifierLength = 32,
    FileIdentifier = 33
  };
}

namespace Attribute
{
  enum
  {
    Hidden = 1,       //  Hidden from the user
    Directory = 2,    //  Directory
    Associated = 4,   //  Normal file
    Extended = 8,     //  Extended information
    Permission = 16,  //  
    Incomplete = 32   //  Incomplete listing
  };
};

class Entry
{
public:
  static const uint8_t SectionHeaderSize = 0x18;

  Entry();
  Entry(std::vector<uint8_t>& bytes, uint32_t r_blocksize, uint32_t l_blocksize);
  ~Entry();

  bool is_directory();
  static bool is_directory(std::vector<uint8_t>& entry);
  static bool is_file(std::vector<uint8_t>& entry);
  uint32_t location(uint32_t blocksize);
  uint32_t size();
  std::string name();

  std::vector<uint8_t> get(std::fstream& source);
  void write(std::fstream& dest, std::vector<uint8_t> data);
private:
  uint8_t m_size;
  uint8_t m_extended_size;
  uint32_t m_location;
  uint32_t m_data_length;

  //  Not important, don't want to copy ISO structure again
  //std::unique_ptr<DateTime> date;

  uint8_t m_flags;
  uint8_t m_file_unit_size;
  uint8_t m_interleave_gap;
  uint16_t m_volume_sequence_number;
  uint8_t m_file_id_length;
  std::string m_identifier;

  //  Extra
  std::string m_extension; // File extension if applicable
  uint32_t m_logical_block_size;
  uint32_t m_real_block_size;
};

struct EntryData
{

  EntryData(uint32_t offset, uint32_t size, uint32_t r_blocksize, uint32_t l_blocksize)
      : m_size(size),
        m_offset(offset),
        m_real_block_size(r_blocksize),
        m_logical_block_size(l_blocksize) 
  {};

  uint32_t offset()             { return m_offset; };
  uint32_t size()               { return m_size; };
  uint32_t logical_block_size() { return m_logical_block_size; };
  uint32_t real_block_size()    { return m_real_block_size; };

private:
  uint32_t m_offset;
  uint32_t m_size;
  uint32_t m_logical_block_size;
  uint32_t m_real_block_size;
};

#endif
