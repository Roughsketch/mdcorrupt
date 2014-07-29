#include "gamecube_entry.h"

GamecubeEntry::GamecubeEntry()
{
  this->m_directory = false;
  this->m_label = "";
  this->m_offset = 0;
  this->m_list_offset = 0;
}

GamecubeEntry::GamecubeEntry(const GamecubeEntry& obj)
{
  this->m_directory = obj.m_directory;
  this->m_label = obj.m_label;
  this->m_offset = obj.m_offset;
  this->m_list_offset = obj.m_list_offset;
  this->m_entry_index = obj.m_entry_index;
  this->m_parent_index = obj.m_parent_index;
  this->m_payload = obj.m_payload;
}

/*
  Takes the entire FST as a vector of uint8_t with an offset to the entry to initialize

  @param data - The entire Gamecube disc FST structure
  @param offset - The offset to the entry needing to be initialized
*/
GamecubeEntry::GamecubeEntry(std::vector<uint8_t>& data, uint32_t offset, uint32_t st_offset)
{
  initialize(data, offset, st_offset);
}

GamecubeEntry::~GamecubeEntry()
{

}

bool GamecubeEntry::operator==(const GamecubeEntry& obj) const
{
  bool equal = false;

  if (&obj == this)
  {
    return true;
  }

  equal = this->m_directory == obj.m_directory;
  equal &= this->m_label == obj.m_label;
  equal &= this->m_offset == obj.m_offset;
  equal &= this->m_list_offset == obj.m_list_offset;
  equal &= this->m_entry_index == obj.m_entry_index;
  equal &= this->m_parent_index == obj.m_parent_index;
  equal &= this->m_payload == obj.m_payload;

  return equal;
}

/*
  Takes the entire FST as a vector of uint8_t with an offset to the entry to initialize

  @param data - The entire Gamecube disc FST structure
  @param offset - The offset to the entry needing to be initialized
  @param st_offset - Max offset found in root node (Do not pass manually)
*/
void GamecubeEntry::initialize(std::vector<uint8_t>& data, uint32_t offset, uint32_t st_offset)
{
  this->m_directory = data[offset] == 1;  //  Flag is 1 when directory or 0 when it's a file

  uint32_t name_offset = Util::read_big<uint32_t>(data, offset) & 0x00FFFFFF; //  Name offset is only 3 bytes
  this->m_offset = Util::read_big<uint32_t>(data, offset + 4);  //  file/parent offset
  this->m_payload = Util::read_big<uint32_t>(data, offset + 8); //  file length (file) / number of entries (root) / next offset (dir)
  this->m_list_offset = offset / GamecubeEntry::DirectorySize;
  this->m_entry_index = 0;
  this->m_parent_index = 0;

  //  If not root then get the name from the string table
  if (st_offset != 0) 
  {
    this->m_label = Util::read(data, st_offset + name_offset);
  }
  else
  {
    //  Name root .
    this->m_label = ".";
  }
}

std::vector<uint8_t> GamecubeEntry::contents(std::fstream& source)
{
  if (this->is_dir())
  {
    return std::vector<uint8_t>(0);
  }

  std::vector<uint8_t> content(this->filesize());

  auto origin = source.tellg();

  source.seekg(this->address());
  source.read(reinterpret_cast<char *>(&content[0]), this->filesize());
  
  source.seekg(origin);

  return content;
}

bool GamecubeEntry::is_yaz0()
{
  return false;
}

//src points to the yaz0 source data (to the "real" source data, not at the header!)
//dst points to a buffer uncompressedSize bytes large (you get uncompressedSize from
//the second 4 bytes in the Yaz0 header).
void GamecubeEntry::yaz0_decode(uint8_t* src, uint8_t* dst, int uncompressedSize)
{
  int srcPlace = 0; //  Current read position
  int dstPlace = 0; //  Current write position

  uint32_t validBitCount = 0; //number of valid bits left in "code" byte
  uint8_t currCodeByte;

  while (dstPlace < uncompressedSize)
  {
    //  Read new "code" byte if the current one is used up
    if (validBitCount == 0)
    {
      currCodeByte = src[srcPlace];
      ++srcPlace;
      validBitCount = 8;
    }

    if ((currCodeByte & 0x80) != 0)
    {
      //  Straight copy
      dst[dstPlace] = src[srcPlace];
      dstPlace++;
      srcPlace++;
    }
    else
    {
      //RLE part
      uint8_t byte1 = src[srcPlace];
      uint8_t byte2 = src[srcPlace + 1];

      srcPlace += 2;

      uint32_t dist = ((byte1 & 0xF) << 8) | byte2;
      uint32_t copySource = dstPlace - (dist + 1);
      uint32_t numBytes = byte1 >> 4;

      if (numBytes == 0)
      {
        numBytes = src[srcPlace] + 0x12;
        srcPlace++;
      }
      else
      {
        numBytes += 2;
      }

      //  Copy run
      for (uint32_t i = 0; i < numBytes; ++i)
      {
        dst[dstPlace] = dst[copySource];
        copySource++;
        dstPlace++;
      }
    }

    //  Use next bit from "code" byte
    currCodeByte <<= 1;
    validBitCount -= 1;
  }
}

void GamecubeEntry::sort()
{
}

void GamecubeEntry::print(std::vector<GamecubeEntry>& dir, uint32_t tabs)
{
}