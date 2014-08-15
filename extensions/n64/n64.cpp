#include "n64.h"

N64Corruption::N64Corruption()
{

}

N64Corruption::N64Corruption(std::string filename, std::vector<std::string>& args)
{
  this->initialize(filename, args);
}

N64Corruption::~N64Corruption()
{

}

void N64Corruption::initialize(std::string filename, std::vector<std::string>& args)
{
  // Read the rom file
  rom = util::read_file(filename);

  header = std::make_unique<N64Header>(rom);
  info = std::make_unique<CorruptionInfo>(args);

  // If the rom is not valid then throw an exception
  if (!valid())
  {
    throw InvalidN64RomException();
  }

  // Rom is valid, set up variables.
}

void N64Corruption::print_header()
{
  std::cout << std::hex;
  std::cout << "Domain:          " << this->header->domain() << std::endl;
  std::cout << "Clock Rate:      " << this->header->clock_rate() << std::endl;
  std::cout << "Program Counter: " << this->header->program_counter() << std::endl;
  std::cout << "Release:         " << this->header->release() << std::endl;
  std::cout << "CRC1:            " << this->header->crc1() << std::endl;
  std::cout << "CRC2:            " << this->header->crc2() << std::endl;
  std::cout << "Name:            " << this->header->name() << std::endl;
  std::cout << "Format:          " << this->header->format() << std::endl;
  std::cout << "ID:              " << this->header->id() << std::endl;
  std::cout << "Region:          " << this->header->region() << std::endl;
  std::cout << "Version:         " << this->header->version() << std::endl;
  std::cout << std::dec;
}

bool N64Corruption::valid()
{
  return true;
}

bool N64Corruption::valid_byte(uint8_t byte, uint32_t location)
{
  //  If location is in the header don't do anything
  if (location < N64Header::Size)
  {
    return false;
  }

  //  Get instruction which is bound to a 4 byte alignment
  uint32_t instruction = util::read<uint32_t>(ref(rom), location - (location % 4));

  //  Get first opcode which is between bits 31 and 26 ( 6 bits total )
  uint8_t opcode = (instruction >> 24) & 0xFC;
  uint8_t high = opcode & 0x38;
  uint8_t low = opcode & 0x07;

  /*
        31---------26---------------------------------------------------0
        |  opcode   |                                                   |
        ------6----------------------------------------------------------
        |--000--|--001--|--010--|--011--|--100--|--101--|--110--|--111--| lo
    000 | *1    | *2    | J     | JAL   | BEQ   | BNE   | BLEZ  | BGTZ  |
    001 | ADDI  | ADDIU | SLTI  | SLTIU | ANDI  | ORI   | XORI  | LUI   |
    010 | *3    | *4    | *5    |       | BEQL  | BNEL  | BLEZL | BGTZL |
    011 | DADDI |DADDIU |  LDL  |  LDR  |       |       |       |       |
    100 | LB    | LH    | LWL   | LW    | LBU   | LHU   | LWR   | LWU   |
    101 | SB    | SH    | SWL   | SW    | SDL   | SDR   | SWR   | CACHE |
    110 | LL    | LWC1  | LWC2  |       | LLD   | LDC1  | LDC2  | LD    |
    111 | SC    | SWC1  | SWC2  |       | SCD   | SDC1  | SDC2  | SD    |
     hi |-------|-------|-------|-------|-------|-------|-------|-------|
         *1 = SPECIAL, see SPECIAL list      *2 = REGIMM, see REGIMM list
         *3 = COP0                  *4 = COP1                   *5 = COP2
  */

  if (opcode == 0x00)       // *1 SPECIAL
  {
    uint8_t function = instruction & 0x3F;
    //  Block the following opcodes:
    //  SLL | SRL | SRA | SLLV | SRLV | SRAV | JR | JALR | SYSCALL | BREAK | SYNC
    if (function <= 0x0F)
    {
      return false;
    }
  }
  else if (opcode == 0x01)  // *2 REGIMM
  {
    return false; // No acceptable instructions under REGIMM
  }
  else if (opcode == 0x10)  // *3 COP0
  {
    //  Format is 5 bits from 25 to 21 in the instruction
    uint8_t format = instruction & 0x3E00000;
    uint8_t fmthigh = format & 0x18;
    uint8_t fmtlow = format & 0x07;

    if (fmthigh == 1)       //  *1 BC Instruction
    {
      return false; // No valid BC instructions
    }
    else if (fmthigh == 2)  //  *2 TLB Instruction
    {
      return false; // No valid TLB instructions
    }
  }
  else if (opcode == 0x11)  // *4 COP1
  {
    //  Format is 5 bits from 25 to 21 in the instruction
    uint8_t format = instruction & 0x3E00000;

    if (format == 8)
    {
      return false; // No valid BC instructions
    }
  }
  else if (opcode == 0x12)  // *4 COP2
  {
    //  No documentation on COP2 instructions from the resource I was given.
    return false; // In the mean while, return false to be safe.
  }

  return true;
}

void N64Corruption::save(std::string filename)
{
  std::string format = ".z64";

  //  If the filename doesn't include the format extension then add it.
  if (boost::filesystem::extension(filename) != format)
  {
    filename += format;
  }

  try
  {
    if (info->save_file() != "")
    {
      util::write_file(info->save_file(), rom);
      this->save_name = info->save_file();
    }
    else
    {
      util::write_file(filename, rom);
      this->save_name = filename;
    }
  }
  catch (InvalidFileNameException e)
  {
    std::cout << e.what() << std::endl;
    exit(EXIT_FAILURE);
  }
}

void N64Corruption::convert_z64(std::string filename)
{
  uint32_t size = static_cast<uint32_t>(boost::filesystem::file_size(filename));
  std::string extension = boost::filesystem::extension(filename);
  std::vector<uint8_t> data(size);

  std::ifstream ifs(filename, std::ios::binary);
  std::ofstream ofs(filename.substr(0, filename.length() - 3) + "z64", std::ios::binary);

  ifs.read(reinterpret_cast<char *>(&data[0]), size);

  if (extension == ".n64")
  {
    for (uint32_t i = 0; i < size; i += 2)
    {
      uint8_t temp = data[i];
      data[i] = data[i + 1];
      data[i + 1] = temp;
    }
  }
  else if (extension == ".v64")
  {

  }

  ofs.write(reinterpret_cast<char *>(&data[0]), size);

  ofs.close();
  ifs.close();
}