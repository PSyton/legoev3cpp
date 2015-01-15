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

static inline size_t alignReply(size_t offset)
{
	return (((offset / 4 ) - 1) * 4) + 4;
}

enum class VarScope
{
	global,
	local
};

class VoidResult;

template <typename Result, VarScope Scope = VarScope::global>
struct OutputStore { using Type = typename Result::Output; };

template <typename Result>
struct OutputStore<Result, VarScope::local> { using Type = struct { }; };

template <typename Result>
struct StorageSpecs
{
	constexpr static inline size_t localCount()
	{
		return Result::ResultCount * (Result::Scope == VarScope::local ? 1 : 0);
	}
	
	constexpr static inline size_t globalCount()
	{
		return Result::ResultCount * (Result::Scope == VarScope::global ? 1 : 0);
	}
	
	constexpr static inline size_t scopedCount()
	{
		return Result::ResultCount;
	}
	
	using Output = typename OutputStore<Result, Result::Scope>::Type;
};

/*
 * The Result structures do not require any storage space.
 * They define types and starategies used for defining how the EV3
 * produces a reply and how the reply items are converted to high level types.
 */

struct VoidResult
{
	// Type provided by EV3 in response buffer
	using Input = struct { };
	
	// Desired type output from result tuple
	using Output = Input;
	
	// Provide the required allocated size for each non-contiguous result
	constexpr static size_t allocatedSize(size_t resultIdx) { return 0; };
	
	// Convert the value
	static inline void convert(const Input*, Output&, size_t) { };
};


template <>
struct StorageSpecs<VoidResult>
{
	constexpr static inline size_t localCount()
	{
		return 0;
	}
	
	constexpr static inline size_t globalCount()
	{
		return 0;
	}
	
	constexpr static inline size_t scopedCount()
	{
		return 0;
	}
	
	using Output = typename VoidResult::Output;
};

template<VarScope varScope, typename InputType, typename OutputType = InputType>
struct BasicResult
{
	using Input = InputType;
	using Output = OutputType;
	
	constexpr static VarScope Scope = varScope;
	constexpr static size_t ResultCount = 1;
	
	constexpr static size_t allocatedSize(size_t resultIdx)
	{
		return sizeof(Input);
	}
	
	static inline void convert(const Input* input, Output& output, size_t)
	{
		output = static_cast<Output>(*input);
	};
};

template <VarScope varScope, size_t MaxSize>
struct StringResult
{
	using Input = const char;
	using Output = std::string;
	
	constexpr static VarScope Scope = varScope;
	constexpr static size_t ResultCount = 1;
	
	constexpr static size_t allocatedSize(size_t resultIdx)
	{
		return MaxSize;
	}
	
	static inline void convert(const Input* input, Output& output, size_t)
	{
		output = input;
	};
};

// Contiguous results...
template<VarScope varScope, typename InputType, size_t Count, typename OutputType = InputType>
struct ArrayResult
{
	using Input = InputType;
	using Output = std::array<OutputType, Count>;
	
	constexpr static VarScope Scope = varScope;
	constexpr static size_t ResultCount = 1;
	
	constexpr static size_t allocatedSize(size_t resultIdx)
	{
		return Count * sizeof(Input);
	}
	
	static inline void convert(const Input* input, Output& output, size_t)
	{
		for (size_t i = 0; i < Count; i++)
		{
			output[i] = static_cast<OutputType>(input[i]);
		}
	};
};
	
}
}
