//
//  SBJEV3DirectResults.h
//  LEGO Control
//
//  Created by David Giovannini on 12/20/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3Results.h"

#include <array>

namespace SBJ
{
namespace EV3
{

struct TypeMode
{
#pragma pack(push, 1)
	struct InputType
	{
		UBYTE type;
		UBYTE mode;
	};
#pragma pack(pop)
	
	using Input = InputType;
	using Output = Input;
	
	constexpr static size_t ResultCount = 2;
	
	const static size_t allocatedSize(size_t resultIdx)
	{
		return sizeof(UBYTE);
	}
	
	static inline void convert(const Input* input, Output& o, size_t)
	{
		o = *input;
	};
};
	
// This is an example of an opcode with two results
struct TachoSpeed
{
#pragma pack(push, 1)
	struct InputType
	{
		UWORD speed;
		UWORD count;
	};
#pragma pack(pop)
	
	using Input = InputType;
	using Output = Input;
	
	constexpr static size_t ResultCount = 2;
	
	const static size_t allocatedSize(size_t resultIdx)
	{
		return sizeof(UWORD);
	}
	
	static inline void convert(const Input* input, Output& o, size_t)
	{
		o = *input;
	};
};
	
}
}
