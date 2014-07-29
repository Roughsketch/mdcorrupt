#ifndef _NDS_EXCEPTION_H
#define _NDS_EXCEPTION_H

#include <exception>

#include "corruption_exceptions.h"

class InvalidNDSRomException : public InvalidRomException
{
public:
  InvalidNDSRomException() : InvalidRomException("Invalid NDS image.") {};
};

class NDSFileNotFoundException : public std::runtime_error
{
public:
  NDSFileNotFoundException(std::string why) : std::runtime_error(why) {};
};

#endif
