#ifndef _GAMECUBE_IMAGE_H
#define _GAMECUBE_IMAGE_H

#include <memory>
#include <vector>

#include <boost/filesystem.hpp>

#include "gamecube_header.h"
#include "gamecube_entry.h"
#include "gamecube_fst.h"

#include "patch.h"

#include "helpers.h"

class GamecubeImage
{
public:
  GamecubeImage();
  GamecubeImage(std::string filename);
  ~GamecubeImage();

  void initialize(std::string filename);
  bool valid();

  void extract();

  GamecubeEntry read(std::string path);
  void write(GamecubeEntry& entry, std::vector<uint8_t>& data);

  std::string to_json();

private:
  std::string image_file;
  std::unique_ptr<GamecubeHeader> header;
  std::unique_ptr<GamecubeFST> root;
};

inline bool GamecubeImage::valid()
{
  return header->valid();
}

#endif
