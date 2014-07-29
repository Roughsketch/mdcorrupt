#include "nds_filesystem.h"

#include <map>

NDSFileSystem::NDSFileSystem()
{

}

NDSFileSystem::NDSFileSystem(std::vector<uint8_t>& rom, std::unique_ptr<NDSHeader>& header)
{
  this->initialize(rom, header);
}

NDSFileSystem::~NDSFileSystem()
{

}

void NDSFileSystem::initialize(std::vector<uint8_t>& rom, std::unique_ptr<NDSHeader>& header)
{
  std::cout << "FNT Offset: " << std::hex << header->file_name_table() << std::dec << std::endl;
  uint16_t dir_total = util::read<uint16_t>(ref(rom), header->file_name_table() + 6);
  uint16_t first_id = util::read<uint16_t>(ref(rom), header->file_name_table() + 4);
  std::cout << "Initializing NDS File System. " << dir_total << " directories found." << std::endl;

  std::vector<std::string> current_directory = { "." };

  std::vector<NDSMainTableEntry> main_table_entries;
  std::map<uint16_t, std::string> directories = { { 0, "." } };
  uint32_t dir_index = 0;
  
  //  Get each of the Main Table Entries into a vector
  for (uint32_t i = 0; i < dir_total; i++)
  {
    main_table_entries.push_back(NDSMainTableEntry(rom, header->file_name_table() + i * 8));
  }

  //  Iterate over each entry
  for (auto& mte : main_table_entries)
  {
    uint32_t file_id = mte.start_id();  //  The starting file id in this folder
    uint32_t subdirs = 0;               //  The amount of sub directories in this folder

    //  Offsets are relative to the FNT offset
    uint32_t offset = mte.offset() + header->file_name_table();
    std::vector<uint32_t> last = { offset };

    std::cout << "NEW" << std::endl;
    //  0 indicates the section has ended
    while (rom[offset] != 0)
    {
      std::cout << "OFFSET: " << std::hex << offset << "\t" << static_cast<int>(rom[offset]) << std::dec << std::endl;
      std::string entry;
      uint8_t len = rom[offset];  //  First byte is the length of the file/directory name

      //  If length is 0x01 to 0x7F then it is a file entry
      if (len > 0x00 && len < 0x80)
      {
        entry = util::read(rom, offset + 1, len);     //  Get the file name
        entry = current_directory[dir_index] + "/" + entry; //  Prepend the directory structure before the file name
        offset += len + 1;                                  //  Offset increase by length byte + file name length

        //  File Allocation Table holds pairs of integers noting start and end offsets into the rom.
        uint32_t file_start = util::read<uint32_t>(rom, header->file_alloc_table() + file_id * 8);
        uint32_t file_end = util::read<uint32_t>(rom, header->file_alloc_table() + file_id * 8 + 4);

        //  Create new NDSEntry with file start offset and file size and store it into the entries map
        m_entries[entry] = NDSEntry(file_start, file_end - file_start);

        std::cout << "File: " << entry << std::endl;

        //  Next file id
        file_id++;
      }
      //  If the length is 0x81 to 0xFF then it is a directory entry
      else if (len > 0x80)
      {
        len -= 0x80;  //  Subtract 0x80 to get directory name length
        entry = util::read(rom, offset + 1, len); //  Get the directory name

        //  Directories have an extra 2 bytes indicating the directory id
        uint16_t id = util::read<uint16_t>(rom, offset + len + 1) & 0x0FFF; //  id is in form 0xFXXX so and it with 0xFFF

        offset += len + 3;  //  Increase offset by length byte + 2 bytes for id + name length

        directories[id] = entry;
        if (dir_index != 0)
        {
          current_directory.insert(current_directory.begin() + dir_index + subdirs + 1, current_directory[dir_index] + "/" + entry);
          subdirs++;
          //current_directory.push_back(current_directory[dir_index] + "/" + entry);
        }
        else
        {
          current_directory.push_back("./" + entry);
        }

        std::cout << "Directory: " << current_directory[dir_index] << std::endl;
      }
      else
      {
        std::cout << "INVALID " << last.size() << std::endl;
        //  0x00 and 0x80 are reserved
        //  0x00 indicates end of sub-table
        //  0x80 is reserved
      }
    }

    //  Increase directory index to point to the next directory name in current_directory
    dir_index++;
  }

  for (auto& entry : m_entries)
  {
    std::cout << entry.first << std::endl;
  }
}

std::string NDSFileSystem::to_json()
{

  std::string json = "";
  std::string files = "{\"entries\":[";


  for (auto& file : m_entries)
  {
    std::string location = file.first;
    NDSEntry entry = file.second;

    files += "\"" + location + "\",";
  }

  return files.substr(0, files.length() - 1) + "]}";
}