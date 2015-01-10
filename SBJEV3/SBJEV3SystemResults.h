//
//  SBJEV3SystemResults.h
//  LEGO Control
//
//  Created by David Giovannini on 12/20/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3Results.h"

#include <array>
#include <vector>

namespace SBJ
{
namespace EV3
{

#pragma pack(push, 1)
struct SysResource
{
	UBYTE code;
	UBYTE status;
	ULONG size;
	UBYTE handle;
};
#pragma pack(pop)

#pragma pack(push, 1)
template <size_t ChunkSize>
struct SysChunkResource : public SysResource
{
	std::array<UBYTE, ChunkSize> data;
};
#pragma pack(pop)

struct SysDirResource : public SysResource
{
	struct Entry
	{
		UBYTE hash[16];
		ULONG size;
		std::string name;
		bool isDirectory() const { return name.back() == '/'; }
		std::string simpleName() const
		{
			if (isDirectory())
			{
				return std::string(name).erase(name.size()-1);
			}
			return name;
		}
	};
	std::vector<Entry> entries;
	
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
					Entry e = {{}, 0, line};
					entries.push_back(e);
					break;
				}
				default:
					line[j] = data[i];
					j++;
			}
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

struct SysDirListingResult
{
	using Input = const char;
	using Output = SysDirResource;
	
	constexpr static size_t ResultCount = 1;
	
	constexpr static size_t allocatedSize(size_t resultIdx)
	{
		return 1014;
	}
	
	static inline void convert(const Input* input, Output& output, size_t maxLen)
	{
		::memcpy(&output, input, sizeof(SysResource));
		if (output.handle != 255)
		{
			output.populate(input + sizeof(SysResource), maxLen - sizeof(SysResource));
		}
	};
	
};

}
}
