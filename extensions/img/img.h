#ifndef _IMG_H
#define _IMG_H

/*
  This is a simple implementation of the raw disc format
  which comprises the .IMG and .BIN formats (hereby references
  as just IMG). The format is extremely similar to ISO9660 
  with some minor changes which I will log here as I find them.

======================================================================
  Differences between IMG and ISO9660 files:

    1)  IMG files have a volume sequence header (VSH) for every sector

          Volume sequence headers always start the same, but the
          ending bytes are still mostly unknown. After the beginning
          10 bytes there follows a possible 0 or 1 followed by the
          section number. After that it seems to be useless data.
          Volume sequence headers are always 0x18 bytes long.

    2)  IMG files have junk data at the end of each sector

          ISO9660 specifies the logical block size in the Primary
          Volume Header, which is usually 0x800. However, the real
          logical block size for an IMG file is taken from the
          difference between the first and second volume sequence
          header. For PSX roms it seems this is always 0x930.

    3)  Path and File entries usually (perhaps always) start with a VSH

          Since files and directories may start on the boundary of a
          sector of the disk, it is possible to have the first 0x18
          bytes be the VSH instead of actual data. If this is the case
          then one must skip the first 0x18 bytes before reading.

    4)  Data is split into multiple sectors

          For reading a file that starts at the beginning of a specific
          sector, the format follows these guidelines:

            RBS = Real Block Size (from difference of VSH)
            LBS = Logical Block Size (from the Primary Volume Header)
            VSS = Size of the Volume Sequence Header (0x18)
            
                ,------------------------,
                | Volume Sequence Header |
                |------------------------|
                |  Data (LBS or smaller) |
                |------------------------|
                | RBS-LBS-VSS junk bytes |
                |------------------------|
                | Volume Sequence Header |
                |------------------------|
                |  Data (LBS or smaller) |
                |------------------------|
                | RBS-LBS-VSS junk bytes |
                |------------------------|
                |         ......         |
                |------------------------|
                |    Repeat until EOF    |
                `------------------------`

    5)  Handling of data is extremely similar to ISO9660
          
          For PSX disks it seems that the handling of data is exactly
          the same as ISO9660. However, all offsets are now multiples
          of the Real Block Size rather than the Logical Block Size
          and one must take into account the stuttering data layout
          as described in section 4 above.

======================================================================
*/


#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstdint>

#include "img_entry.h"
#include "img_except.h"
#include "img_path.h"

#include "util.h"

class IMG
{
public:
  static const uint8_t SectionHeaderSize = 0x18;

  IMG();
  IMG(std::string filename, bool has_headers = true);
  ~IMG();

  Entry operator[](std::string file);
  std::string to_json();

  void directories();
  void files();

private:
  std::vector<std::shared_ptr<Path>> root;
  std::string m_filename;

  uint32_t real_block_size;
  uint32_t block_size;
  uint32_t path_table_size;
  uint32_t path_table_location;

  bool has_headers;

  uint32_t get_offset(std::fstream& file);
  void print_tree();
};

#endif
