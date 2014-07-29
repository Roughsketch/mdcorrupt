/*
  A handful of functions that are used in various places in the project.
*/

#ifndef _HELPERS_H
#define _HELPERS_H

#include <iostream>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <cstdint>
#include <vector>

namespace Util
{
  inline std::vector<uint8_t> read_file(std::string filename)
  {
    std::ifstream ifs(filename, std::ios::binary);

    ifs.seekg(0, std::ios::end);
    auto filesize = ifs.tellg();
    ifs.seekg(0);

    std::vector<uint8_t> content(static_cast<uint32_t>(filesize));

    ifs.read(reinterpret_cast<char *>(&content[0]), filesize);
    ifs.close();

    return content;
  }

  inline void write_file(std::string filename, std::vector<uint8_t>& data)
  {
    std::ofstream ofs(filename, std::ios::binary);
    ofs.write(reinterpret_cast<char *>(&data[0]), data.size());
    ofs.close();
  }

  template <typename T> inline T read(std::vector<uint8_t>& data, uint32_t offset = 0)
  {
    static_assert(std::is_integral<T>::value, "Value must be an integral type.");
    T ret = 0;

    for (uint32_t i = 0; i < sizeof(T); i++)
    {
      //  Cast it to T to allow proper shifting for 64 bit values
      ret |= static_cast<T>(data[i + offset]) << (i * 8);
    }

    return ret;
  }

  template <typename T> inline T read_big(std::vector<uint8_t>& data, uint32_t offset = 0)
  {
    static_assert(std::is_integral<T>::value, "Value must be an integral type.");
    T ret = 0;

    for (int32_t i = sizeof(T) - 1; i >= 0; i--)
    {
      //  Cast it to T to allow proper shifting for 64 bit values
      ret |= static_cast<T>(data[i + offset]) << (((sizeof(T) - 1) - i) * 8);
    }

    return ret;
  }

  inline std::string read(std::vector<uint8_t>& data, uint32_t offset, size_t size = 0)
  {
    uint32_t length = size;

    //  If length is 0 then try to find the next null
    if (length == 0)
    {
      while (data[offset + length] != '\0' && offset + length < data.size())
      {
        length++;
      }
    }

    std::string ret(data.begin() + offset, data.begin() + offset + length);
    ret.erase(ret.find_last_not_of(" \n\r\t") + 1);
    return ret;
  }

  /*
  Implementation taken from the following link:
  http://programmingknowledgeblog.blogspot.com/2013/04/the-most-elegant-way-to-split-string-in.html
  */
  inline std::vector<std::string> split(const std::string& s, const std::string& delim, const bool keep_empty)
  {
    std::vector<std::string> result;

    if (delim.empty()) 
    {
      result.push_back(s);
      return result;
    }

    std::string::const_iterator substart = s.begin(), subend;

    while (true) 
    {
      subend = std::search(substart, s.end(), delim.begin(), delim.end());
      std::string temp(substart, subend);

      if (keep_empty || !temp.empty()) 
      {
        result.push_back(temp);
      }

      if (subend == s.end()) 
      {
        break;
      }

      substart = subend + delim.size();
    }

    return result;
  }

  inline uint32_t to_int32(const std::string& str)
  {
    std::istringstream iss;
    uint32_t value;

    iss.str(str);
    iss >> value;

    return value;
  }

  template<typename T> inline T rol(T x, uint32_t n)
  {
    static_assert(std::is_integral<T>::value, "Value must be an integral type.");

    return x << (n % (sizeof(x)* 8)) | x >> ((sizeof(x)* 8) - (n % (sizeof(x)* 8)));
  }

  template<typename T> inline T ror(T x, uint32_t n)
  {
    static_assert(std::is_integral<T>::value, "Value must be an integral type.");

    return x >> (n % (sizeof(x)* 8)) | x << ((sizeof(x)* 8) - (n % (sizeof(x)* 8)));
  }

  template<typename T, typename A> std::vector<T, A> subset(std::vector<T, A>& v, uint32_t start, uint32_t count)
  {
    return std::vector<T, A>(v.begin() + start, v.begin() + start + count);
  }
};

#endif
