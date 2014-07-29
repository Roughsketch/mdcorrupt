#ifndef _GAMECUBE_FILE_SYSTEM
#define _GAMECUBE_FILE_SYSTEM

#include <map>
#include <vector>
#include <cstdint>

#include "gamecube_entry.h"

class GamecubeFST
{
public:
  GamecubeFST();
  GamecubeFST(std::vector<uint8_t>& data);
  ~GamecubeFST();

  std::map<std::string, GamecubeEntry>::iterator begin()
  {
    return fst_map.begin();
  }

  std::map<std::string, GamecubeEntry>::iterator end()
  {
    return fst_map.end();
  }

  std::map<std::string, GamecubeEntry>::const_iterator begin() const
  {
    return fst_map.begin();
  }

  std::map<std::string, GamecubeEntry>::const_iterator end() const
  {
    return fst_map.end();
  }

  void initialize(std::vector<uint8_t>& data);

  GamecubeEntry get(std::string path);
private:
  std::map<std::string, GamecubeEntry> fst_map;
};

inline GamecubeEntry GamecubeFST::get(std::string path)
{
  if (this->fst_map.count(path))
  {
    return this->fst_map.at(path);
  }

  throw GamecubeFileNotFoundException("Could not find " + path);
}

#endif
