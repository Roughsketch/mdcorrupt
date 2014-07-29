/*
  This header is deprecated. Use helpers.h instead.
*/

#ifndef _IMG_HELPER_H
#define _IMG_HELPER_H


namespace IMGHelper
{

  template <typename T> inline T read(std::vector<uint8_t>& data, uint32_t offset)
  {
    static_assert(std::is_integral<T>::value, "Value must be an integral type.");
    T ret = 0;

    for (uint32_t i = 0; i < sizeof(T); i++)
    {
      ret |= data[i + offset] << (i * 8);
    }

    return ret;
  }

  inline std::string read(std::vector<uint8_t>& data, uint32_t offset, size_t size)
  {
    std::string ret(data.begin() + offset, data.begin() + offset + size);
    ret.erase(ret.find_last_not_of(" \n\r\t") + 1);
    return ret;
  }
}

#endif
