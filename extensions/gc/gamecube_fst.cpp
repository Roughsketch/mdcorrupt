#include "gamecube_fst.h"

GamecubeFST::GamecubeFST()
{

}

GamecubeFST::GamecubeFST(std::vector<uint8_t>& data)
{
  initialize(data);
}

GamecubeFST::~GamecubeFST()
{

}

void GamecubeFST::initialize(std::vector<uint8_t>& data)
{
  GamecubeEntry root(data);

  uint32_t current_offset = 0;
  uint32_t current_index = 1;
  std::vector<GamecubeEntry> current_dir{ root };
  std::vector<uint32_t> current_parent{ 0 };
  std::vector<std::string> current_path{ "." };
  std::string last_file = "";

  //  Fill the children of root
  for (uint32_t i = 1; i < root.payload(); i++)
  {
    bool popped = false;
    GamecubeEntry entry = GamecubeEntry(data, i * GamecubeEntry::DirectorySize, root.payload() * GamecubeEntry::DirectorySize);

    //  Gamecube doesn't care about upper/lower for alphabetical listings
    //  so just make the label uppercase for the checks
    std::string upper_label = entry.name();
    std::transform(upper_label.begin(), upper_label.end(), upper_label.begin(), ::toupper);

    if (entry.is_dir())
    {
      entry.setid(current_index++);
    }

    //  Move back until the correct parent directory if the previous is finished
    while (current_offset > 0 && current_dir[current_offset].payload() <= i)
    {
      current_offset--;       //  Move back one directory
      popped = true;          //  For next check
      last_file = "";         //  Reset file listing
    }

    //  If this file isn't in alphabetical order then it goes one directory up
    if (!popped && current_offset > 0 && entry.is_file() && (last_file > upper_label))  // || (i >= current_dir[current_offset].payload))
    {
      current_offset--; //  Move back one directory
      popped = true;
    }

    //  Set the parent id to the current directory
    entry.setparent(current_offset);

    if (entry.is_file())
    {
      //  Set last file name to check for alphabetical order
      last_file = upper_label;
    }
    else
    {
      //  If this is a directory, then next files will be for this entry
      current_dir.push_back(entry); //  Push directory into current
      last_file = "";               //  Reset file listing check
      current_path.push_back(current_path[current_offset] + "/" + entry.name());
      current_offset = current_dir.size() - 1;
    }

    std::string path = current_path[current_offset];

    //  Append filename to path if available
    if (entry.is_file())
    {
      path += "/" + entry.name();
    }

    //std::cout << "Current path: " << path << std::endl;

    //  Add entry to fst
    this->fst_map[path] = entry;
  }
}