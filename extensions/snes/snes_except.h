#ifndef _SNES_EXCEPTION_H
#define _SNES_EXCEPTION_H

#include <exception>

#include "corruption_exceptions.h"

class InvalidSNESRomException : public InvalidRomException
{
public:
  InvalidSNESRomException() : InvalidRomException("Invalid SNES ROM file.") {};
};

#endif
