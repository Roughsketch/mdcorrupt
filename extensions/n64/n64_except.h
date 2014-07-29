#ifndef _N64_EXCEPTION_H
#define _N64_EXCEPTION_H

#include <exception>

#include "corruption_exceptions.h"

class InvalidN64RomException : public InvalidRomException
{
public:
  InvalidN64RomException() : InvalidRomException("Invalid N64 ROM file.") {};
};

#endif
