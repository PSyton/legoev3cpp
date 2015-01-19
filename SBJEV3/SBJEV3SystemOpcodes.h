//
//  SBJEV3SystemOpcodes.h
//  Jove's Landing
//
//  Created by David Giovannini on 1/7/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3SystemResults.h"

namespace SBJ
{
namespace EV3
{

// TODO: Implement the 15 System Opcodes

#pragma pack(push, 1)

template <size_t MaxSize, size_t MinLen = 0>
struct SysString : public VariableSizedEntity
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
	
	size_t size() const
	{
		size_t len = ::strlen((const char*)this);
		assert(len >= MinLen);
		return len + 1;
	}
private:
	std::array<char, MaxSize> _data;
};


template <UBYTE CmdCode, UWORD ChunkSize, typename ResultType>
struct UploadOpcode : public VariableSizedEntity
{
	constexpr static UWORD BaseSize = ChunkSize;
	
	size_t size() const
	{
		return (sizeof(*this) - sizeof(resource)) + resource.size();
	}
	
	const UBYTE code = CmdCode;
	const UWORD readSize = ChunkSize;
	SysString<vmPATHSIZE> resource;
	
	using Result = ResultType;
};

template <UBYTE CmdCode, UWORD ChunkSize, typename ResultType>
struct ContinueOpcode
{
	const UBYTE code = CmdCode;
	UWORD handle = 0;
	const UWORD readSize = ChunkSize;
	
	using Result = ResultType;
};

template <UWORD ChunkSize>
using BeginUpload = UploadOpcode<BEGIN_UPLOAD, ChunkSize, UploadBeganResult<BEGIN_UPLOAD, ChunkSize>>;

template <UWORD ChunkSize>
using ContinueUpload = ContinueOpcode<CONTINUE_UPLOAD, ChunkSize, UploadContunuedResult<CONTINUE_UPLOAD, ChunkSize>>;

using ListFiles = UploadOpcode<LIST_FILES, DirectoryResult::allocatedSize(0), DirectoryResult>;

#pragma pack(pop)
	
}
}