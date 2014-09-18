#include "img.h"

IMG::IMG()
{

}

IMG::IMG(std::string filename, bool has_headers)
{
  static std::vector<uint8_t> section_header{
    0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00
  };

  this->has_headers = has_headers;

  std::fstream img(filename, std::ios::in | std::ios::binary);
  std::vector<uint8_t> raw;
  std::streamoff size = 0;

  if (!img.good())
  {
    throw IMGException("Could not open.");
  }

  img.seekg(0, std::ios::end);
  size = img.tellg();
  img.seekg(0, std::ios::beg);

  if (has_headers)
  {
    //  Read in $8000 bytes of data to check for valid img file and get real block size
    raw.resize(0x8000);
    img.read(reinterpret_cast<char *>(&raw[0]), 0x8000);

    //  If the image doesn't start with the sequence header then it's not valid
    if (!std::equal(section_header.begin(), section_header.end(), raw.begin()))
    {
      img.close();
      throw IMGException("Image does not start with valid sequence header.");
    }

    //  Find second section header
    auto second_header = std::search(raw.begin() + 1, raw.end(), section_header.begin(), section_header.end());

    if (second_header == raw.end())
    {
      img.close();
      throw IMGException("Image does not have a valid sequence header layout.");
    }

    //  Valid sequence found, difference between them is the real block size
    this->real_block_size = second_header - raw.begin();

    //  Block size must be a valid multiple of the img size, otherwise it is invalid
    if (size % this->real_block_size != 0)
    {
      img.close();
      throw IMGException("File size does not match block size.");
    }
  }
  else
  {
    this->real_block_size = 0x800;  //  Default ISO block size;
  }

  //  Move to start of volume header
  img.seekg(IMG::get_offset(img), std::ios::beg);

  //  Clear contents and prepare to read the primary volume header
  raw.clear();
  raw.resize(this->real_block_size);

  img.read(reinterpret_cast<char *>(&raw[0]), this->real_block_size);

  //  Header must start with a byte value of 1 followed by the string "CD001"
  if (raw[0] != 1 || std::string(raw.begin() + 1, raw.begin() + 6) != "CD001")
  {
    img.close();
    throw IMGException("Header is not valid.");
  }

  this->block_size = util::read<uint16_t>(ref(raw), 0x80); //  Read fake block size
  this->path_table_size = util::read<uint32_t>(ref(raw), 0x84); //  Read path table size
  this->path_table_location = util::read<uint32_t>(ref(raw), 0x8C); //  Read path table location (LSB)

  //  Clear contents and prepare to read the global path table
  raw.clear();
  raw.resize(this->path_table_size);

  if (has_headers == true)
  {
    img.seekg(this->path_table_location * this->real_block_size + SectionHeaderSize, std::ios::beg);
  }
  else
  {
    std::cout << std::hex << this->path_table_location * this->block_size << std::dec << std::endl;
    std::cout << std::hex << this->path_table_location  << std::dec << std::endl;
    std::cout << std::hex << this->block_size << std::dec << std::endl;
    img.seekg(this->path_table_location * this->block_size, std::ios::beg);
  }

  img.read(reinterpret_cast<char *>(&raw[0]), this->path_table_size);

  //  Iterate over the path table and make entries for each path
  for (auto it = raw.begin(); it < raw.end(); it += this->root.back()->size())
  {
    //std::cout << "Populating entries for " << this->root.back()->identifier() << std::endl;
    this->root.push_back(std::make_unique<Path>(raw, it - raw.begin()));

    this->root.back()->populate(img, this->real_block_size, this->block_size);

    /*
    for (auto& file : this->root.back()->entries())
    {
      std::cout << this->root.back()->identifier() << ": " << file.name() << std::endl;
    }
    */
  }

  img.close();

  this->m_filename = filename;
}

IMG::~IMG()
{

}

/*
  @return the contents of the filename provided or an empty vector if it doesn't exist in the image.
*/
Entry IMG::operator[](std::string filename)
{
  //  Convert to uppercase
  std::transform(filename.begin(), filename.end(), filename.begin(), ::toupper);

  uint16_t index = 0;           //  Current index of directory
  uint16_t last_index = 0;      //  Flag for parent directory
  uint32_t location = 0;        //  Unique location of current directory
  std::shared_ptr<Path> parent = nullptr; //  Path that holds the file requested

  //  Get the directories from the filename and the filename itself
  std::vector<std::string> directories = util::split(filename, "/", false);
  std::string file = directories.back();

  //  Remove last entry which was the file name
  directories.pop_back();

  //  Loop through directory names
  for (auto& dir : directories)
  {
    index = 0;  //  Current index of directory starts at 0 (root)

    //  For each Path in the list
    for (auto& path : this->root)
    {
      //  Check if the parent of this Path is the last index and if the name matches the one being searched for
      if (path->parent_index() == last_index && dir == path->identifier())
      {
        //  If it does set the next search up
        last_index = index;           //  Index of this directory
        location = path->location();  //  Unique location of this directory
        break;  //  Stop searching
      }

      index++;  //  Increase directory index
    }
  }

  //  Finialize the search by looping through all Path's to find the one with the correct location
  for (auto& path : this->root)
  {
    //  If the unique location matches
    if (location == path->location())
    {
      //  Set the parent path as the one found
      parent = path;
    }
  }

  if (parent == nullptr)
  {
    throw FileNotFoundException("File " + filename + " not found.");
  }

  //  For each file entry in the directory
  for (auto& entry : parent->entries())
  {
    //  If the file name matches then return the entry
    if (entry.name() == file)
    {
      return entry;
    }
  }

  //  No match, no file
  throw FileNotFoundException("File '" + filename + "' not found.");
}

void IMG::directories()
{

}

void IMG::files()
{

}

/*
  Converts the directory and file structure into a json string for passing to other applications.
*/
std::string IMG::to_json()
{
  std::string json = "";
  std::string directories = "\"directories\":[";
  std::string files = "\"files\":[";

  uint32_t dir_index = 0;
  uint32_t file_index = 0;

  for (auto& path : root)
  {
    directories += "{\"name\":\"" + path->identifier() + "\"," +
                   "\"id\":" + std::to_string(dir_index) + "," +
                   "\"parent\":" + std::to_string(path->parent_index()) + "},";

    for (auto& entry : path->entries())
    {
      files += "{\"name\":\"" + entry.name() + "\"," +
               "\"id\":" + std::to_string(file_index) + "," +
               "\"parent\":" + std::to_string(dir_index) + "},";

      file_index++;
    }
    dir_index++;
  }

  return "{" + directories.substr(0, directories.length() - 1) + "]," + files.substr(0, files.length() - 1) + "]}";
}

/*
  Gets the offset of the img header in bytes given a blocksize

  @param blocksize - The real blocksize of the img taken from the difference between sectors

  @return the entry location in bytes of the header
*/
uint32_t IMG::get_offset(std::fstream& file)
{
  if (this->has_headers)
  {
    //  If the IMG uses section headers then
    return this->real_block_size * 0x10 + IMG::SectionHeaderSize;
  }

  //  If no section headers, then attempt to find the header manually
  std::streampos pos = file.tellg();

  uint32_t header_pos = 0;
  uint32_t count = 0;
  std::string str(0x8000, ' ');

  while (header_pos == 0)
  {
    file.read(reinterpret_cast<char *>(&str[0]), 0x8000);

    auto cd = str.find("CD001");
    if (cd != std::string::npos && str[cd - 1] == 1)
    {
      header_pos = (0x8000 * count) + cd - 1;
    }
    count++;
  }

  file.seekg(pos, std::ios::beg);

  return header_pos;
}

/*
  Prints a directory tree of the file
*/
void IMG::print_tree()
{

}
