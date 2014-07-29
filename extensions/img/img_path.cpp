#include "img_path.h"

using namespace IMGHelper;

Path::Path()
{

}

/*
  Constructor for a Path object.

  @param bytes - The raw bytes read from an img or bin file from the global path table
  @param offset - Offset of the bytes to use to construct the class. Defaults to 0.
*/
Path::Path(std::vector<uint8_t>& bytes, uint32_t offset)
{
  //  Grab identifier length
  uint8_t size = bytes[offset];

  //  Size is always 8 + identifier size + 1 byte padding if size is odd
  this->m_raw.resize(8 + size + ((size % 2) ? 1 : 0));

  //  Move the bytes into the class vector
  std::copy_n(bytes.begin() + offset, 8 + size + ((size % 2) ? 1 : 0), this->m_raw.begin());

  //  Set up variables
  this->m_location = read<uint32_t>(this->m_raw, 2);     //  2 = Location of Extent
  this->m_parent_index = read<uint16_t>(this->m_raw, 6); //  6 = Directory number of parent index
  this->m_identifier = read(this->m_raw, 8, size);       //  8 = Variable length identifier for path

  if (this->m_identifier[0] == 0)
  {
    this->m_identifier = ".";
  }
}

/*
  Simple copy constructor for the class

  @param obj - The object to copy into the new one
*/
Path::Path(Path& obj)
{
  this->m_identifier = obj.m_identifier;
  this->m_parent_index = obj.m_parent_index;
  this->m_location = obj.m_location;

  this->m_raw.assign(obj.m_raw.begin(), obj.m_raw.end());
  this->m_entries.assign(obj.m_entries.begin(), obj.m_entries.end());
}

Path::~Path() {} // Nothing to destruct manually

/*
  Populates the file entries from the path data

  @param img - fstream reference of an open file of the img the path belongs to.
*/
void Path::populate(std::fstream& img, uint32_t r_blocksize, uint32_t l_blocksize)
{
  //  Store original position so it can be restored later
  std::streampos fpos = img.tellg();

  //  Get the offset given the real blocksize
  uint32_t offset = this->location(r_blocksize);
  std::vector<uint8_t> raw(r_blocksize);
  uint8_t size = -1;

  //  Read the data into the raw vector
  img.seekg(offset + SectionHeaderSize, std::ios::beg);
  img.read(reinterpret_cast<char *>(&raw[0]), r_blocksize);

  size = raw[0];  //  First size is the first byte
  for (uint32_t i = 0; size != 0; size = raw[i])
  {
    //  Grab next entry given the size
    std::vector<uint8_t> entry(raw.begin() + i, raw.begin() + i + size);

    //  If the entry is a file then store it
    if (Entry::is_file(entry))
    {
      this->m_entries.push_back(Entry(entry, r_blocksize, l_blocksize));
    }
    i += size;  //  Move to next entry in the data
  }

  //  Restore starting file position
  img.seekg(fpos, std::ios::beg);
}

/*
  Accessor for the raw location value.

  @return The raw location not multiplied by the blocksize.
*/
uint32_t Path::location()
{
  return m_location;
}

/*
  Returns the real location of a path table given the image block size

  @param blocksize - The block size of the image

  @return The real location of the path table
*/
uint32_t Path::location(uint32_t blocksize)
{
  return m_location * blocksize;
}

/*
  @return The index in the global path table of this path's parent directory.
*/
uint16_t Path::parent_index()
{
  return m_parent_index - 1;
}

/*
  @return The identifier of the path
*/
std::string Path::identifier()
{
  return m_identifier;
}

/*
  @return The raw size of the path initializer in bytes
*/
uint16_t Path::size()
{
  return this->m_raw.size();
}

/*
  @return a vector of file entries under the path
*/
std::vector<Entry> Path::entries()
{
  return std::vector<Entry>(this->m_entries);
}