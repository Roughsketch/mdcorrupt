#include "gamecube_image.h"

GamecubeImage::GamecubeImage()
{

}

GamecubeImage::GamecubeImage(std::string filename)
{
  initialize(filename);
}

GamecubeImage::~GamecubeImage()
{

}

void GamecubeImage::initialize(std::string filename)
{
  std::vector<uint8_t> fst;
  std::fstream ifs(filename, std::ios::in | std::ios::binary);

  if (!ifs.is_open())
  {
    throw InvalidFileNameException("Could not open file " + filename);
  }

  header = std::make_unique<GamecubeHeader>(filename);

  fst.resize(header->fst_size());

  ifs.seekg(header->fst_offset());
  ifs.read(reinterpret_cast<char *>(&fst[0]), header->fst_size());
  ifs.close();

  root = std::make_unique<GamecubeFST>(fst);

  image_file = filename;
}

void GamecubeImage::extract()
{
  std::ifstream ifs(image_file, std::ios::binary);

  if (!ifs.is_open())
  {
    throw FileOpenException(image_file);
  }

  for (auto& item : *root)
  {
    std::string path = "./tmp/" + item.first.substr(2); //  Replace ./ with ./tmp/ so files are extracted to a tmp directory
    std::vector<std::string> split = Util::split(path, "/", false);
    GamecubeEntry entry = item.second;

    if (entry.is_dir())
    {
      std::string current = "";
      for (auto& name : split)
      {
        current += name + "/";
        boost::filesystem::create_directory(current);
      }
    }
    else
    {
      std::ofstream ofs(path, std::ios::binary);

      if (ofs.is_open())
      {
        std::vector<uint8_t> content(entry.filesize());

        if (entry.filesize() > 0)
        {
          ifs.seekg(entry.address());
          ifs.read(reinterpret_cast<char *>(&content[0]), entry.filesize());

          ofs.write(reinterpret_cast<char *>(&content[0]), entry.filesize());
        }
        ofs.close();
      }
    }
  }
}

GamecubeEntry GamecubeImage::read(std::string path)
{
  return root->get(path);
}

void GamecubeImage::write(GamecubeEntry& entry, std::vector<uint8_t>& data)
{
  std::fstream ofs(image_file, std::ios::out | std::ios::binary);

  if (!ofs.is_open())
  {
    throw InvalidFileNameException("Could not open file " + image_file);
  }

  entry.write(ofs, data);
}

std::string GamecubeImage::to_json()
{

  std::string json = "";
  std::string files = "\"entries\":[";

  files += "{\"name\":\".\",\"id\":0,\"parent\":0,\"type\":\"d\"},";

  for (auto& path : *root)
  {
    std::string location = path.first;
    GamecubeEntry entry = path.second;

    files += "{\"name\":\"" + entry.name() + "\"," +
      "\"id\":" + std::to_string(entry.id()) + "," +
      "\"parent\":" + std::to_string(entry.parent_id()) + "," +
      "\"type\":" + (entry.is_dir() ? "\"d\"" : "\"f\"") + "},";
  }

  return "{" + files.substr(0, files.length() - 1) + "]}";
}
