#ifndef _IMG_EXCEPTION_H
#define _IMG_EXCEPTION_H

#include <exception>

class IMGOpenException : public std::runtime_error
{
public:
  IMGOpenException() : std::runtime_error("Could not open image.") {};
};

class IMGException : public std::runtime_error
{
public:
  IMGException(std::string why) : std::runtime_error(why) {};
};

class FileNotFoundException : public std::runtime_error
{
public:
  FileNotFoundException(std::string why) : std::runtime_error(why) {};
};

#endif
