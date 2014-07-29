#include "img_entry.h"

Entry::Entry()
{

}

Entry::Entry(std::vector<uint8_t>& data, uint32_t r_blocksize, uint32_t l_blocksize)
{
  m_size = Util::read<uint8_t>(data, EntryOffset::Size);
  m_extended_size = Util::read<uint8_t>(data, EntryOffset::Extended);
  m_location = Util::read<uint32_t>(data, EntryOffset::ExtentLocation);
  m_data_length = Util::read<uint32_t>(data, EntryOffset::DataLength);
  //date = std::make_unique<DateTime>(data, EntryOffset::Date);
  m_flags = Util::read<uint8_t>(data, EntryOffset::FileFlags);
  m_file_unit_size = Util::read<uint8_t>(data, EntryOffset::FileUnitSize);
  m_interleave_gap = Util::read<uint8_t>(data, EntryOffset::InterleaveGap);
  m_volume_sequence_number = Util::read<uint16_t>(data, EntryOffset::VolumeSequenceNumber);
  m_file_id_length = Util::read<uint8_t>(data, EntryOffset::FileIdentifierLength);
  m_identifier = Util::read(data, EntryOffset::FileIdentifier, m_file_id_length);

  if (m_identifier.length() == 1 && m_identifier[0] == 1)
  {
    m_identifier = "..";
  }
  else if (m_identifier.length() == 1 && m_identifier[0] == 0)
  {
    m_identifier = ".";
  }

  if (m_identifier.length() > 2 && m_identifier.substr(m_identifier.length() - 2) == ";1")
  {
    m_flags |= Attribute::Associated;

    m_extension = m_identifier.substr(m_identifier.find_last_of(".") + 1);
    m_extension = m_extension.substr(0, m_extension.length() - 2);
    m_identifier = m_identifier.substr(0, m_identifier.length() - 2);
  }
  else
  {
    m_flags |= Attribute::Directory;
  }

  m_real_block_size = r_blocksize;
  m_logical_block_size = l_blocksize;
}

Entry::~Entry() {}

/*
  Tells whether an entry is a directory

  @return True if the entry is a directory
*/
bool Entry::is_directory()
{
  return (m_flags & Attribute::Directory) != 0;
}

/*
  Tells whether an uninitialized entry is a directory

  @return True if the entry is a directory
*/
bool Entry::is_directory(std::vector<uint8_t>& entry)
{
  return !Entry::is_file(entry);
}

/*
  Tells whether an uninitialized entry is a file

  @return True if the entry is a file
*/
bool Entry::is_file(std::vector<uint8_t>& entry)
{
  //  Read in the file identifier length to know how long the identifier is
  uint8_t file_id_length = Util::read<uint8_t>(entry, EntryOffset::FileIdentifierLength);

  //  Read in the identifier
  std::string identifier = Util::read(entry, EntryOffset::FileIdentifier, file_id_length);

  //  Entry is a file if the entry ends with ;1 (at least for most cases)
  return (identifier.length() > 2 && identifier.substr(identifier.length() - 2) == ";1");
}

/*
  Return the location of the entry given an image blocksize

  @return the real location of the entry
*/
uint32_t Entry::location(uint32_t blocksize)
{
  return m_location * blocksize;
}

/*
  @return the size in bytes of the entry's data
*/
uint32_t Entry::size()
{
  return m_data_length;
}

/*
  @return the identifier of the entry
*/
std::string Entry::name()
{
  return m_identifier;
}

/*
  Reads the raw contents of a file on the image and returns it without sequence headers or junk data.
  The fstream will be placed back at the position it was in before the call.

  @param source - An open file stream to read the data from.

  @return raw contents of the file.
*/
std::vector<uint8_t> Entry::get(std::fstream& source)
{
  std::streampos fpos = source.tellg();
  std::vector<uint8_t> data(this->size());
  uint32_t dataread = 0;
  uint32_t junksize = this->m_real_block_size - this->m_logical_block_size - Entry::SectionHeaderSize;

  source.seekg(this->location(this->m_real_block_size) + SectionHeaderSize, std::ios::beg);

  while (dataread < this->size() - this->m_logical_block_size)
  {
    source.read(reinterpret_cast<char *>(&data[dataread]), this->m_logical_block_size);
    dataread += this->m_logical_block_size;
    source.seekg(junksize, std::ios::cur);
  }

  source.read(reinterpret_cast<char *>(&data[dataread]), this->size() - dataread);


  source.seekg(fpos, std::ios::beg);

  return data;
}

/*
  Write raw data to the location where the original file is stored on the img

  @param dest - Open file stream of the img to write to
  @param data - Raw data that matches the size of the file it will overwrite
*/
void Entry::write(std::fstream& dest, std::vector<uint8_t> data)
{
  //  Data to write does not match the entry size, do nothing
  if (data.size() != this->size())
  {
    return;
  }

  //  Store stream size so it can be returned to at the end
  std::streampos fpos = dest.tellg();
  //  Total bytes written to the file so far
  uint32_t written = 0;
  //  Amount of junk between sectors of the img
  uint32_t junksize = this->m_real_block_size - this->m_logical_block_size - SectionHeaderSize;

  //  Move to the file location
  dest.seekg(this->location(this->m_real_block_size) + SectionHeaderSize, std::ios::beg);

  //  While we haven't written the full amount
  while (written < data.size() - this->m_logical_block_size)
  {
    //  Write the max concurrent size possible to the disk
    dest.write(reinterpret_cast<char *>(&data[written]), this->m_logical_block_size);
    //  Increase amount written
    written += this->m_logical_block_size;
    //  Skip the junk section at the end of the sector
    dest.seekg(junksize, std::ios::cur);
  }

  //  Write any extra data to the file if it has an odd size
  dest.write(reinterpret_cast<char *>(&data[written]), data.size() - written);

  //  Restore original position
  dest.seekg(fpos, std::ios::beg);
}