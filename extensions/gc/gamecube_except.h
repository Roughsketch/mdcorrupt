#ifndef _GAMECUBE_EXCEPTION_H
#define _GAMECUBE_EXCEPTION_H

#include <exception>

#include "corruption_exceptions.h"

class InvalidGamecubeRomException : public InvalidRomException
{
public:
	InvalidGamecubeRomException() : InvalidRomException("Invalid Gamecube image.") {};
};

class GamecubeFileNotFoundException : public std::runtime_error
{
public:
  GamecubeFileNotFoundException(std::string why) : std::runtime_error(why) {};
};

class InvalidFileSize : public std::runtime_error
{
public:
  InvalidFileSize() : std::runtime_error("Entry size and data size do not match.") {};
};

#endif
