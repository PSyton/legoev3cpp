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

template<VarScope varScope = VarScope::global>
struct TypeMode
{
#pragma pack(push, 1)
	struct InputType
	{
		UBYTE type = TYPE_ERROR;
		UBYTE mode = MODE_KEEP;
	};
#pragma pack(pop)
	
	using Input = InputType;
	using Output = Input;
	
	constexpr static VarScope Scope = varScope;
	constexpr static size_t ResultCount = 2;
	
	const static size_t allocatedSize(size_t resultIdx)
	{
		return sizeof(UBYTE);
	}
	
	static inline void convert(const Input* input, Output& output, size_t)
	{
		if (input == nullptr) return;
		output = *input;
	};
};
	
template<VarScope varScope = VarScope::global>
struct TachoSpeed
{
#pragma pack(push, 1)
	struct InputType
	{
		UWORD speed = 0;
		UWORD count = 0;
	};
#pragma pack(pop)
	
	using Input = InputType;
	using Output = Input;
	
	constexpr static VarScope Scope = varScope;
	constexpr static size_t ResultCount = 2;
	
	const static size_t allocatedSize(size_t resultIdx)
	{
		return sizeof(UWORD);
	}
	
	static inline void convert(const Input* input, Output& output, size_t)
	{
		if (input == nullptr) return;
		output = *input;
	};
};
	
}
}
