//
//  SBJEV3SystemResults.h
//  LEGO Control
//
//  Created by David Giovannini on 12/20/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3Results.h"
#include "SBJEV3DirectoryEntry.h"

#include <array>

namespace SBJ
{
namespace EV3
{

#pragma pack(push, 1)

struct FileBegan
{
	UBYTE code;
	UBYTE status;
	ULONG size;
	UBYTE handle;
};

struct FileContinued
{
	UBYTE code;
	UBYTE status;
	UBYTE handle;
};

#pragma pack(pop)

template <size_t ChunkSize>
struct UploadBeganOutput : public FileBegan
{
	std::array<UBYTE, ChunkSize> data;
};

template <size_t ChunkSize>
struct UploadContunuedOutput : public FileContinued
{
	std::array<UBYTE, ChunkSize> data;
};

struct DirectoryBeganOutput : public FileBegan
{
	std::vector<DirectoryEntry> entries;
};

// According to docs DirectoryContunuedOutput is not implemented

template <size_t ChunkSize>
struct UploadBeganResult
{
	using Input = const char;
	using Output = UploadBeganOutput<ChunkSize>;
	
	constexpr static size_t ResultCount = 1;
	
	constexpr static size_t allocatedSize(size_t resultIdx)
	{
		return sizeof(Output);
	}
	
	static inline void convert(const Input* input, Output& output, size_t maxLen)
	{
		::memcpy(&output, input, sizeof(FileBegan));
		if (maxLen - sizeof(FileBegan) > 0)
		{
			::memcpy(&output.data, input + sizeof(FileBegan), maxLen - sizeof(FileBegan));
		}
	};
};

template <size_t ChunkSize>
struct UploadContunuedResult
{
	using Input = const char;
	using Output = UploadContunuedOutput<ChunkSize>;
	
	constexpr static size_t ResultCount = 1;
	
	constexpr static size_t allocatedSize(size_t resultIdx)
	{
		return sizeof(Output);
	}
	
	static inline void convert(const Input* input, Output& output, size_t maxLen)
	{
		::memcpy(&output, input, sizeof(FileContinued));
		if (maxLen - sizeof(FileContinued) > 0)
		{
			::memccpy(&output.data, input + sizeof(FileContinued), maxLen - sizeof(FileContinued));
		}
	};
};

struct DirectoryResult
{
	using Input = const char;
	using Output = DirectoryBeganOutput;
	
	constexpr static size_t ResultCount = 1;
	
	constexpr static size_t allocatedSize(size_t resultIdx)
	{
		return 1014; // according to docs list cannot excede this size
	}
	
	static inline void convert(const Input* input, Output& output, size_t maxLen)
	{
		::memcpy(&output, input, sizeof(FileBegan));
		output.entries = DirectoryEntry::read(input + sizeof(FileBegan), maxLen - sizeof(FileBegan));
	};
	
};

}
}
