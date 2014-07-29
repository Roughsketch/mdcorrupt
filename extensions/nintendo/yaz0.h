#ifndef _YAZ0_COMPRESSION_H
#define _YAZ0_COMPRESSION_H

/*
  Most of the code in the yaz0 files are taken from
  shevious (enc) and thakis (dec). I didn't want to
  spend days reading documentation on how to decode
  and encode YAZ0 files then debug everything, so I
  just reworked their code to work in this project.

  The majority of the actual work with the encoding
  and decoding have been taken from their code, but 
  the output, naming, and a few other minor details
  have been changed to suit the project better.

  Thank you very much shevious and thakis, although
  you will probably never see this.
*/


#include <vector>
#include <string>
#include <fstream>

#include <cstdint>


#include "helpers.h"

namespace yaz0
{
  namespace detail
  {
    struct Ret
    {
      uint32_t srcPos;
      uint32_t dstPos;
    };

    uint32_t toDWORD(uint32_t d);

    // simple and straight encoding scheme for Yaz0
    uint32_t simpleEnc(uint8_t* src, uint32_t size, uint32_t pos, uint32_t *pMatchPos);

    // a lookahead encoding scheme for ngc Yaz0
    uint32_t nintendoEnc(uint8_t* src, uint32_t size, uint32_t pos, uint32_t *pMatchPos); 
    
    Ret decodeYaz0(uint8_t* src, uint32_t srcSize, uint8_t* dst, uint32_t uncompressedSize);

  }

  std::vector<uint8_t> encode(std::string filename);
  std::vector<uint8_t> decode(std::string filename);
  std::vector<uint8_t> encode(std::vector<uint8_t>& src);
  std::vector<uint8_t> decode(std::vector<uint8_t>& src);
}

#endif
