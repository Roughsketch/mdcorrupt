/*
  VineCorrupt License

  Copyright (c) 2014 maiddog.com

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  1. Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

  3. The name of the author may not be used to endorse or promote products
  derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER "AS IS" AND ANY EXPRESS
  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
  THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <iostream>
#include <memory>
#include <string>
#include <regex>

#include "corrupt.h"

#include "log.h"

#include "nintendo/yaz0.h"
/*
  Takes a class type derived from Corruption and forms a template to execute the corruption

  @type T - a type derived from Corruption
  @param filename - filename or fully qualified path of the file to corrupt
  @param args - Custom arguments that will be passed to the given class T
*/
template<typename T> void corrupt(std::string filename, std::vector<std::string> args)
{
  //  Give off appropriate errors if this template is compiled with a bad type.
  static_assert(std::is_base_of<Corruption, T>::value, "function only takes a derived class of Corruption.");
  //static_assert(std::is_default_constructible<T>::value, "class must have default constructor.");

  debug::cout << "Initializing." << std::endl;
  std::unique_ptr<T> rom = std::make_unique<T>(filename, args);

	try
  {
    //debug::cout << "Initializing." << std::endl;
    //rom->initialize(filename, args);
    //debug::cout << "Parsing." << std::endl;
    //rom->parse(args);
    //debug::cout << "Printing." << std::endl;
    //rom->print_header();

    debug::cout << "Corrupting." << std::endl;
    rom->corrupt();

    rom->save("output");

    //rom->run();
	}
  catch (const std::exception& e)
	{
    std::cerr << "Exception: " << e.what() << std::endl;
  }
}

/*
  Removes any unwanted temp files that were created.
*/
void clean()
{
  std::regex tempfile("tmp_[0-9]{10}\\.");  //  Match tmp_##########.*

  for (boost::filesystem::recursive_directory_iterator dir_end, dir("."); dir != dir_end; ++dir)
  {
    try
    {
      auto file = boost::filesystem::path(*dir);

      if (boost::filesystem::is_regular_file(file))
      {
        if (std::regex_search(file.string(), tempfile))
        {
          boost::filesystem::remove(file);
        }
      }
    }
    catch (boost::filesystem::filesystem_error e)
    {
      std::cout << "Loop: " << e.what() << "\t" << BOOST_CURRENT_FUNCTION << std::endl;
    }
  }
}

void usage(char *name)
{
  std::cout << "Usage: " << name << " <Rom File> <Corruption Args>";
  std::cout << R"DOC(
  Basic corruption arguments:
    (--step, -s)  <x>: Steps x bytes after each corruption.
    (--shift, -h) <x>: Replaces a byte with the byte x spaces away.
    (--swap, -w)  <x>: Swaps the current byte and another x spaces away.
    (--add, -a)   <x>: Adds a static value to the current byte.
    (--set, -t)   <x>: Sets the current byte to a static value.
    (--random)       : Will assign a randomly generated value to the current byte.

  Note: For NES corruptions you will need to prepend a p/c or prg/chr to the argument.
    Ex: --step is either --prg-step | -ps or --chr-step | -cs
  )DOC" << std::endl;
}

std::vector<std::string> split_args(std::string args)
{
  bool quote = false;
  std::vector<std::string> ret;
  std::string arg;

  for (char c : args)
  {
    if (c == '"')
    {
      quote = !quote;
    }
    else
    {
      if (c == ' ' && !quote)
      {
        if (arg != "")
        {
          ret.push_back(arg);
          arg = "";
        }
      }
      else
      {
        arg += c;
      }
    }
  }

  if (arg != "")
  {
    ret.push_back(arg);
  }

  return ret;
}

void parse(std::vector<std::string> args)
{
  std::string file(args[0]);
  std::string extension = boost::filesystem::extension(file);

  args.erase(args.begin()); //  Remove the index that holds the filename

  if (std::find(args.begin(), args.end(), "--nintendo") != args.end())
  {
    debug::cout << "Nintendo" << std::endl;
    args.erase(std::remove(args.begin(), args.end(), "--nintendo"), args.end());

    NintendoFile::start(file, args);
  }
  else if (std::find(args.begin(), args.end(), "--gamecube") != args.end())
  {
    debug::cout << "GameCube" << std::endl;
    args.erase(std::remove(args.begin(), args.end(), "--gamecube"), args.end());
    corrupt<GamecubeCorruption>(file, args);
  }
  else if (std::find(args.begin(), args.end(), "--playstation") != args.end())
  {
    debug::cout << "Playstation" << std::endl;
    args.erase(std::remove(args.begin(), args.end(), "--playstation"), args.end());
    corrupt<PSXCorruption>(file, args);
  }
  else if (extension == ".nes")
  {
    debug::cout << "NES" << std::endl;
    corrupt<NESCorruption>(file, args);
  }
  else if (extension == ".smc" || extension == ".sfc")
  {
    debug::cout << "SNES" << std::endl;
    corrupt<SNESCorruption>(file, args);
  }
  else if (extension == ".z64")
  {
    debug::cout << "N64" << std::endl;
    corrupt<N64Corruption>(file, args);
  }
  //else if (extension == ".n64")
  //{
  //  N64Corruption::convert_z64(file);
  //}
  else if (extension == ".gbc" || extension == ".gb")
  {
    debug::cout << "Game Boy" << std::endl;
    corrupt<GBCCorruption>(file, args);
  }
  else if (extension == ".gba")
  {
    debug::cout << "Game Boy Advance" << std::endl;
    corrupt<GBACorruption>(file, args);
  }
  else if (extension == ".nds")
  {
    debug::cout << "Nintendo DS" << std::endl;
    corrupt<NDSCorruption>(file, args);
  }
  else if (extension == ".gcm")
  {
    debug::cout << "GameCube" << std::endl;
    corrupt<GamecubeCorruption>(file, args);
  }
  else if (extension == ".img" || (extension == ".bin" && boost::filesystem::file_size(file) > (16 * 0x100000)))
  {
    debug::cout << "Playstation" << std::endl;
    corrupt<PSXCorruption>(file, args);
  }
  else if (extension == ".bin" || extension == ".md" || extension == ".smd")
  {
    debug::cout << "Sega Mega Drive / Sega Genesis" << std::endl;
    corrupt<GenesisCorruption>(file, args);
  }
  else if (extension == ".cdi")
  {
    debug::cout << "Normal" << std::endl;
    corrupt<DreamcastCorruption>(file, args);
  }
  else
  {
    corrupt<Corruption>(file, args);
  }
}

int main(int argc, char *argv[])
{
  std::cout << "Corrupter v0.8" << std::endl;

  if (argc == 1 || argc >= 2 && strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
  {
    usage(argv[0]);
    exit(EXIT_FAILURE);
  }

  std::string file(argv[1]);
  std::string extension(boost::filesystem::extension(file));
  std::vector<std::string> args;

  for (int i = 1; i < argc; i++)
  {
    debug::cout << "Found arg: " << argv[i] << std::endl;
    args.push_back(argv[i]);
  }

  //  If debug is active then print extra info.
  if (std::find(args.begin(), args.end(), "--debug") != args.end())
  {
    args.erase(std::remove(args.begin(), args.end(), "--debug"), args.end());
    debug::cout.enable();
    debug::cerr.enable();
  }
  else
  {
    debug::cout.disable();
    debug::cerr.disable();
  }

  debug::cout << "file:   " << file << std::endl;
  debug::cout << "Extension: " << extension << std::endl;
  debug::cout << "Type: ";

  //  If this is a batch execution then run each portion individually.
  if (std::find(args.begin(), args.end(), "--batch") != args.end())
  {
    std::string templine;
    std::vector<std::string> batchargs;
    std::ifstream ifs(file);

    //  Get each line of the batch into a vector
    while (std::getline(ifs, templine))
    {
      batchargs.push_back(templine);
    }

    //  Pass each line of the batch file into the parser
    for (auto& line : batchargs)
    {
      parse(split_args(line));
    }
  }
  else
  {
    parse(args);
  }

  clean();

  return 0;
}