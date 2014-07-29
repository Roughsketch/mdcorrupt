#include "btp_file.h"

namespace btp
{
  void BTPFile::corrupt(std::vector<uint8_t>& data, std::vector<std::string>& args)
  {
    //  As far as I'm aware, you only need to skip the header which is always 0x20 bytes.
    std::vector<uint8_t> subdata(data.begin() + 0x20, data.end());
    NintendoFile::corrupt(subdata, args);
    std::copy(subdata.begin(), subdata.end(), data.begin() + 0x20);
  }
}