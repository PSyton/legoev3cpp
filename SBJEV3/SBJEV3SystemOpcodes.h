//
//  SBJEV3SystemOpcodes.h
//  Jove's Landing
//
//  Created by David Giovannini on 1/7/15.
//  Copyright (c) 2015 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3Opcodes.h"

namespace SBJ
{
namespace EV3
{

#define ROOTDIR std::string("/home/root/lms2012/")
#define TOOLDIR std::string("/home/root/lms2012/tools/")
#define SYSDIR std::string("/home/root/lms2012/sys/")
#define SOURCEDIR std::string("/home/root/lms2012/source/")
#define APPDIR std::string("/home/root/lms2012/apps/")
#define PROJDIR std::string("/home/root/lms2012/prjs/")

template <size_t MaxSize, size_t MinLen = 0>
struct SystemStr
{
	SystemStr(const std::string& v = "")
	{
		size_t len = std::min(v.length(), MaxSize-1);
		for (int i = 0; i <= len; i++)
		{
			_data[i] = v[i];
		}
	}
	
	SystemStr& operator = (const std::string& v)
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
	
#pragma pack(push, 1)

// TODO: Implement the 15 System Opcodes

template <size_t ChunkSize>
struct SysResource
{
	UBYTE code;
	UBYTE status;
	ULONG size;
	UWORD handle;
	std::array<UBYTE, ChunkSize> data;
};

template <size_t ChunkSize>
struct SysResourceResult
{
	using Input = const char;
	using Output = SysResource<ChunkSize>;
	
	constexpr static size_t ResultCount = 1;
	
	constexpr static size_t allocatedSize(size_t resultIdx)
	{
		return sizeof(Output);
	}
	
	static inline void convert(const Input* input, Output& output)
	{
		::memcpy(&output, input, sizeof(Output)-ChunkSize);
	};
};

template <UBYTE CmdCode, size_t ChunkSize>
struct SysResourceOpcode : public VariableLenOpcode
{
	size_t pack(UBYTE* into) const
	{
		const size_t s = sizeof(*this) - resource.differential();
		if (into) ::memcpy(into, this, s);
		return s;
	}
	//static constexpr size_t MaxChunk = 64434 - (sizeof(COMCMD) - sizeof(COMCMD::CmdSize)) - sizeof(BeginUpload);
	const UBYTE code = CmdCode;
	const UWORD chunkSize = ChunkSize;
	SystemStr<vmPATHSIZE> resource;
	using Result = SysResourceResult<ChunkSize>;
};

using ListFiles = SysResourceOpcode<LIST_FILES, 1014>;
using BeginUpload = SysResourceOpcode<BEGIN_UPLOAD, 60000>;

#pragma pack(pop)
	
}
}