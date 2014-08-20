/*
Originally created by Maiddog <maiddogsrl@gmail.com> 6/20/2014

Contains a handful of useful functions that are used in various places.



Copyright (c) 2014 maiddog.com

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.

3. The name of the author may not be used to endorse or promote products
derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER "AS IS" AND ANY EXPRESS
OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _UTIL_H
#define _UTIL_H

#include <iostream>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <cstdint>
#include <vector>

namespace util
{
  inline std::vector<uint8_t> read_file(std::string filename, size_t count = std::numeric_limits<size_t>::max(), size_t offset = 0)
  {
    if (count == 0)
    {
      return std::vector<uint8_t>();
    }

    FILE *fp = fopen(filename.c_str(), "rb");

    if (!fp)
    {
      return std::vector<uint8_t>();
    }

    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    rewind(fp);

    if (size == 0)
    {
      fclose(fp);
      return std::vector<uint8_t>();
    }

    if (count < size)
    {
      size = count;
    }

    fseek(fp, offset, SEEK_SET);

    std::vector<uint8_t> ret(size);
    fread(&ret[0], sizeof(uint8_t), size, fp);
    fclose(fp);

    return ret;
  }

  inline void write_file(std::string filename, std::vector<uint8_t>& data, uint32_t count = 0)
  {
    FILE *fp = fopen(filename.c_str(), "wb");

    if (fp && (data.size() > 0))
    {
      fwrite(&data[0], sizeof(data[0]), count ? count : data.size(), fp);
      fclose(fp);
    }
  }

  inline void append_file(std::string filename, std::vector<uint8_t>& data, uint32_t count = 0, uint32_t offset = 0)
  {
    FILE *fp = fopen(filename.c_str(), "rb+");

    if (fp && (data.size() > 0))
    {
      if (offset > 0)
      {
        fseek(fp, offset, SEEK_SET);
      }
      else
      {
        fseek(fp, 0, SEEK_END);
      }

      fwrite(&data[0], sizeof(data[0]), count ? count : data.size(), fp);

      if (count > data.size())
      {
        char temp = 0;
        fwrite(&temp, 1, count - data.size(), fp);
      }

      fclose(fp);
    }
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

    for (int32_t i = sizeof(T)-1; i >= 0; i--)
    {
      //  Cast it to T to allow proper shifting for 64 bit values
      ret |= static_cast<T>(data[i + offset]) << (((sizeof(T)-1) - i) * 8);
    }

    return ret;
  }

  template <typename T> inline T read_big(std::string file, uint32_t offset = 0)
  {
    static_assert(std::is_integral<T>::value, "Value must be an integral type.");
    T ret = 0;

    FILE *fp = fopen(file.c_str(), "rb");

    if (!fp)
    {
      return -1;
    }

    fseek(fp, offset, SEEK_SET);
    fread(&ret, sizeof(T), 1, fp);
    fclose(fp);

    return swap_endian<T>(ret);
  }

  inline std::string read(std::vector<uint8_t>& data, uint32_t offset, size_t size = 0)
  {
    uint32_t length = size;

    //  If length is 0 then try to find the next null
    if (size == 0)
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

  inline std::string read(std::string& file, uint32_t offset, size_t size = 0)
  {
    std::string ret = "";
    uint32_t length = size;

    FILE *fp = fopen(file.c_str(), "rb");

    if (!fp)
    {
      return ret;
    }

    fseek(fp, offset, SEEK_SET);

    if (size == 0)
    {
      fgets(&ret[0], ret.max_size(), fp);
    }
    else
    {
      ret.resize(size);
      fread(&ret[0], 1, size, fp);
    }

    fclose(fp);

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

  //  swap_endian taken from StackOverflow
  //  https://stackoverflow.com/questions/105252
  template <typename T> T swap_endian(T u)
  {
    union
    {
      T u;
      unsigned char u8[sizeof(T)];
    } source, dest;

    source.u = u;

    for (size_t k = 0; k < sizeof(T); k++)
      dest.u8[k] = source.u8[sizeof(T)-k - 1];

    return dest.u;
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

  template<typename T, typename A> inline std::vector<T, A> subset(std::vector<T, A>& v, uint32_t start, uint32_t count)
  {
    return std::vector<T, A>(v.begin() + start, v.begin() + start + count);
  }

  template<typename T> inline void push_int(std::vector<uint8_t>& v, T val)
  {
    for (int i = 0; i < sizeof(T); i++)
    {
      v.push_back((val & (static_cast<T>(0xFF) << (i * 8))) >> (i * 8));
    }
  }

  template<typename T> inline void push_int_big(std::vector<uint8_t>& v, T val)
  {
    push_int<T>(v, swap_endian<T>(val));
  }

  template<typename T> inline void insert_int(std::vector<uint8_t>& v, T val, uint32_t offset = 0)
  {
    for (int i = 0; i < sizeof(T); i++)
    {
      v.insert(v.begin() + offset, (val & (static_cast<T>(0xFF) << (i * 8))) >> (i * 8));
    }
  }

  template<typename T> inline void insert_int_big(std::vector<uint8_t>& v, T val, uint32_t offset = 0)
  {
    insert_int<T>(v, swap_endian<T>(val), offset);
  }

  template<typename T> inline void write_int(std::vector<uint8_t>& v, T val, uint32_t offset)
  {
    for (int i = 0; i < sizeof(T); i++)
    {
      v[offset + i] = (val & (0xFF << (i * 8))) >> (i * 8);
    }
  }

  template<typename T> inline void write_int_big(std::vector<uint8_t>& v, T val, uint32_t offset)
  {
    write_int<T>(v, swap_endian<T>(val), offset);
  }

  inline void push(std::vector<uint8_t>& v, std::string val)
  {
    for (auto& c : val)
    {
      v.push_back(c);
    }
  }

  template<typename T> inline T pad(T val, uint32_t align)
  {
    static_assert(std::is_integral<T>::value, "Value must be an integral type.");
    return (val % align == 0) ? 0 : align - (val % align);
  }

  inline void pad(std::vector<uint8_t>& v, uint32_t count, uint8_t val = 0)
  {
    for (uint32_t i = 0; i < count; i++)
    {
      v.push_back(val);
    }
  }

  template<typename T> inline std::string zero_pad(T value, uint32_t count)
  {
    std::string ret = std::to_string(value);

    while (ret.length() < count)
    {
      ret = "0" + ret;
    }

    return ret;
  }

  template<typename T> inline uint8_t get_bit(T v, uint8_t offset)
  {
    return (v >> offset) & 1;
  }

  template<typename T> inline T read_bits(std::vector<uint8_t>& v, uint8_t num_bits, uint8_t& bits_left, uint32_t offset = 0)
  {
    T out = 0;
    uint8_t current = v[offset];

    for (uint8_t index = 1; num_bits > 0; num_bits--)
    {
      out = out << 1;
      out |= get_bit<uint8_t>(current, bits_left - 1);

      bits_left--;

      if (bits_left == 0)
      {
        current = v[offset + index];
        bits_left = 8;
        index++;
      }
    }

    return out;
  }
};

#endif
