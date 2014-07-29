#include "rarc.h"

/*
namespace rarc
{
  namespace detail
  {
    std::string getString(int pos, FILE* f)
    {
      int t = ftell(f);
      fseek(f, pos, SEEK_SET);

      std::string ret;
      char c;
      while ((c = fgetc(f)) != '\0')
        ret.append(1, c);

      fseek(f, t, SEEK_SET);

      return ret;
    }

    Node getNode(int i, std::ifstream& f)
    {
      fseek(f, sizeof(RarcHeader)+i*sizeof(Node), SEEK_SET);
      Node ret;
      fread(&ret, 1, sizeof(Node), f);

      toDWORD(ret.filenameOffset);
      toWORD(ret.unknown);
      toWORD(ret.numFileEntries);
      toDWORD(ret.firstFileEntryOffset);

      return ret;
    }

    FileEntry getFileEntry(int i, const RarcHeader& h, FILE* f)
    {
      fseek(f, h.fileEntriesOffset + i*sizeof(FileEntry)+0x20, SEEK_SET);
      FileEntry ret;
      fread(&ret, 1, sizeof(FileEntry), f);

      toWORD(ret.id);
      toWORD(ret.unknown);
      toWORD(ret.unknown2);
      toWORD(ret.filenameOffset);
      toDWORD(ret.dataOffset);
      toDWORD(ret.dataSize);
      toDWORD(ret.zero);

      return ret;
    }

    void dumpNode(const Node& n, const RarcHeader& h, FILE* f)
    {
      std::string nodeName = getString(n.filenameOffset + h.stringTableOffset + 0x20, f);

      boost::filesystem::create_directory(nodeName);
      boost::filesystem::current_path(boost::filesystem::absolute(boost::filesystem::path(nodeName)));

      for (int i = 0; i < n.numFileEntries; ++i)
      {
        FileEntry curr = getFileEntry(n.firstFileEntryOffset + i, h, f);

        if (curr.id == 0xFFFF) //subdirectory
        {
          if (curr.filenameOffset != 0 && curr.filenameOffset != 2) //don't go to "." and ".."
            dumpNode(getNode(curr.dataOffset, f), h, f);
        }
        else //file
        {
          std::string currName = getString(curr.filenameOffset + h.stringTableOffset + 0x20, f);
          std::cout << nodeName << "/" << currName << std::endl;

          std::ofstream dest(currName, std::ios::binary);
          //fopen_s(&dest, currName.c_str(), "wb");

          uint32_t read = 0;
          uint8_t buff[1024];
          fseek(f, curr.dataOffset + h.dataStartOffset + 0x20, SEEK_SET);
          while (read < curr.dataSize)
          {
            int r = fread(buff, 1, std::min(1024u, curr.dataSize - read), f);

            dest.write(reinterpret_cast<char *>(&buff[0]), r);
            //fwrite(buff, 1, r, dest);
            read += r;
          }
          dest.close();
        }
      }

      boost::filesystem::current_path(boost::filesystem::absolute(boost::filesystem::path("..")));
    }

    void readFile(std::ifstream& f)
    {
      //read header
      RarcHeader h;
      f.read(reinterpret_cast<char *>(&h), sizeof(h));
      //fread(&h, 1, sizeof(h), f);
      toDWORD(h.size);
      toDWORD(h.unknown);
      toDWORD(h.dataStartOffset);
      for (int i = 0; i < 4; ++i)
        toDWORD(h.unknown2[i]);

      toDWORD(h.numNodes);
      toDWORD(h.unknown3[0]);
      toDWORD(h.unknown3[1]);
      toDWORD(h.fileEntriesOffset);
      toDWORD(h.unknown4);
      toDWORD(h.stringTableOffset);
      toDWORD(h.unknown5[0]);
      toDWORD(h.unknown5[1]);

      Node root = getNode(0, f);

      dumpNode(root, h, f);
    };
  }

  void extract(std::string file)
  {
    //FILE *f = nullptr;
    //fopen_s(&f, file.c_str(), "rb");

    //if (f == nullptr)
    //{
    //  return;
    //}

    std::ifstream f(file, std::ios::binary);
    std::string dirName = file + std::string("_dir");

    boost::filesystem::create_directory(dirName);
    boost::filesystem::current_path(boost::filesystem::absolute(boost::filesystem::path(dirName)));

    detail::readFile(f);

    boost::filesystem::current_path(boost::filesystem::absolute(boost::filesystem::path("..")));
    //fclose(f);
    f.close();
  }
}
*/