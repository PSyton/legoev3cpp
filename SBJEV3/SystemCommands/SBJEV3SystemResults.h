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

template <UBYTE BaseCode>
struct FileBegan
{
	UBYTE code = BaseCode;
	UBYTE status = UNKNOWN_ERROR;
	ULONG size = 0;
	UBYTE handle = 255;
	
	size_t read(const uint8_t* input, size_t len)
	{
		len = std::min(len, sizeof(*this));
		::memcpy((void*)this, input, len);
		return len;
	}
};

template <UBYTE BaseCode>
struct FileContinued
{
	UBYTE code = BaseCode;
	UBYTE status = UNKNOWN_ERROR;
	UBYTE handle = 255;
	
	size_t read(const uint8_t* input, size_t len)
	{
		len = std::min(len, sizeof(*this));
		::memcpy((void*)this, input, len);
		return len;
	}
};

#pragma pack(pop)

template <UBYTE BaseCode, UWORD ChunkSize>
struct UploadBeganOutput : public FileBegan<BaseCode>
{
	UWORD bytesRead = 0;
	UBYTE data[ChunkSize];
	
	void read(const uint8_t* input, size_t len)
	{
		size_t base = FileBegan<BaseCode>::read(input, len);
		size_t payLoadLen = len - base;
		if (payLoadLen > 0)
		{
			bytesRead = payLoadLen;
			::memcpy((void*)data, input + base, payLoadLen);
		}
	}
};

template <UBYTE BaseCode, UWORD ChunkSize>
struct UploadContunuedOutput : public FileContinued<BaseCode>
{
	UWORD bytesRead = 0;
	UBYTE data[ChunkSize];
	
	void read(const uint8_t* input, size_t len)
	{
		size_t base = FileContinued<BaseCode>::read(input, len);
		size_t payLoadLen = len - base;
		if (payLoadLen > 0)
		{
			bytesRead = payLoadLen;
			::memcpy((void*)data, input + base, payLoadLen);
		}
	}
};

struct DirectoryBeganOutput : public FileBegan<LIST_FILES>
{
	std::vector<DirectoryEntry> entries;
	
	void read(const uint8_t* input, size_t len)
	{
		size_t base = FileBegan::read((uint8_t*)input, len);
		size_t payLoadLen = len - base;
		entries = DirectoryEntry::read((const char*)(input + base), payLoadLen);
	};
};

// According to docs DirectoryContunuedOutput is not implemented

template <UBYTE BaseCode, UWORD ChunkSize>
struct UploadBeganResult
{
	using Input = uint8_t;
	using Output = UploadBeganOutput<BaseCode, ChunkSize>;
	
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

template <UBYTE BaseCode, UWORD ChunkSize>
struct UploadContunuedResult
{
	using Input = uint8_t;
	using Output = UploadContunuedOutput<BaseCode, ChunkSize>;
	
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
	using Input = uint8_t;
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
