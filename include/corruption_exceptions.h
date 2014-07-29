#ifndef _CORRUPTION_EXCEPTION_H
#define _CORRUPTION_EXCEPTION_H

#include <string>
#include <exception>
#include <stdexcept>
#include <iostream>

class InvalidRomException : public std::runtime_error
{
public:
  InvalidRomException() : std::runtime_error("Invalid ROM file.") {};
  InvalidRomException(std::string msg) : std::runtime_error(msg) {};
};

class InvalidArgumentException : public std::runtime_error
{
public:
  InvalidArgumentException(std::string arg) : std::runtime_error("Invalid argument: " + arg) {};
};

class InvalidFileNameException : public std::runtime_error
{
public:
  InvalidFileNameException(std::string arg) : std::runtime_error("Invalid argument: " + arg) {};
};

class FileOpenException : public std::runtime_error
{
public:
  FileOpenException(std::string arg) : std::runtime_error("Could not open file: " + arg) {};
};

#endif
