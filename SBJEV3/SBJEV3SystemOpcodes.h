//
//  SBJEV3SystemOpcodes.h
//  Jove's Landing
//
//  Created by David Giovannini on 1/7/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3Opcodes.h"
#include "SBJEV3SystemResults.h"

namespace SBJ
{
namespace EV3
{

// TODO: Implement the 15 System Opcodes

#pragma pack(push, 1)

template <size_t MaxSize, size_t MinLen = 0>
struct SysString
{
	SysString(const std::string& v = "")
	{
		size_t len = std::min(v.length(), MaxSize-1);
		for (int i = 0; i <= len; i++)
		{
			_data[i] = v[i];
		}
	}
	
	SysString& operator = (const std::string& v)
	{
		size_t len = std::min(v.length(), MaxSize-1);
		for (int i = 0; i <= len; i++)
		{
			_data[i] = v[i];
		}
		return *this;
	}
		
	size_t differential() const
	{
		size_t len = ::strlen((const char*)this);
		assert(len >= MinLen);
		return MaxSize - (len + 1);
	}
private:
	std::array<char, MaxSize> _data;
};


template <UBYTE CmdCode, size_t ChunkSize, typename ResultType>
struct UploadOpcode : public VariableLenOpcode
{
	size_t pack(UBYTE* into) const
	{
		const size_t s = sizeof(*this) - resource.differential();
		if (into) ::memcpy(into, this, s);
		return s;
	}
	
	const UBYTE code = CmdCode;
	const UWORD chunkSize = ChunkSize;
	SysString<vmPATHSIZE> resource;
	
	using Result = ResultType;
};

template <UBYTE CmdCode, size_t ChunkSize, typename ResultType>
struct ContinueOpcode
{
	const UBYTE code = CmdCode;
	UWORD handle = 0;
	UWORD readSize = ChunkSize;
	
	using Result = ResultType;
};

template <size_t ChunkSize>
using BeginUpload = UploadOpcode<BEGIN_UPLOAD, ChunkSize, UploadBeganResult<ChunkSize>>;

template <size_t ChunkSize>
using ContinueUpload = ContinueOpcode<CONTINUE_UPLOAD, ChunkSize, UploadContunuedResult<ChunkSize>>;

using ListFiles = UploadOpcode<LIST_FILES, DirectoryResult::allocatedSize(0), DirectoryResult>;

#pragma pack(pop)
	
}
}