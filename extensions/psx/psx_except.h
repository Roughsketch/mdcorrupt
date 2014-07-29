#ifndef _PLAYSTATION_EXCEPTION_H
#define _PLAYSTATION_EXCEPTION_H

class InvalidFileException : public std::runtime_error
{
public:
  InvalidFileException(std::string why) : std::runtime_error(why) {};
};

#endif
