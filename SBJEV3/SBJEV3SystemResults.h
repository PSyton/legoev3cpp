//
//  SBJEV3SystemResults.h
//  LEGO Control
//
//  Created by David Giovannini on 12/20/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3Results.h"
#include "SBJEV3Hex.h"

#include <array>
#include <vector>

namespace SBJ
{
namespace EV3
{

#define DIREXT std::string("/")
#define EXEEXT std::string(".rbf")
#define DATALOGEXT std::string(".raf")
#define CURRENTDIR std::string("./")
#define PARENTDIR std::string("../")
#define ROOTDIR std::string("/home/root/lms2012/")
#define TOOLDIR std::string("/home/root/lms2012/tools/")
#define SYSDIR std::string("/home/root/lms2012/sys/")
#define SOURCEDIR std::string("/home/root/lms2012/source/")
#define APPDIR std::string("/home/root/lms2012/apps/")
#define PROJDIR std::string("/home/root/lms2012/prjs/")

#pragma pack(push, 1)
struct SysResource
{
	UBYTE code;
	UBYTE status;
	ULONG size;
	UBYTE handle;
};
/*
struct SysContinue
{
	UBYTE code;
	UBYTE status;
	UBYTE handle;
};
*/
#pragma pack(pop)

#pragma pack(push, 1)
template <size_t ChunkSize>
struct SysChunkResource : public SysResource
{
	std::array<UBYTE, ChunkSize> data;
};
/*
template <size_t ChunkSize>
struct SysChunkContunue : public SysContinue
{
	std::array<UBYTE, ChunkSize> data;
};
*/
#pragma pack(pop)

struct SysDirEntry
{
	SysDirEntry()
	: hash({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0})
	, size(0)
	{
	}
	
	SysDirEntry(const std::string& line)
	: hash(Hash(line))
	, size(Size(line))
	, name(Name(line))
	{
	}
		
	std::array<UBYTE, 16> hash;
	ULONG size;
	std::string name;
	
	bool isDirectory() const { return name.back() == '/'; }
	
	std::string simpleName() const
	{
		if (name.back() == '/')
		{
			return name.substr(0, name.size()-1);
		}
		size_t ext = name.rfind('.');
		if (ext != std::string::npos && ext != 0)
		{
			return name.substr(0, ext);
		}
		return name;
	}
	
	std::string extension()
	{
		if (name.back() == '/')
		{
			return "/";
		}
		size_t ext = name.rfind('.');
		if (ext != std::string::npos && ext != 0)
		{
			return name.substr(ext);
		}
		return "";
	}
	
	std::string escapedName() const
	{
		return replace(name, " ", "\\ ");
	}
	
	std::string pathRelativeToSys(std::string path) const
	{
		if (path.find(ROOTDIR) != std::string::npos)
		{
			return "../" + path.substr(ROOTDIR.size()) + name;
		}
		return path + name;
	}
	
private:
	static inline std::array<UBYTE, 16> Hash(const std::string& line)
	{
		if (line.back() != '/')
		{
			return hexbytes<16>(line);
		}
		return {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	}
	
	static inline ULONG Size(const std::string& line)
	{
		if (line.back() != '/')
		{
			std::array<UBYTE, 4> bytes = hexbytes<4>(line.substr(34));
			std::reverse(bytes.begin(), bytes.end());
			ULONG l = *(ULONG*)&bytes;
			return l;
		}
		return 0;
	}
	
	static inline std::string Name(const std::string& line)
	{
		if (line.back() != '/')
		{
			return line.substr(42);
		}
		return line;
	}
};

struct SysDirResource : public SysResource
{
	std::vector<SysDirEntry> entries;
	
	void populate(const char* data, size_t len)
	{
		char line[1024];
		int j = 0;
		for (int i = 0; i < len; i++)
		{
			switch (data[i])
			{
				case '\n':
				{
					line[j] = 0;
					j = 0;
					SysDirEntry e(line);
					entries.push_back(e);
					break;
				}
				default:
					line[j] = data[i];
					j++;
			}
		}
		if (entries.size() == 0)
		{
			const std::string r = "../\n./\n";
			populate(r.c_str(), r.length());
		}
	}
};

template <size_t ChunkSize>
struct SysChunkResourceResult
{
	using Input = const char;
	using Output = SysChunkResource<ChunkSize>;
	
	constexpr static size_t ResultCount = 1;
	
	constexpr static size_t allocatedSize(size_t resultIdx)
	{
		return sizeof(Output);
	}
	
	static inline void convert(const Input* input, Output& output, size_t maxLen)
	{
		::memcpy(&output, input, sizeof(SysResource));
		// TODO: specialize content, check for errors
	};
};
/*
template <size_t ChunkSize>
struct SysChunkContinueResult
{
	using Input = const char;
	using Output = SysChunkContunue<ChunkSize>;
	
	constexpr static size_t ResultCount = 1;
	
	constexpr static size_t allocatedSize(size_t resultIdx)
	{
		return sizeof(Output);
	}
	
	static inline void convert(const Input* input, Output& output, size_t maxLen)
	{
		::memcpy(&output, input, sizeof(SysResource));
		// TODO: specialize content, check for errors
	};
};
*/
struct SysDirListingResult
{
	using Input = const char;
	using Output = SysDirResource;
	
	constexpr static size_t ResultCount = 1;
	
	constexpr static size_t allocatedSize(size_t resultIdx)
	{
		return 1014; // according to docs lust cannot excede this size
	}
	
	static inline void convert(const Input* input, Output& output, size_t maxLen)
	{
		::memcpy(&output, input, sizeof(SysResource));
		output.populate(input + sizeof(SysResource), maxLen - sizeof(SysResource));
	};
	
};

}
}
