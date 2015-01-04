//
//  SBJEV3Results.h
//  LEGO Control
//
//  Created by David Giovannini on 12/20/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3ByteCodes.h"

#include <array>

namespace SBJ
{
namespace EV3
{

/*
 * The Result structures do not require any storage space.
 * They define types and starategies used for defining how the EV3
 * produces a reply and how the reply items are converted to high level types.
 *
 * TODO: ResultCount and allocatedSize will likely have to be expanded to
 * help define local variable space.
 */

struct VoidResult
{
	// Type provided by EV3 in response buffer
	using Input = struct { };
	
	// Desired type output from result tuple
	using Output = Input;
	
	// Some opodes produce several non-contiguous results
	constexpr static size_t ResultCount = 0;
	
	// Provide the required allocated size for each non-contiguous result
	const static size_t allocatedSize(size_t resultIdx) { return 0; };
	
	// Convert the value
	static inline void convert(const Input*, Output&) { };
};

template<typename InputType, typename OutputType = InputType>
struct BasicResult
{
	using Input = InputType;
	using Output = OutputType;
	
	constexpr static size_t ResultCount = 1;
	
	const static size_t allocatedSize(size_t resultIdx)
	{
		return sizeof(Input);
	}
	
	static inline void convert(const Input* input, Output& output)
	{
		output = static_cast<Output>(*input);
	};
};

template <size_t maxLen>
struct StringResult
{
	using Input = const char;
	using Output = std::string;
	
	constexpr static size_t ResultCount = 1;
	
	const static size_t allocatedSize(size_t resultIdx)
	{
		return maxLen + 1;
	}
	
	static inline void convert(const Input* input, Output& output)
	{
		output = input;
	};
};

// Contiguous results...
template<typename InputType, size_t Count, typename OutputType = InputType>
struct ArrayResult
{
	using Input = InputType;
	using Output = std::array<OutputType, Count>;
	
	constexpr static size_t ResultCount = 1;
	
	const static size_t allocatedSize(size_t resultIdx)
	{
		return Count * sizeof(Input);
	}
	
	static inline void convert(const Input* input, Output& output)
	{
		for (size_t i = 0; i < Count; i++)
		{
			output[i] = static_cast<OutputType>(input[i]);
		}
	};
};

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
	
	static inline void convert(const Input* input, Output& o)
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
	
	static inline void convert(const Input* input, Output& o)
	{
		o = *input;
	};
};
	
}
}
