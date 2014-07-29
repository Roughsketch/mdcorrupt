#include "yay0.h"

namespace yay0
{
  /*
      This function was adapted from thakis' Yay0 decoder.
      You can find the source to that here: http://www.amnoid.de/gc/yay0dec.zip
  */
  Ret decodeYay0(std::vector<uint8_t>& codes, std::vector<uint8_t>& counts, std::vector<uint8_t>& srcData, std::vector<uint8_t>& dst, int uncompressedSize)
  {
    Ret r = { 0, 0 };
    //int srcPlace = 0, dstPlace = 0; //current read/write positions

    uint32_t codePos = 0;
    uint32_t countPos = 0;

    uint32_t validBitCount = 0; //number of valid bits left in "code" byte
    uint8_t currCodeByte;
    while (r.dstPos < uncompressedSize)
    {
      //read new "code" byte if the current one is used up
      if (validBitCount == 0)
      {
        currCodeByte = codes[codePos];
        ++codePos;
        validBitCount = 8;
      }

      if ((currCodeByte & 0x80) != 0)
      {
        //straight copy
        dst[r.dstPos] = srcData[r.srcPos];
        r.dstPos++;
        r.srcPos++;
        //if(r.srcPos >= srcSize)
        //  return r;
      }
      else
      {
        //RLE part
        uint8_t byte1 = counts[countPos];
        uint8_t byte2 = counts[countPos + 1];
        countPos += 2;
        //if(r.srcPos >= srcSize)
        //  return r;

        uint32_t dist = ((byte1 & 0xF) << 8) | byte2;
        uint32_t copySource = r.dstPos - (dist + 1);

        uint32_t numBytes = byte1 >> 4;
        if (numBytes == 0)
        {
          numBytes = srcData[r.srcPos] + 0x12;
          r.srcPos++;
          //if(r.srcPos >= srcSize)
          //  return r;
        }
        else
        {
          numBytes += 2;
        }

        //copy run
        for (uint32_t i = 0; i < numBytes; ++i)
        {
          dst[r.dstPos] = dst[copySource];
          copySource++;
          r.dstPos++;
        }
      }

      //use next bit from "code" byte
      currCodeByte <<= 1;
      validBitCount -= 1;
    }

    return r;
  }


  std::vector<uint8_t> decode(const std::vector<uint8_t>& src)
  {
    if (util::read(src, 0, 4) != "Yay0")
    {
      return std::vector<uint8_t>();
    }

    uint32_t decodedSize = util::read_big<uint32_t>(src, 4);
    uint32_t countOffset = util::read_big<uint32_t>(src, 8);
    uint32_t dataOffset = util::read_big<uint32_t>(src, 12);


    std::vector<uint8_t> dst(decodedSize + 0x1000);

    //Ret r = decodeYay0(src + 16, src + countOffset, src + dataOffset, dst, decodedSize);
    Ret r = decodeYay0(util::subset(src, 16, src.size() - 16), util::subset(src, countOffset, src.size() - countOffset), util::subset(src, dataOffset, src.size() - dataOffset), dst, decodedSize);

    return dst;
  }
}