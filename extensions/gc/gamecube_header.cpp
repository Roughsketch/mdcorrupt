#include "gamecube_header.h"

#include <boost/filesystem.hpp>

GamecubeHeader::GamecubeHeader()
{
}

GamecubeHeader::GamecubeHeader(std::string file)
{
	initialize(file);
}

GamecubeHeader::~GamecubeHeader()
{
}

void GamecubeHeader::initialize(std::string file)
{
	//	If the file is not a valid Gamecube image then throw an exception
	if (boost::filesystem::file_size(file) != GamecubeHeader::FileSize)
	{
		throw InvalidGamecubeRomException();
	}

	//	Open Gamecube disc for reading
	std::ifstream ifs(file, std::ios::binary);

	//	Make room in the header vector
	header.resize(GamecubeHeader::Size);

	//	Read the data from 0x000 to 0x440 into header
	ifs.read(reinterpret_cast<char *>(&header[0]), GamecubeHeader::Size);

	//	Clean up
	ifs.close();
}
