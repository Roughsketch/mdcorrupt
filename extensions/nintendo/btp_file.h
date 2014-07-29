#ifndef _BTP_FILE_H
#define _BTP_FILE_H

#include "nintendo.h"

namespace btp
{
  /*
  namespace detail
  {
    struct BTPHeader
    {
    private:

    };
  }
  */
  class BTPFile
  {
  public:
    static void corrupt(std::vector<uint8_t>& data, std::vector<std::string>& args);

    static inline bool match(std::vector<uint8_t>& data)
    {
      return Util::read(data, 0, 8) == "J3D1btp1";
    }
  private:
  };
}

#endif
