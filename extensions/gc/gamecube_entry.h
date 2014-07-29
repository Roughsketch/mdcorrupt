#ifndef _GAMECUBE_ENTRY_H
#define _GAMECUBE_ENTRY_H

#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <fstream>

#include "helpers.h"
#include "gamecube_except.h"

class GamecubeEntry
{
public:
  static const uint32_t DirectorySize = 0x0C; // Size of directory entries

  GamecubeEntry();
  GamecubeEntry(const GamecubeEntry& obj);
  GamecubeEntry(std::vector<uint8_t>& data, uint32_t offset = 0, uint32_t st_offset = 0);
  ~GamecubeEntry();

  bool operator==(const GamecubeEntry& obj) const;

  void initialize(std::vector<uint8_t>& data, uint32_t offset = 0, uint32_t st_offset = 0);
  std::vector<uint8_t> contents(std::fstream& source);
  GamecubeEntry get(std::string path);
  void write(std::fstream& source, std::vector<uint8_t> data);

  bool is_dir();
  bool is_file();
  uint32_t address();

  std::string name();
  uint32_t filesize();
  uint32_t parent();
  uint32_t parent_id();
  uint32_t id();
  uint32_t payload();

  void setid(uint32_t id);
  void setparent(uint32_t id);

  void sort();
  void print(std::vector<GamecubeEntry>& dir, uint32_t tabs = 0);

  bool is_yaz0();

private:
  bool m_directory;       //  Is directory
  std::string m_label;    //  File or directory label
  uint32_t m_offset;      //  Parent offset if dir, file offset if file
  uint32_t m_list_offset; //  The offset into the FST
  /*
    num_entries (root)
    next_offset (dir)
    file_length (file)
  */
  uint32_t m_payload;
  uint32_t m_parent_index;
  uint32_t m_entry_index;

  void yaz0_decode(uint8_t* src, uint8_t* dst, int uncompressedSize);
};


inline void GamecubeEntry::write(std::fstream& source, std::vector<uint8_t> data)
{
  if (this->filesize() != data.size())
  {
    throw InvalidFileSize();
  }

  auto origin = source.tellg();

  source.seekg(this->address());
  source.write(reinterpret_cast<char *>(&data[0]), this->filesize());
  source.seekg(origin);
}

/*
returns true when the entry is a directory
*/
inline bool GamecubeEntry::is_dir()
{
  return m_directory;
}

/*
returns true when the entry is a file
*/
inline bool GamecubeEntry::is_file()
{
  return !m_directory;
}

inline uint32_t GamecubeEntry::address()
{
  return m_offset;
}

/*
returns the file name or directory label of the entry
*/
inline std::string GamecubeEntry::name()
{
  return m_label;
}

inline uint32_t GamecubeEntry::filesize()
{
  return this->is_file() ? this->m_payload : 0;
}

inline uint32_t GamecubeEntry::parent()
{
  return this->is_dir() ? this->m_payload : 0;
}

inline uint32_t GamecubeEntry::payload()
{
  return this->m_payload;
}

inline uint32_t GamecubeEntry::parent_id()
{
  return m_parent_index;
}

inline uint32_t GamecubeEntry::id()
{
  return m_entry_index;
}

inline void GamecubeEntry::setparent(uint32_t id)
{
  m_parent_index = id;
}

inline void GamecubeEntry::setid(uint32_t id)
{
  m_entry_index = id;
}

#endif
