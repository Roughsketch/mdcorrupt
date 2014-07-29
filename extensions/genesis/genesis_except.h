#ifndef _GENESIS_EXCEPTION_H
#define _GENESIS_EXCEPTION_H

#include <exception>

#include "corruption_exceptions.h"

class InvalidGenesisRomException : public InvalidRomException
{
public:
  InvalidGenesisRomException() : InvalidRomException("Invalid Genesis ROM file.") {};
};

#endif
