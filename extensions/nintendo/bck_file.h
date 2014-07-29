#ifndef _NINTENDO_BCK_H
#define _NINTENDO_BCK_H

#include "nintendo.h"

namespace bck
{
  namespace detail
  {
    struct SubHeader
    {
      SubHeader() : m_magic(""), m_size(0) { };
      SubHeader(std::vector<uint8_t>& data, uint32_t offset = 0);
    private:
      std::string m_magic;
      uint32_t m_size;
    };

    struct Header
    {
      Header(std::vector<uint8_t>& data, uint32_t offset = 0);
      inline uint32_t size()
      {
        //  This is from looking at 3 random BCK files in Super Mario Sunshine.
        //  Can probably use improvement on accuracy.
        return 0x60;
      }
    private:
      std::string m_magic;
      uint32_t m_size;
      SubHeader m_ank;
    };
  }

  class BCKFile
  {
  public:
    //BCKFile(std::vector<uint8_t>& data, std::vector<std::string>& args);
    static void corrupt(std::vector<uint8_t>& data, std::vector<std::string>& args);

    static inline bool match(std::vector<uint8_t>& data)
    {
      return util::read(data, 0, 8) == "J3D1bck1" || util::read(data, 0, 8) == "J3D1btk1";
    }
  private:
    std::unique_ptr<CorruptionInfo> info;
  };
}

#endif
