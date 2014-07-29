#ifndef _GAMECUBE_HEADER_H
#define _GAMECUBE_HEADER_H

#include <cstdint>
#include <fstream>
#include <vector>
#include <memory>

#include "gamecube_except.h"

#include "helpers.h"

namespace GamecubeOffset
{
	const uint32_t NameSize = 0x03E0;
	enum Gamecube
	{
		ConsoleID = 0x000,
		Gamecode = 0x001,
		CountryCode = 0x003,
		MakerCode = 0x004,
		DiskID = 0x006,
		Version = 0x007,
		AudoStreaming = 0x008,
		StreamBufferSize = 0x009,
		MagicWord = 0x01C,
		Name = 0x020,
		DebugOffset = 0x400,
		DebugAddress = 0x404,
		DOLOffset = 0x420,
		FSTOffset = 0x424,
		FSTSize = 0x428,
		FSTMaxSize = 0x42C,
		UserPosition = 0x430,
		UserLength = 0x434,
		Unknown = 0x438
	};
}

class GamecubeHeader
{
public:
	static const uint32_t FileSize = 0x57058000;	//	1459978240 | All gamecube discs must be 1.4GB
	static const uint32_t Size = 0x440;				//	Size of the Gamecube header
  static const uint32_t Magic = 0xC2339F3D; // Magic word

	GamecubeHeader();
	GamecubeHeader(std::string file);
	~GamecubeHeader();

	void initialize(std::string file);

  bool valid();

	uint8_t console_id();
	uint16_t gamecode();
	uint8_t country_code();

	std::string maker_code();

	uint8_t disk_id();
	uint8_t version();
	uint8_t audio_streaming();
	uint8_t stream_buffer_size();

	uint32_t magic();
	std::string name();

	uint32_t debug_offset();
	uint32_t debug_load_addr();

	uint32_t dol_offset();
	uint32_t fst_offset();
	uint32_t fst_size();
	uint32_t fst_max_size();

	uint32_t user_position();
	uint32_t user_length();

	uint32_t unknown();
private:
	std::vector<uint8_t> header;
};

inline bool GamecubeHeader::valid()
{
  return this->magic() == GamecubeHeader::Magic;
}

inline uint8_t GamecubeHeader::console_id()
{
  return header[GamecubeOffset::ConsoleID];
}

inline uint16_t GamecubeHeader::gamecode()
{
  return Util::read_big<uint16_t>(ref(header), GamecubeOffset::Gamecode);
}

inline uint8_t GamecubeHeader::country_code()
{
  return header[GamecubeOffset::CountryCode];
}

inline std::string GamecubeHeader::maker_code()
{
  return Util::read(ref(header), GamecubeOffset::MakerCode, 2);
}

inline uint8_t GamecubeHeader::disk_id()
{
  return header[GamecubeOffset::DiskID];
}

inline uint8_t GamecubeHeader::version()
{
  return header[GamecubeOffset::Version];
}

inline uint8_t GamecubeHeader::audio_streaming()
{
  return header[GamecubeOffset::AudoStreaming];
}

inline uint8_t GamecubeHeader::stream_buffer_size()
{
  return header[GamecubeOffset::StreamBufferSize];
}

inline uint32_t GamecubeHeader::magic()
{
  return Util::read_big<uint32_t>(ref(header), GamecubeOffset::MagicWord);
}

inline std::string GamecubeHeader::name()
{
  return Util::read(ref(header), GamecubeOffset::Name, GamecubeOffset::NameSize);
}

inline uint32_t GamecubeHeader::debug_offset()
{
  return Util::read_big<uint32_t>(ref(header), GamecubeOffset::DebugOffset);
}

inline uint32_t GamecubeHeader::debug_load_addr()
{
  return Util::read_big<uint32_t>(ref(header), GamecubeOffset::DebugAddress);
}

inline uint32_t GamecubeHeader::dol_offset()
{
  return Util::read_big<uint32_t>(ref(header), GamecubeOffset::DOLOffset);
}

inline uint32_t GamecubeHeader::fst_offset()
{
  return Util::read_big<uint32_t>(ref(header), GamecubeOffset::FSTOffset);
}

inline uint32_t GamecubeHeader::fst_size()
{
  return Util::read_big<uint32_t>(ref(header), GamecubeOffset::FSTSize);
}

inline uint32_t GamecubeHeader::fst_max_size()
{
  return Util::read_big<uint32_t>(ref(header), GamecubeOffset::FSTMaxSize);
}

inline uint32_t GamecubeHeader::user_position()
{
  return Util::read_big<uint32_t>(ref(header), GamecubeOffset::UserPosition);
}

inline uint32_t GamecubeHeader::user_length()
{
  return Util::read_big<uint32_t>(ref(header), GamecubeOffset::UserLength);
}

inline uint32_t GamecubeHeader::unknown()
{
  return Util::read_big<uint32_t>(ref(header), GamecubeOffset::Unknown);
}

#endif
