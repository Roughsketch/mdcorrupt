#ifndef _NES_EXCEPTION_H
#define _NES_EXCEPTION_H

#include <exception>

#include "corruption_exceptions.h"

class InvalidNESRomException : public InvalidRomException
{
public:
  InvalidNESRomException() : InvalidRomException("Invalid NES ROM file.") {};
};

#endif
