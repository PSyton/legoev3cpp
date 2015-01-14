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

template <UWORD ChunkSize>
struct UploadBeganOutput : public FileBegan
{
	UBYTE data[ChunkSize];
	
	ULONG actualRead() const
	{
		return std::min((ULONG)ChunkSize, size);
	}
};

template <UWORD ChunkSize>
struct UploadContunuedOutput : public FileContinued
{
	UBYTE data[ChunkSize];
	
	ULONG actualRead(ULONG fileSize) const
	{
		ULONG remaining = fileSize - (ULONG)ChunkSize;
		return std::min(remaining, (ULONG)ChunkSize);
	}
};

struct DirectoryBeganOutput : public FileBegan
{
	std::vector<DirectoryEntry> entries;
};

// According to docs DirectoryContunuedOutput is not implemented

template <UWORD ChunkSize>
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
		size_t payLoadLen = maxLen - sizeof(FileBegan);
		if (payLoadLen > 0)
		{
			::memcpy(output.data, input + sizeof(FileBegan), payLoadLen);
		}
	};
};

template <UWORD ChunkSize>
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
		size_t payLoadLen = maxLen - sizeof(FileContinued);
		if (payLoadLen > 0)
		{
			::memcpy(output.data, input + sizeof(FileContinued), payLoadLen);
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
