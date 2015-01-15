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
	UWORD bytesRead = 0;
	UBYTE data[ChunkSize];
	
	void read(const uint8_t* input, size_t len)
	{
		::memcpy(this, input, sizeof(FileBegan));
		size_t payLoadLen = len - sizeof(FileBegan);
		if (payLoadLen > 0)
		{
			bytesRead = payLoadLen;
			::memcpy((void*)data, input + sizeof(FileBegan), payLoadLen);
		}
	}
};

template <UWORD ChunkSize>
struct UploadContunuedOutput : public FileContinued
{
	UWORD bytesRead = 0;
	UBYTE data[ChunkSize];
	
	void read(const uint8_t* input, size_t len)
	{
		::memcpy(this, input, sizeof(FileContinued));
		size_t payLoadLen = len - sizeof(FileContinued);
		if (payLoadLen > 0)
		{
			bytesRead = payLoadLen;
			::memcpy((void*)data, input + sizeof(FileContinued), payLoadLen);
		}
	}
};

struct DirectoryBeganOutput : public FileBegan
{
	std::vector<DirectoryEntry> entries;
	
	void read(const char* input, size_t len)
	{
		::memcpy(this, input, sizeof(FileBegan));
		entries = DirectoryEntry::read(input + sizeof(FileBegan), len - sizeof(FileBegan));
	};
};

// According to docs DirectoryContunuedOutput is not implemented

template <UWORD ChunkSize>
struct UploadBeganResult
{
	using Input = uint8_t;
	using Output = UploadBeganOutput<ChunkSize>;
	
	constexpr static VarScope Scope = VarScope::global;
	constexpr static size_t ResultCount = 1;
	
	constexpr static size_t allocatedSize(size_t resultIdx)
	{
		return sizeof(Output);
	}
	
	static inline void convert(const Input* input, Output& output, size_t maxLen)
	{
		output.read(input, maxLen);
	};
};

template <UWORD ChunkSize>
struct UploadContunuedResult
{
	using Input = uint8_t;
	using Output = UploadContunuedOutput<ChunkSize>;
	
	constexpr static VarScope Scope = VarScope::global;
	constexpr static size_t ResultCount = 1;
	
	constexpr static size_t allocatedSize(size_t resultIdx)
	{
		return sizeof(Output);
	}
	
	static inline void convert(const Input* input, Output& output, size_t maxLen)
	{
		output.read(input, maxLen);
	};
};

struct DirectoryResult
{
	
	using Input = char;
	using Output = DirectoryBeganOutput;
	
	constexpr static VarScope Scope = VarScope::global;
	constexpr static size_t ResultCount = 1;
	
	constexpr static size_t allocatedSize(size_t resultIdx)
	{
		return 1014; // according to docs list cannot excede this size
	}
	
	static inline void convert(const Input* input, Output& output, size_t maxLen)
	{
		output.read(input, maxLen);
	};
};

}
}
