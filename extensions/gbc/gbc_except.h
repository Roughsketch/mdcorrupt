#ifndef _GAMEBOY_COLOR_CORRUPTION_EXCEPTION_H
#define _GAMEBOY_COLOR_CORRUPTION_EXCEPTION_H

#include <exception>

#include "corruption_exceptions.h"

class InvalidGBCRomException : public InvalidRomException
{
public:
  InvalidGBCRomException() : InvalidRomException("Invalid Gameboy ROM file.") {};
};

#endif
