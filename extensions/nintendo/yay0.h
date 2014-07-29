#include <vector>
#include <cstdint>

#include "util.h"

namespace yay0
{
  struct Ret
  {
    int srcPos, dstPos;
  };

  Ret decodeYay0(std::vector<uint8_t>& codes, std::vector<uint8_t>& counts, std::vector<uint8_t>& srcData, std::vector<uint8_t>& dst, int uncompressedSize);
  std::vector<uint8_t> decode(const std::vector<uint8_t>& src);
}