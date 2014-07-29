#include "yaz0.h"
#include <iostream>

namespace yaz0
{
  namespace detail
  {
    uint32_t toDWORD(uint32_t d)
    {
      uint8_t w1 = d & 0xFF;
      uint8_t w2 = (d >> 8) & 0xFF;
      uint8_t w3 = (d >> 16) & 0xFF;
      uint8_t w4 = d >> 24;
      return (w1 << 24) | (w2 << 16) | (w3 << 8) | w4;
    }

    // simple and straight encoding scheme for Yaz0
    uint32_t simpleEnc(uint8_t* src, uint32_t size, uint32_t pos, uint32_t *pMatchPos)
    {
      uint32_t i, j;
      int32_t startPos = pos - 0x1000;
      uint32_t numBytes = 1;
      uint32_t matchPos = 0;

      if (startPos < 0)
      {
        startPos = 0;
      }

      for (i = startPos; i < pos; i++)
      {
        for (j = 0; j < size - pos; j++)
        {
          if (src[i + j] != src[j + pos])
          {
            break;
          }
        }

        if (j > numBytes)
        {
          numBytes = j;
          matchPos = i;
        }
      }

      *pMatchPos = matchPos;

      if (numBytes == 2)
      {
        numBytes = 1;
      }

      return numBytes;
    }

    // a lookahead encoding scheme for ngc Yaz0
    uint32_t nintendoEnc(uint8_t* src, uint32_t size, uint32_t pos, uint32_t *pMatchPos)
    {
      int startPos = pos - 0x1000;
      uint32_t numBytes = 1;
      static uint32_t numBytes1;
      static uint32_t matchPos;
      static int prevFlag = 0;

      // if prevFlag is set, it means that the previous position was determined by look-ahead try.
      // so just use it. this is not the best optimization, but nintendo's choice for speed.
      if (prevFlag == 1)
      {
        *pMatchPos = matchPos;
        prevFlag = 0;
        return numBytes1;
      }

      prevFlag = 0;
      numBytes = simpleEnc(src, size, pos, &matchPos);
      *pMatchPos = matchPos;

      // if this position is RLE encoded, then compare to copying 1 byte and next position(pos+1) encoding
      if (numBytes >= 3) 
      {
        numBytes1 = simpleEnc(src, size, pos + 1, &matchPos);

        // if the next position encoding is +2 longer than current position, choose it.
        // this does not guarantee the best optimization, but fairly good optimization with speed.
        if (numBytes1 >= numBytes + 2)
        {
          numBytes = 1;
          prevFlag = 1;
        }
      }

      return numBytes;
    }

    Ret decodeYaz0(uint8_t* src, uint32_t srcSize, uint8_t* dst, uint32_t uncompressedSize)
    {
      Ret r = { 0, 0 };
      //int srcPlace = 0, dstPlace = 0; //current read/write positions

      uint32_t validBitCount = 0; //number of valid bits left in "code" byte
      uint8_t currCodeByte;

      while (r.dstPos < uncompressedSize)
      {
        //read new "code" byte if the current one is used up
        if (validBitCount == 0)
        {
          currCodeByte = src[r.srcPos];
          ++r.srcPos;
          validBitCount = 8;
        }

        if ((currCodeByte & 0x80) != 0)
        {
          //straight copy
          dst[r.dstPos] = src[r.srcPos];
          r.dstPos++;
          r.srcPos++;
          //if(r.srcPos >= srcSize)
          //  return r;
        }
        else
        {
          //RLE part
          uint8_t byte1 = src[r.srcPos];
          uint8_t byte2 = src[r.srcPos + 1];
          r.srcPos += 2;
          //if(r.srcPos >= srcSize)
          //  return r;

          uint32_t dist = ((byte1 & 0xF) << 8) | byte2;
          uint32_t copySource = r.dstPos - (dist + 1);

          uint32_t numBytes = byte1 >> 4;
          if (numBytes == 0)
          {
            numBytes = src[r.srcPos] + 0x12;
            r.srcPos++;
            //if(r.srcPos >= srcSize)
            //  return r;
          }
          else
            numBytes += 2;

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
  }

  std::vector<uint8_t> encode(std::string filename)
  {
    return encode(Util::read_file(filename));
  }

  std::vector<uint8_t> decode(std::string filename)
  {
    return decode(Util::read_file(filename));
  }

  std::vector<uint8_t> encode(std::vector<uint8_t>& src)
  {
    std::vector<uint8_t> ret;

    //FILE* dstFile = nullptr;
    //fopen_s(&dstFile, "out_file.szs", "wb");

    ret.push_back('Y');
    ret.push_back('a');
    ret.push_back('z');
    ret.push_back('0');

    ret.push_back((src.size() & 0xFF000000) >> 24);
    ret.push_back((src.size() & 0x00FF0000) >> 16);
    ret.push_back((src.size() & 0x0000FF00) >> 8);
    ret.push_back((src.size() & 0x000000FF));

    for (int i = 0; i < 8; i++)
    {
      ret.push_back(0);
    }

    detail::Ret r = { 0, 0 };
    uint8_t dst[24];    // 8 codes * 3 bytes maximum
    int dstSize = 0;
    int percent = -1;

    uint32_t validBitCount = 0; //number of valid bits left in "code" byte
    uint8_t currCodeByte = 0;

    while (r.srcPos < src.size())
    {
      uint32_t numBytes;
      uint32_t matchPos;
      uint32_t srcPosBak;

      numBytes = detail::nintendoEnc(&src[0], src.size(), r.srcPos, &matchPos);
      if (numBytes < 3)
      {
        //straight copy
        dst[r.dstPos] = src[r.srcPos];
        r.dstPos++;
        r.srcPos++;
        //set flag for straight copy
        currCodeByte |= (0x80 >> validBitCount);
      }
      else
      {
        //RLE part
        uint32_t dist = r.srcPos - matchPos - 1;
        uint8_t byte1, byte2, byte3;

        if (numBytes >= 0x12)  // 3 byte encoding
        {
          byte1 = 0 | (dist >> 8);
          byte2 = dist & 0xff;

          dst[r.dstPos++] = byte1;
          dst[r.dstPos++] = byte2;

          // maximum runlength for 3 byte encoding
          if (numBytes > 0xff + 0x12)
          {
            numBytes = 0xff + 0x12;
          }

          byte3 = numBytes - 0x12;
          dst[r.dstPos++] = byte3;
        }
        else  // 2 byte encoding
        {
          byte1 = ((numBytes - 2) << 4) | (dist >> 8);
          byte2 = dist & 0xff;
          dst[r.dstPos++] = byte1;
          dst[r.dstPos++] = byte2;
        }
        r.srcPos += numBytes;
      }

      validBitCount++;

      //write eight codes
      if (validBitCount == 8)
      {
        //fwrite(&currCodeByte, 1, 1, dstFile);
        //fwrite(dst, 1, r.dstPos, dstFile);
        //fflush(dstFile);

        ret.push_back(currCodeByte);

        for (uint32_t i = 0; i < r.dstPos; i++)
        {
          ret.push_back(dst[i]);
        }

        dstSize += r.dstPos + 1;

        srcPosBak = r.srcPos;
        currCodeByte = 0;
        validBitCount = 0;
        r.dstPos = 0;
      }
      if ((r.srcPos + 1) * 100 / src.size() != percent) 
      {
        percent = (r.srcPos + 1) * 100 / src.size();
      }
    }

    if (validBitCount > 0)
    {
      //fwrite(&currCodeByte, 1, 1, dstFile);
      //fwrite(dst, 1, r.dstPos, dstFile);

      ret.push_back(currCodeByte);

      for (uint32_t i = 0; i < r.dstPos; i++)
      {
        ret.push_back(dst[i]);
      }

      dstSize += r.dstPos + 1;

      currCodeByte = 0;
      validBitCount = 0;
      r.dstPos = 0;
    }

    return ret;
  }

  std::vector<uint8_t> decode(std::vector<uint8_t>& src)
  {
    std::vector<uint8_t> ret;

    uint32_t readBytes = 0;

    while (readBytes < src.size())
    {
      /*
        && (src[readBytes] != 'Y'
        || src[readBytes + 1] != 'a'
        || src[readBytes + 2] != 'z'
        || src[readBytes + 3] != '0'))
      */
      //search yaz0 block
      while (readBytes + 3 < src.size() && Util::read(src, readBytes, 4) != "Yaz0")
      {
        readBytes++;
      }

      if (readBytes + 3 >= src.size())
      {
        break; //nothing left to decode
      }

      readBytes += 4;

      //  char dstName[300];
      //  sprintf_s(dstName, "%s %x.rarc", filename.c_str(), readBytes - 4);
      //  FILE* DataFile;
      //
      //  if ((DataFile = fopen(dstName, "wb")) == NULL)
      //  {
      //    return std::vector<uint8_t>(0);
      //  }
      //  printf("Writing %s\n", dstName);


      //  Updated version of above, but not needed.
      //  std::string dstName = filename + " " + std::to_string(readBytes - 4) + ".rarc";
      //  std::cout << "Writing " << dstName << std::endl;

      //uint32_t Size = detail::toDWORD(*(uint32_t*)(&src[0] + readBytes));
      uint32_t Size = Util::read_big<uint32_t>(src, readBytes);

      //printf("Writing 0x%X bytes\n", Size);
      //std::cout << "Writing 0x" << Size << " bytes" << std::endl;

      //uint8_t* Dst = (uint8_t *)malloc(Size + 0x1000);
      std::vector<uint8_t> Dst(Size + 0x1000);

      readBytes += 12; // 4 byte size, 8 byte unused

      detail::Ret r = detail::decodeYaz0(&src[0] + readBytes, src.size() - readBytes, &Dst[0], Size);
      readBytes += r.srcPos;

      //printf("Read 0x%X bytes from input\n", readBytes);
      //std::cout << "Read 0x" << readBytes << " bytes from input" << std::endl;

      for (uint32_t i = 0; i < r.dstPos; i++)
      {
        ret.push_back(Dst[i]);
      }
      //free(Dst);

      //fwrite(Dst, 1, r.dstPos, DataFile);
      //fclose(DataFile);
    }

    return ret;
  }
}